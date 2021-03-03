/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: lrns.c
*/

#include <string.h>
#include <math.h>
#include "stm32f10x.h"
#include "main.h"
#include "service.h"
#include "settings.h"
#include "gps.h"
#include "points.h"
#include "lrns.h"
#include "si4463.h"
#include "gpio.h"



//(bit) start position in "flag" variable
#define FLAGS_BATTERY_POS       (0)
#define FLAGS_GPS_FIX_POS       (3)
#define FLAGS_PDOP_POS          (4)
#define FLAGS_ALARM_POS         (5)

//bit masks in "flag" variable
#define FLAGS_BATTERY_MASK      (0x07)
#define FLAGS_GPS_FIX_MASK      (0x08)
#define FLAGS_PDOP_MASK         (0x10)
#define FLAGS_ALARM_MASK        (0x20)



//Air packet structure and fields position (must match structure "gps_air")
#define PACKET_DEVICE_NUMBER_POS    (0)
#define PACKET_DEVICE_ID_POS        (1)
#define PACKET_FLAGS_POS            (3)
#define PACKET_LATITUDE_POS         (4)
#define PACKET_LONGITUDE_POS        (8)
#define PACKET_ALTITUDE_POS         (12)
#define PACKET_SPEED_POS            (14)
#define PACKET_COURSE_POS           (15)



#define PDOP_THRESHOLD				(6.0)



const double rad_to_deg = 57.29577951308232;        //rad to deg multiplyer
const double deg_to_rad = 0.0174532925199433;       //deg to rad multiplyer

const double twice_mean_earth_radius = 12742016.0;  // 2 * 6371008 meters
const double pi_div_by_4 = 0.7853981633974483;      // pi / 4



struct gps_air_struct gps_air[DEVICES_IN_GROUP + 1];        //structure for devices from 1 to DEVICES_IN_GROUP. Index 0 is invalid and always empty
struct gps_air_struct *p_gps_air[DEVICES_IN_GROUP + 1];

struct gps_rel_struct gps_rel[DEVICES_IN_GROUP + 1];        //structure with relative coordinates. Indexing is the same as above
struct gps_rel_struct *p_gps_rel[DEVICES_IN_GROUP + 1];

struct dev_aux_struct dev_aux[DEVICES_IN_GROUP + 1];        //structure with auxiliary information. Indexing is the same as above
struct dev_aux_struct *p_dev_aux[DEVICES_IN_GROUP + 1];

struct settings_struct *p_settings;
struct gps_num_struct *p_gps_num;
uint8_t *p_air_packet_tx;
uint8_t *p_air_packet_rx;

uint8_t device_number;



void init_lrns(void)
{
	//Clear mem
    for (uint8_t dev = 1; dev <= DEVICES_IN_GROUP; dev++)
    {
        memset(&gps_air[dev], 0, sizeof(gps_air[dev]));
        memset(&gps_rel[dev], 0, sizeof(gps_rel[dev]));
    }

	//Get external things
	p_settings = get_settings();
	p_gps_num = get_gps_num();
	p_air_packet_tx = get_air_packet_tx();
	p_air_packet_rx = get_air_packet_rx();

	//This device number
	device_number = p_settings->device_number;

    //Exist flag
	dev_aux[device_number].exist_flag = 1;
    
    //ID
    gps_air[device_number].device_id[0] = p_settings->device_id[0];		//note: ID loads only once at startup
    gps_air[device_number].device_id[1] = p_settings->device_id[1];
}



//Set this device flags
void set_device_flags(uint8_t parameter_to_set, uint8_t parameter_value)
{
	uint8_t mask;
	uint8_t pos;

    switch (parameter_to_set)
    {
        case FLAGS_BATTERY:
        	mask = FLAGS_BATTERY_MASK;
            pos = FLAGS_BATTERY_POS;
            break;

        case FLAGS_GPS_FIX:
        	mask = FLAGS_GPS_FIX_MASK;
            pos = FLAGS_GPS_FIX_POS;
            break;

        case FLAGS_PDOP:
        	mask = FLAGS_PDOP_MASK;
            pos = FLAGS_PDOP_POS;
            break;

        case FLAGS_ALARM:
        	mask = FLAGS_ALARM_MASK;
            pos = FLAGS_ALARM_POS;
            break;
    }

    gps_air[device_number].flags &= ~mask;             			//clear bits
	parameter_value &= (mask >> pos);   							//clear non-masked bits
	gps_air[device_number].flags |= (parameter_value << pos);  	//add to flags
}



//Get any device flags
uint8_t get_device_flags(uint8_t dev_num, uint8_t parameter_to_get)
{
	uint8_t mask;
	uint8_t pos;

    switch (parameter_to_get)
    {
		case FLAGS_BATTERY:
			mask = FLAGS_BATTERY_MASK;
			pos = FLAGS_BATTERY_POS;
			break;

		case FLAGS_GPS_FIX:
			mask = FLAGS_GPS_FIX_MASK;
			pos = FLAGS_GPS_FIX_POS;
			break;

		case FLAGS_PDOP:
			mask = FLAGS_PDOP_MASK;
			pos = FLAGS_PDOP_POS;
			break;

		case FLAGS_ALARM:
			mask = FLAGS_ALARM_MASK;
			pos = FLAGS_ALARM_POS;
			break;
    }

    return (gps_air[dev_num].flags & mask) >> pos;
}



//Check alarm status of each device and make red led on if any alarm exist
uint8_t check_alarms(void)
{
    uint8_t led_status = 0;

    for (uint8_t dev = 1; dev <= DEVICES_IN_GROUP; dev++)
    {
        if (get_device_flags(dev, FLAGS_ALARM) == FLAG_ALARM_ON)
        {
            led_status = 1;
        }
    }

    if (led_status == 1)
    {
        led_red_on();
        return 1;
    }
    else
    {
        led_red_off();
        return 0;
    }
}



void calc_timeout(uint32_t current_uptime)
{
    for (uint8_t dev = 1; dev <= DEVICES_IN_GROUP; dev++)
    {
        if (dev_aux[dev].exist_flag == 1)
        {
        	dev_aux[dev].timeout = current_uptime - dev_aux[dev].timestamp;
        }
    }
}



//Check devices timeout and set flags if needed
uint8_t check_timeout(void)
{
	uint8_t timeout_status = 0;

    for (uint8_t dev = 1; dev <= DEVICES_IN_GROUP; dev++)
    {
		if (dev_aux[dev].memory_point_flag == 0)	//no timeout alarm for memory points
		{
			if (p_settings->timeout_threshold.as_integer != TIMEOUT_ALARM_DISABLED)
			{
				if (dev_aux[dev].timeout > p_settings->timeout_threshold.as_integer)
				{
					dev_aux[dev].timeout_flag = 1;
					timeout_status = 1;
				}
				else
				{
					dev_aux[dev].timeout_flag = 0;
				}
			}
		}
    }

    if (timeout_status == 1)
    {
    	return 1;
    }
    else
    {
    	return 0;
    }
}



uint8_t check_fence(void)
{
	uint8_t fence_status = 0;

	for (uint8_t dev = 1; dev <= DEVICES_IN_GROUP; dev++)
	{
		if (dev_aux[dev].exist_flag)
		{
			if (gps_rel[dev].distance > p_settings->fence_threshold.as_integer)
			{
				dev_aux[dev].fence_flag = 1;
				fence_status = 1;
			}
			else
			{
				dev_aux[dev].fence_flag = 0;
			}
		}
	}

    if (fence_status == 1)
    {
    	return 1;
    }
    else
    {
    	return 0;
    }
}



void process_all_devices(void)
{
	for (uint8_t dev = 1; dev <= DEVICES_IN_GROUP; dev++)
	{
		if (dev_aux[dev].exist_flag == 1)	//process mem points too
		{
			calc_relative_position(dev);
		}
	}
}



//Update gps_air structure with coordinates of this device in order to transmit them further
void gps_air_update_my_data(uint32_t uptime)
{

    //Timestamp
	dev_aux[device_number].timestamp = uptime;
    

    //Flags
    if (p_gps_num->mode == GPS_POSITION_3DFIX)
    {
        set_device_flags(FLAGS_GPS_FIX, FLAG_GPS_FIX_3D);
    }
    else
    {
        set_device_flags(FLAGS_GPS_FIX, FLAG_GPS_FIX_2D);
    }
    
    if (p_gps_num->pdop <= PDOP_THRESHOLD)
    {
        set_device_flags(FLAGS_PDOP, FLAG_PDOP_GOOD);
    }
    else
    {
        set_device_flags(FLAGS_PDOP, FLAG_PDOP_BAD);
    }
    
    //Latitude & Longitude
    gps_air[device_number].latitude.as_float = p_gps_num->latitude.in_deg;
    gps_air[device_number].longitude.as_float = p_gps_num->longitude.in_deg;
    
    //Altitude
    gps_air[device_number].altitude.as_integer = (int16_t)p_gps_num->altitude;
    
    //Speed
    gps_air[device_number].speed = (uint8_t)p_gps_num->speed;
    
    //Course
    gps_air[device_number].course.as_integer = (uint16_t)p_gps_num->course;
}



//Copy gps_air struct data to the TX air packet
void fill_air_packet_with_struct_data(void)
{
	p_air_packet_tx[PACKET_DEVICE_NUMBER_POS] = 	device_number;             //this device number
	p_air_packet_tx[PACKET_DEVICE_ID_POS] = 		gps_air[device_number].device_id[0];
	p_air_packet_tx[PACKET_DEVICE_ID_POS + 1] = 	gps_air[device_number].device_id[1];
	p_air_packet_tx[PACKET_FLAGS_POS] = 			gps_air[device_number].flags;
	p_air_packet_tx[PACKET_LATITUDE_POS] = 			gps_air[device_number].latitude.as_array[0];
	p_air_packet_tx[PACKET_LATITUDE_POS + 1] = 		gps_air[device_number].latitude.as_array[1];
	p_air_packet_tx[PACKET_LATITUDE_POS + 2] = 		gps_air[device_number].latitude.as_array[2];
	p_air_packet_tx[PACKET_LATITUDE_POS + 3] = 		gps_air[device_number].latitude.as_array[3];
	p_air_packet_tx[PACKET_LONGITUDE_POS] = 		gps_air[device_number].longitude.as_array[0];
	p_air_packet_tx[PACKET_LONGITUDE_POS + 1] = 	gps_air[device_number].longitude.as_array[1];
	p_air_packet_tx[PACKET_LONGITUDE_POS + 2] = 	gps_air[device_number].longitude.as_array[2];
	p_air_packet_tx[PACKET_LONGITUDE_POS + 3] = 	gps_air[device_number].longitude.as_array[3];
	p_air_packet_tx[PACKET_ALTITUDE_POS] = 			gps_air[device_number].altitude.as_array[0];
	p_air_packet_tx[PACKET_ALTITUDE_POS + 1] = 		gps_air[device_number].altitude.as_array[1];
	p_air_packet_tx[PACKET_SPEED_POS] = 			gps_air[device_number].speed;
	p_air_packet_tx[PACKET_COURSE_POS] = 			gps_air[device_number].course.as_array[0];
	p_air_packet_tx[PACKET_COURSE_POS + 1] = 		gps_air[device_number].course.as_array[1];
}



//Copy RX air packet data to the gps_air struct
uint8_t fill_struct_with_air_packet_data(uint32_t uptime)
{
	uint8_t rx_device = p_air_packet_rx[PACKET_DEVICE_NUMBER_POS]; //extract device number from received packet

	dev_aux[rx_device].exist_flag = 				1;
	dev_aux[rx_device].timestamp = 					uptime;
    gps_air[rx_device].device_id[0] = 				p_air_packet_rx[PACKET_DEVICE_ID_POS];
    gps_air[rx_device].device_id[1] = 				p_air_packet_rx[PACKET_DEVICE_ID_POS + 1];
    gps_air[rx_device].flags = 						p_air_packet_rx[PACKET_FLAGS_POS];
    gps_air[rx_device].latitude.as_array[0] = 		p_air_packet_rx[PACKET_LATITUDE_POS];
    gps_air[rx_device].latitude.as_array[1] = 		p_air_packet_rx[PACKET_LATITUDE_POS + 1];
    gps_air[rx_device].latitude.as_array[2] = 		p_air_packet_rx[PACKET_LATITUDE_POS + 2];
    gps_air[rx_device].latitude.as_array[3] = 		p_air_packet_rx[PACKET_LATITUDE_POS + 3];
    gps_air[rx_device].longitude.as_array[0] = 		p_air_packet_rx[PACKET_LONGITUDE_POS];
    gps_air[rx_device].longitude.as_array[1] = 		p_air_packet_rx[PACKET_LONGITUDE_POS + 1];
    gps_air[rx_device].longitude.as_array[2] = 		p_air_packet_rx[PACKET_LONGITUDE_POS + 2];
    gps_air[rx_device].longitude.as_array[3] = 		p_air_packet_rx[PACKET_LONGITUDE_POS + 3];
    gps_air[rx_device].altitude.as_array[0] =		p_air_packet_rx[PACKET_ALTITUDE_POS];
    gps_air[rx_device].altitude.as_array[1] = 		p_air_packet_rx[PACKET_ALTITUDE_POS + 1];
    gps_air[rx_device].speed = 						p_air_packet_rx[PACKET_SPEED_POS];
    gps_air[rx_device].course.as_array[0] = 		p_air_packet_rx[PACKET_COURSE_POS];
    gps_air[rx_device].course.as_array[1] = 		p_air_packet_rx[PACKET_COURSE_POS + 1];

    return rx_device;
}



//  Heart of the device <3
//Calculates distance between two GPS points (using haversine formula)
//Calculates heading between two GPS points (loxodrome, or rhumb line)
//Calculates altitude difference
void calc_relative_position(uint8_t another_device)
{
    double lat1, lat2, lon1, lon2;
    double distance, heading;
    
    //my position
    lat1 = p_gps_num->latitude.in_rad;
    lon1 = p_gps_num->longitude.in_rad;
    
    //position of the device to calculate relative position
    lat2 = gps_air[another_device].latitude.as_float * deg_to_rad;
    lon2 = gps_air[another_device].longitude.as_float * deg_to_rad;
    
    if (lat1 == lat2)
    {
        lat2 += 0.00000001;       //slightly shift the position
    }
    
    if (lon1 == lon2)
    {
        lon2 += 0.00000001;       //slightly shift the position
    }
    
    distance = twice_mean_earth_radius *
               asin( sqrt( pow(sin((lat2 - lat1) / 2), 2) +
                           cos(lat2) * cos(lat1) * pow(sin((lon2 - lon1) / 2), 2)));
    
    heading = atan((lon2 - lon1) /
                    log(tan(pi_div_by_4 + lat2 / 2) / tan(pi_div_by_4 + lat1 / 2)));
    
    
    if ((lat2 > lat1) && (lon2 > lon1))
    {
        heading *= rad_to_deg;
    }
    else if ((lat2 > lat1) && (lon2 < lon1))
    {
        heading = 360.0 + heading * rad_to_deg;
    }
    else if (lat2 < lat1)
    {
        heading = 180.0 + heading * rad_to_deg;
    }
    
    gps_rel[another_device].distance = (uint32_t)distance;

    if (gps_rel[another_device].distance == 0)
    {
    	gps_rel[another_device].heading = 0;	//if distance is zero then make heading equals zero too
    }
    else
    {
    	gps_rel[another_device].heading = (uint16_t)heading;
    }

    gps_rel[another_device].altitude_diff = gps_air[another_device].altitude.as_integer - (int16_t)p_gps_num->altitude;
}



struct gps_air_struct **get_gps_air(void)
{
	for (uint8_t i = 0; i <= DEVICES_IN_GROUP; i++)
	{
		p_gps_air[i] = &gps_air[i];
	}

	return &p_gps_air[0];
}



struct gps_rel_struct **get_gps_rel(void)
{
	for (uint8_t i = 0; i <= DEVICES_IN_GROUP; i++)
	{
		p_gps_rel[i] = &gps_rel[i];
	}

	return &p_gps_rel[0];
}



struct dev_aux_struct **get_dev_aux(void)
{
	for (uint8_t i = 0; i <= DEVICES_IN_GROUP; i++)
	{
		p_dev_aux[i] = &dev_aux[i];
	}

	return &p_dev_aux[0];
}
