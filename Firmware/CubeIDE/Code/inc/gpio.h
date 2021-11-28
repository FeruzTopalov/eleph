/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: gpio.h
*/



#ifndef GPIO_HEADER
#define GPIO_HEADER



void gpio_init(void);
void ext_int_init(void);

void led_red_on(void);
void led_red_off(void);
void led_green_on(void);
void led_green_off(void);

void cs_rfm98_active(void);
void cs_rfm98_inactive(void);

void res_rfm98_active(void);
void res_rfm98_inactive(void);

void res_ssd1306_active(void);
void res_ssd1306_inactive(void);

void ssd1306_data_mode(void);
void ssd1306_command_mode(void);

void cs_ssd1306_active(void);
void cs_ssd1306_inactive(void);

void hold_power(void);
void release_power(void);



#endif /*GPIO_HEADER*/
