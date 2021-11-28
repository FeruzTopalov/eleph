/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: points.c
*/
#include <string.h>
#include "stm32f10x.h"
#include "points.h"
#include "m24c64.h"
#include "settings.h"
#include "lrns.h"
#include "gps.h"
#include "service.h"
#include "main.h"



//positions:
#define MEMORY_POINT_EXIST_FLAG_POS         	(0)

#define MEMORY_POINT_NAME_POS          			(1)

#define MEMORY_POINT_DAY_POS          			(6)
#define MEMORY_POINT_MONTH_POS          		(8)
#define MEMORY_POINT_YEAR_POS          			(10)

#define MEMORY_POINT_LATITUDE_POS          		(12)
#define MEMORY_POINT_LONGITUDE_POS          	(16)
#define MEMORY_POINT_ALTITUDE_POS          		(20)







//default values:
#define POINT_EXIST_FLAG_DEFAULT      	(0xAA)



#define MEMORY_SLOT_1_NAME		("P1")		//"P" stands for Point
#define MEMORY_SLOT_2_NAME		("P2")
#define MEMORY_SLOT_3_NAME		("P3")
#define MEMORY_SLOT_4_NAME		("P4")
#define MEMORY_SLOT_5_NAME		("P5")
#define MEMORY_SLOT_6_NAME		("P6")

#define MEMORY_SLOT_NAMES		{ 	"0", 					\
									MEMORY_SLOT_1_NAME, 	\
									MEMORY_SLOT_2_NAME, 	\
									MEMORY_SLOT_3_NAME, 	\
									MEMORY_SLOT_4_NAME, 	\
									MEMORY_SLOT_5_NAME,		\
									MEMORY_SLOT_6_NAME}


void clear_point_buffer(void);



struct memory_slot_struct memory_slot[MEMORY_SLOTS_TOTAL + 1];	//index 0 is invalid and not used; slots start from 1 to MEMORY_SLOTS_TOTAL
struct memory_slot_struct *p_memory_slot[MEMORY_SLOTS_TOTAL + 1];

uint8_t point_buffer[M24C64_PAGE_SIZE];
struct gps_raw_struct *p_gps_raw;
struct gps_air_struct **pp_gps_air;
struct dev_aux_struct **pp_dev_aux;

char *memory_slot_names[MEMORY_SLOTS_TOTAL + 1] = MEMORY_SLOT_NAMES;



struct memory_slot_struct **get_memory_slot(void)
{
	for (uint8_t slot = 1; slot <= MEMORY_SLOTS_TOTAL; slot++)
	{
		p_memory_slot[slot] = &memory_slot[slot];
	}

	return &p_memory_slot[0];
}



void init_points(void)
{
	p_gps_raw = get_gps_raw();
	pp_gps_air = get_gps_air();
	pp_dev_aux = get_dev_aux();

	for (uint8_t slot = 1; slot <= MEMORY_SLOTS_TOTAL; slot++)
	{
		memory_slot[slot].slot_name[0] = memory_slot_names[slot][0];	//init slot names
		memory_slot[slot].slot_name[1] = memory_slot_names[slot][1];
		memory_slot[slot].slot_name[2] = 0;
	}
}



void read_memory_slots(void)
{
	for (uint8_t slot = 1; slot <= MEMORY_SLOTS_TOTAL; slot++)
	{

		clear_point_buffer();
		m24c64_read_page(&point_buffer[0], slot);

		if (point_buffer[MEMORY_POINT_EXIST_FLAG_POS] == POINT_EXIST_FLAG_DEFAULT)
		{
			memory_slot[slot].exist_flag = 1;

			memory_slot[slot].point_name[0] = point_buffer[MEMORY_POINT_NAME_POS];
			memory_slot[slot].point_name[1] = point_buffer[MEMORY_POINT_NAME_POS + 1];
			memory_slot[slot].point_name[2] = point_buffer[MEMORY_POINT_NAME_POS + 2];
			memory_slot[slot].point_name[3] = point_buffer[MEMORY_POINT_NAME_POS + 3];
			memory_slot[slot].point_name[4] = point_buffer[MEMORY_POINT_NAME_POS + 4];
			memory_slot[slot].point_name[5] = 0;

			memory_slot[slot].save_date[0] = point_buffer[MEMORY_POINT_DAY_POS];
			memory_slot[slot].save_date[1] = point_buffer[MEMORY_POINT_DAY_POS + 1];
			memory_slot[slot].save_date[2] = point_buffer[MEMORY_POINT_MONTH_POS];
			memory_slot[slot].save_date[3] = point_buffer[MEMORY_POINT_MONTH_POS + 1];
			memory_slot[slot].save_date[4] = point_buffer[MEMORY_POINT_YEAR_POS];
			memory_slot[slot].save_date[5] = point_buffer[MEMORY_POINT_YEAR_POS + 1];
		}
		else
		{
			memory_slot[slot].exist_flag = 0;
		}
	}
}



//Save selected device in a slot
void save_memory_point(uint8_t dev_num, char *point_name, uint8_t slot_num)
{
	clear_point_buffer();

	point_buffer[MEMORY_POINT_EXIST_FLAG_POS] = 		POINT_EXIST_FLAG_DEFAULT;

	point_buffer[MEMORY_POINT_NAME_POS] = 				point_name[0];
	point_buffer[MEMORY_POINT_NAME_POS + 1] = 			point_name[1];
	point_buffer[MEMORY_POINT_NAME_POS + 2] = 			point_name[2];
	point_buffer[MEMORY_POINT_NAME_POS + 3] = 			point_name[3];
	point_buffer[MEMORY_POINT_NAME_POS + 4] = 			point_name[4];

	point_buffer[MEMORY_POINT_DAY_POS] = 				p_gps_raw->date[0];	//save in char format for easy print in the menu
	point_buffer[MEMORY_POINT_DAY_POS + 1] = 			p_gps_raw->date[1];

	point_buffer[MEMORY_POINT_MONTH_POS] = 			p_gps_raw->date[2];
	point_buffer[MEMORY_POINT_MONTH_POS + 1] = 		p_gps_raw->date[3];

	point_buffer[MEMORY_POINT_YEAR_POS] = 				p_gps_raw->date[4];
	point_buffer[MEMORY_POINT_YEAR_POS + 1] = 			p_gps_raw->date[5];

	point_buffer[MEMORY_POINT_LATITUDE_POS] = 			pp_gps_air[dev_num]->latitude.as_array[0];
	point_buffer[MEMORY_POINT_LATITUDE_POS + 1] = 		pp_gps_air[dev_num]->latitude.as_array[1];
	point_buffer[MEMORY_POINT_LATITUDE_POS + 2] = 		pp_gps_air[dev_num]->latitude.as_array[2];
	point_buffer[MEMORY_POINT_LATITUDE_POS + 3] = 		pp_gps_air[dev_num]->latitude.as_array[3];

	point_buffer[MEMORY_POINT_LONGITUDE_POS] = 		pp_gps_air[dev_num]->longitude.as_array[0];
	point_buffer[MEMORY_POINT_LONGITUDE_POS + 1] = 	pp_gps_air[dev_num]->longitude.as_array[1];
	point_buffer[MEMORY_POINT_LONGITUDE_POS + 2] = 	pp_gps_air[dev_num]->longitude.as_array[2];
	point_buffer[MEMORY_POINT_LONGITUDE_POS + 3] = 	pp_gps_air[dev_num]->longitude.as_array[3];

	point_buffer[MEMORY_POINT_ALTITUDE_POS] = 			pp_gps_air[dev_num]->altitude.as_array[0];
	point_buffer[MEMORY_POINT_ALTITUDE_POS + 1] = 		pp_gps_air[dev_num]->altitude.as_array[1];

	m24c64_write_page(&point_buffer[0], slot_num);
}



void load_memory_point(uint8_t dev_num, uint8_t slot_num)
{
	clear_point_buffer();

	m24c64_read_page(&point_buffer[0], slot_num);

	memset(pp_gps_air[dev_num], 0, sizeof(*(pp_gps_air[dev_num])));
	memset(pp_dev_aux[dev_num], 0, sizeof(*(pp_dev_aux[dev_num])));

	pp_dev_aux[dev_num]->exist_flag = 1;
	pp_dev_aux[dev_num]->memory_point_flag = 1;
	pp_dev_aux[dev_num]->timestamp = get_uptime();

	for (uint8_t c = 0; c < MEMORY_POINT_NAME_LENGTH; c++)
	{
		pp_dev_aux[dev_num]->point_name[c] = memory_slot[slot_num].point_name[c];
	}

	pp_gps_air[dev_num]->device_id[0] = memory_slot[slot_num].slot_name[0];
	pp_gps_air[dev_num]->device_id[1] = memory_slot[slot_num].slot_name[1];

	pp_gps_air[dev_num]->latitude.as_array[0] = 	point_buffer[MEMORY_POINT_LATITUDE_POS];
	pp_gps_air[dev_num]->latitude.as_array[1] =  	point_buffer[MEMORY_POINT_LATITUDE_POS + 1];
	pp_gps_air[dev_num]->latitude.as_array[2] =  	point_buffer[MEMORY_POINT_LATITUDE_POS + 2];
	pp_gps_air[dev_num]->latitude.as_array[3] =  	point_buffer[MEMORY_POINT_LATITUDE_POS + 3];

	pp_gps_air[dev_num]->longitude.as_array[0] = 	point_buffer[MEMORY_POINT_LONGITUDE_POS];
	pp_gps_air[dev_num]->longitude.as_array[1] = 	point_buffer[MEMORY_POINT_LONGITUDE_POS + 1];
	pp_gps_air[dev_num]->longitude.as_array[2] = 	point_buffer[MEMORY_POINT_LONGITUDE_POS + 2];
	pp_gps_air[dev_num]->longitude.as_array[3] = 	point_buffer[MEMORY_POINT_LONGITUDE_POS + 3];

	pp_gps_air[dev_num]->altitude.as_array[0] =  	point_buffer[MEMORY_POINT_ALTITUDE_POS];
	pp_gps_air[dev_num]->altitude.as_array[1] =		point_buffer[MEMORY_POINT_ALTITUDE_POS + 1];
}




void delete_memory_point(uint8_t slot_num)
{
	clear_point_buffer();
	m24c64_write_page(&point_buffer[0], slot_num);
}




void clear_point_buffer(void)
{
	for (uint8_t i = 0; i < M24C64_PAGE_SIZE; i++)
	{
		point_buffer[i] = M24C64_EMPTY_CELL_VALUE;
	}
}
