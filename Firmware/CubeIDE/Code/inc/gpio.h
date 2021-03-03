/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: gpio.h
*/

void gpio_init(void);
void ext_int_init(void);

void led_board_on(void);
void led_board_off(void);
void led_red_on(void);
void led_red_off(void);
void led_green_on(void);
void led_green_off(void);

void sdn_si4463_active(void);
void sdn_si4463_inactive(void);

void cs_si4463_active(void);
void cs_si4463_inactive(void);

void res_ssd1306_active(void);
void res_ssd1306_inactive(void);

void ssd1306_data_mode(void);
void ssd1306_command_mode(void);

void cs_ssd1306_active(void);
void cs_ssd1306_inactive(void);

void bat_mon_on(void);
void bat_mon_off(void);
