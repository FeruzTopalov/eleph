/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: main.h
*/



#ifndef MAIN_HEADER
#define MAIN_HEADER



uint32_t get_uptime(void);
struct main_flags_struct *get_main_flags(void);



struct main_flags_struct
{
    uint8_t gps_ready;
    uint8_t gps_sync;
    uint8_t rx_ready;
    uint8_t time_slots_end;
    uint8_t battery_low;
    uint8_t act_status;
    uint8_t begin_scan_buttons;
    uint8_t tick_1s;
    uint8_t do_beep;
    uint8_t do_screen_update;
};



#endif /*MAIN_HEADER*/
