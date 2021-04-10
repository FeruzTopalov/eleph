/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: lrns.h
*/



#ifndef LRNS_HEADER
#define LRNS_HEADER



extern const double deg_to_rad;       //deg to rad multiplyer



//DEVICES
#define DEVICES_IN_GROUP    (6)         //total number of devices in group



#define FLAGS_BATTERY           (1)
#define FLAGS_GPS_FIX           (2)
#define FLAGS_PDOP              (3)
#define FLAGS_ALARM             (4)

#define FLAG_BATTERY_0_TO_10   	(0)
#define FLAG_BATTERY_10_TO_25  	(1)
#define FLAG_BATTERY_25_TO_50  	(2)
#define FLAG_BATTERY_50_TO_75  	(3)
#define FLAG_BATTERY_75_TO_100 	(4)
#define FLAG_GPS_FIX_2D        	(0)
#define FLAG_GPS_FIX_3D        	(1)
#define FLAG_PDOP_BAD          	(0)
#define FLAG_PDOP_GOOD         	(1)
#define FLAG_ALARM_OFF         	(0)
#define FLAG_ALARM_ON          	(1)



void init_lrns(void);
void set_device_flags(uint8_t parameter_to_set, uint8_t parameter_value);
uint8_t get_device_flags(uint8_t device_number, uint8_t parameter_to_get);

uint8_t check_alarms(void);
void calc_timeout(uint32_t current_uptime);
uint8_t check_timeout(void);
uint8_t check_fence(void);

void process_all_devices(void);

void gps_air_update_my_data(uint32_t uptime);
void fill_air_packet_with_struct_data(void);
uint8_t fill_struct_with_air_packet_data(uint32_t uptime);
void calc_relative_position(uint8_t another_device);

struct gps_air_struct **get_gps_air(void);
struct gps_rel_struct **get_gps_rel(void);
struct dev_aux_struct **get_dev_aux(void);



//Data to be transferred over-the-air
struct gps_air_struct
{
    char device_id[DEVICE_ID_LEN];  //user id, ASCII symbols
    uint8_t flags;                  //device flags

    union
    {
        float as_float;             //latitude in decimal degrees (-90...+90)
        uint8_t as_array[4];
    } latitude;

    union
    {
        float as_float;             //longitude in decimal degrees (-180...+180)
        uint8_t as_array[4];
    } longitude;

    union
    {
        int16_t as_integer;         //altitude in meters, signed
        uint8_t as_array[2];
    } altitude;

    uint8_t speed;                  //speed in km/h, unsigned

    union
    {
        uint16_t as_integer;        //course in degrees, unsigned
        uint8_t as_array[2];
    } course;
};



//Data with relative position info
struct gps_rel_struct
{
    uint32_t distance;          //distance in meters between this device and another one
    uint16_t heading;           //heading to another device, degrees
    int16_t altitude_diff;      //difference in altitudes, in meters, signed
};



//Auxiliary information about devices
struct dev_aux_struct
{
    uint8_t exist_flag;             		//is this device exist? (i.e. at least one successful receprion is exist)
    uint32_t timestamp;             		//"uptime" value at the moment of packet reception
    uint32_t timeout;               		//timeout value at the moment of calculation via check_timeout(); equals uptime-timestamp
    uint8_t timeout_flag;					//flag, indicates that timeout occured
    uint8_t fence_flag;						//indicates if distance to a device is bigger than predefined fence radius
    uint8_t memory_point_flag;				//is this device was loaded as a point from memory?
    char point_name[MEMORY_POINT_NAME_LENGTH + 1];	//user-defined point name in the slot
};



#endif /*LRNS_HEADER*/
