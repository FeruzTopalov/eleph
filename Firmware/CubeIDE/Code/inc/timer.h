/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: timer.h
*/



#ifndef TIMER_HEADER
#define TIMER_HEADER



void timers_init(void);
void make_a_beep(void);
void make_a_long_beep(void);
void toggle_mute(void);
uint8_t get_mute_flag(void);
void systick_stop(void);
void timer1_start(void);
void timer1_stop_reload(void);
void timer3_stop(void);
void timer3_clock_disable(void);



#endif /*TIMER_HEADER*/
