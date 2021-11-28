/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>

    file: points.h
*/



#ifndef POINTS_HEADER
#define POINTS_HEADER



#define MEMORY_POINT_NAME_LENGTH   (5) //excluding string-end symbol \0
#define MEMORY_SLOTS_TOTAL	(6)



void init_points(void);
struct memory_slot_struct **get_memory_slot(void);
void read_memory_slots(void);
void save_memory_point(uint8_t dev_num, char *point_name, uint8_t slot_num);
void load_memory_point(uint8_t dev_num, uint8_t slot_num);
void delete_memory_point(uint8_t slot_num);



struct memory_slot_struct
{
    uint8_t exist_flag;             				//is this point exist (was previously saved) in memory?
    char slot_name[sizeof("SX")];					//slot name S1...S5, not user-defined
    char point_name[MEMORY_POINT_NAME_LENGTH + 1];	//user-defined point name in the slot
    char save_date[sizeof("ddmmyy")];				//point save date
};



#endif /*POINTS_HEADER*/
