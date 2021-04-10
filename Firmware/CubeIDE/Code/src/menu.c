/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: menu.c
*/



#include <math.h>
#include <string.h>
#include "stm32f10x.h"
#include "menu.h"
#include "buttons.h"
#include "ssd1306.h"
#include "main.h"
#include "service.h"
#include "settings.h"
#include "m24c64.h"
#include "ssd1306_bitmaps.h"
#include "gpio.h"
#include "adc.h"
#include "gps.h"
#include "points.h"
#include "lrns.h"
#include "si4463.h"
#include "timer.h"



char *FW_VERSION = "2.0";	//firmware
char *HW_VERSION = "1";		//hardware



#define ANOTHER_DEVICE_START_ROW        (3)
#define ANOTHER_RADAR_DEVICE_START_ROW	(2)

#define FREQ_CHANNEL_FIRST  (1)
#define FREQ_CHANNEL_LAST   (69)

#define DEVICE_ID_FIRST_SYMBOL      ('A')
#define DEVICE_ID_LAST_SYMBOL       ('Z')

#define POINT_NAME_FIRST_SYMBOL      (' ')
#define POINT_NAME_LAST_SYMBOL       ('~')

#define TX_POWER_FIRST_OPTION       (TX_POWER_10MILLIW_SETTING)
#define TX_POWER_LAST_OPTION        (TX_POWER_100MILLIW_SETTING)

#define SEND_INTERVAL_FIRST_OPTION       (SEND_INTERVAL_1S_SETTING)
#define SEND_INTERVAL_LAST_OPTION        (SEND_INTERVAL_60S_SETTING)

#define MEMORY_SLOT_FIRST		(1)
#define MEMORY_SLOT_LAST		(MEMORY_SLOTS_TOTAL)

#define DEVICE_NUMBER_FIRST (1)
#define DEVICE_NUMBER_LAST  (DEVICES_IN_GROUP)

#define MEMORY_POINT_DEFAULT_NAME 	("ALPHA")



void toggle_alarm(void);



uint8_t get_current_item(void);
uint8_t get_last_item(void);
void set_current_item(uint8_t new_value);
void reset_current_item_in_menu(uint8_t menu);



void scroll_up(void);
void scroll_down(void);
void switch_forward(void);
void switch_backward(void);



void draw_main(void);
void draw_devices(void);
void draw_settings(void);
void draw_info(void);
void draw_edit_settings(void);
void draw_set_dev_num(void);
void draw_set_dev_id(void);
void draw_set_freq_ch(void);
void draw_set_tx_pow(void);
void draw_set_send_intvl(void);
void draw_set_to_thr(void);
void draw_set_fnc_thr(void);
void draw_confirm_settings_save(void);
void draw_restore_defaults(void);
void draw_erase_all(void);
void draw_each_device(void);
void draw_each_device_submenu(void);
void draw_radar(void);
void draw_points(void);
void draw_each_point(void);
void draw_load_point(void);
void draw_delete_point(void);
void draw_delete_device(void);
void draw_save_device(void);
void draw_save_device_as(void);
void draw_saved_popup(void);



void set_dev_num_up(void);
void set_dev_num_down(void);
void set_dev_num_ok(void);
void set_dev_num_esc(void);
void set_dev_id_up(void);
void set_dev_id_down(void);
void set_dev_id_ok(void);
void set_dev_id_ok_long(void);
void set_dev_id_esc(void);
void set_freq_ch_up(void);
void set_freq_ch_down(void);
void set_freq_ch_ok(void);
void set_freq_ch_esc(void);
void set_tx_pow_up(void);
void set_tx_pow_down(void);
void set_tx_pow_ok(void);
void set_tx_pow_esc(void);
void set_send_intvl_up(void);
void set_send_intvl_down(void);
void set_send_intvl_ok(void);
void set_send_intvl_esc(void);
void set_to_thr_up(void);
void set_to_thr_down(void);
void set_to_thr_ok(void);
void set_to_thr_esc(void);
void set_fnc_thr_up(void);
void set_fnc_thr_down(void);
void set_fnc_thr_ok(void);
void set_fnc_thr_esc(void);
void confirm_settings_save_ok(void);
void confirm_settings_save_esc(void);
void restore_defaults_ok(void);
void erase_all_ok(void);
void devices_ok(void);
void each_device_up(void);
void each_device_down(void);
void each_device_ok(void);
void radar_up(void);
void radar_down(void);
void radar_ok(void);
void points_up(void);
void points_down(void);
void points_ok(void);
void points_esc(void);
void load_point_up(void);
void load_point_down(void);
void load_point_ok(void);
void load_point_esc(void);
void delete_point_ok(void);
void delete_device_ok(void);
void save_device_up(void);
void save_device_down(void);
void save_device_ok(void);
void save_device_esc(void);
void save_device_as_up(void);
void save_device_as_down(void);
void save_device_as_ok(void);
void save_device_as_ok_long(void);
void save_device_as_esc(void);
void saved_popup_esc(void);



//ALL MENUS HERE
enum
{
    //menu numbers start from 1, because 0 is used as "end marker" in menu structs
    M_MAIN = 1,
    M_DEVICES,
    M_EACH_DEVICE,
	M_EACH_DEVICE_SUBMENU,
	M_DELETE_DEVICE,
	M_SAVE_DEVICE,
	M_SAVE_DEVICE_AS,
	M_SAVED_POPUP,
	M_RADAR,
	M_POINTS,
	M_EACH_POINT,
	M_LOAD_POINT,
	M_DELETE_POINT,
    M_SETTINGS,
    M_INFO,
    M_EDIT_SETTINGS,
    M_SET_DEV_NUM,
    M_SET_DEV_ID,
    M_SET_FREQ_CH,
    M_SET_TX_POW,
	M_SET_SEND_INTVL,
	M_SET_TO_THR,
	M_SET_FNC_THR,
    M_CONFIRM_SETTINGS_SAVE,
    M_RESTORE_DEFAULTS,
    M_ERASE_ALL
};



//ALL MENU ITEMS HERE (for each menu separately)
//note: for all menus first item always has index of 0
#define M_ALL_I_FIRST (0)



//MAIN
enum
{
    M_MAIN_I_DEVICES = 0,
	M_MAIN_I_RADAR,
	M_MAIN_I_POINTS,
    M_MAIN_I_SETTINGS,
    M_MAIN_I_INFO,                  //last item
    M_MAIN_I_LAST = M_MAIN_I_INFO   //copy last item here
};



//SETTINGS
enum
{
    M_SETTINGS_I_EDIT = 0,
    M_SETTINGS_I_RESTORE,
    M_SETTINGS_I_ERASE,
    M_SETTINGS_I_LAST = M_SETTINGS_I_ERASE
};



//EDIT SETTINGS
enum
{
    M_EDIT_SETTINGS_I_DEV_NUM = 0,
    M_EDIT_SETTINGS_I_DEV_ID,
    M_EDIT_SETTINGS_I_FREQ_CH,
    M_EDIT_SETTINGS_I_TX_POW,
	M_EDIT_SETTINGS_I_SEND_INTVL,
	M_EDIT_SETTINGS_I_TO_THR,
	M_EDIT_SETTINGS_I_FNC_THR,
    M_EDIT_SETTINGS_I_LAST = M_EDIT_SETTINGS_I_FNC_THR
};



//EACH POINT
enum
{
	M_EACH_POINT_I_LOAD = 0,
	M_EACH_POINT_I_DELETE,
	M_EACH_POINT_I_LAST = M_EACH_POINT_I_DELETE
};



//EACH DEVICE SUBMENU
enum
{
	M_EACH_DEVICE_SUBMENU_I_SAVE = 0,
	M_EACH_DEVICE_SUBMENU_I_DELETE,
	M_EACH_DEVICE_SUBMENU_I_LAST = M_EACH_DEVICE_SUBMENU_I_DELETE
};



//Only exclusive (non default) actions here, for example edit a variable in settings
const struct
{
    uint8_t current_menu;
    uint8_t button_pressed;
    void (*execute_function)(void);
} menu_exclusive_table[] = 
{
//  Current Menu                Button pressed          Action (function name)
    {M_DEVICES,                 BTN_OK,                 devices_ok},
    {M_EACH_DEVICE,             BTN_UP,                 each_device_up},
    {M_EACH_DEVICE,             BTN_DOWN,               each_device_down},
	{M_EACH_DEVICE,             BTN_OK,               	each_device_ok},
    {M_RADAR,             		BTN_UP,                 radar_up},
    {M_RADAR,             		BTN_DOWN,               radar_down},
	{M_RADAR,             		BTN_OK,               	radar_ok},
	{M_SAVE_DEVICE,				BTN_UP,					save_device_up},
	{M_SAVE_DEVICE,				BTN_DOWN,				save_device_down},
	{M_SAVE_DEVICE,				BTN_OK,					save_device_ok},
	{M_SAVE_DEVICE,				BTN_ESC,				save_device_esc},
    {M_SAVE_DEVICE_AS,          BTN_UP,                 save_device_as_up},
    {M_SAVE_DEVICE_AS,          BTN_DOWN,               save_device_as_down},
    {M_SAVE_DEVICE_AS,          BTN_OK,                 save_device_as_ok},
    {M_SAVE_DEVICE_AS,          BTN_OK_LONG,            save_device_as_ok_long},
    {M_SAVE_DEVICE_AS,          BTN_ESC,                save_device_as_esc},
	{M_SAVED_POPUP,				BTN_ESC,				saved_popup_esc},
	{M_DELETE_DEVICE,			BTN_OK,					delete_device_ok},
	{M_POINTS,					BTN_UP,					points_up},
	{M_POINTS,					BTN_DOWN,				points_down},
	{M_POINTS,					BTN_OK,					points_ok},
	{M_POINTS,					BTN_ESC,				points_esc},
	{M_LOAD_POINT,				BTN_UP,					load_point_up},
	{M_LOAD_POINT,				BTN_DOWN,				load_point_down},
	{M_LOAD_POINT,             	BTN_OK,               	load_point_ok},
	{M_LOAD_POINT,             	BTN_ESC,               	load_point_esc},
	{M_DELETE_POINT,			BTN_OK,					delete_point_ok},
    {M_SET_DEV_NUM,             BTN_UP,                 set_dev_num_up},
    {M_SET_DEV_NUM,             BTN_DOWN,               set_dev_num_down},
    {M_SET_DEV_NUM,             BTN_OK,                 set_dev_num_ok},
    {M_SET_DEV_NUM,             BTN_ESC,                set_dev_num_esc},
    {M_SET_DEV_ID,              BTN_UP,                 set_dev_id_up},
    {M_SET_DEV_ID,              BTN_DOWN,               set_dev_id_down},
    {M_SET_DEV_ID,              BTN_OK,                 set_dev_id_ok},
    {M_SET_DEV_ID,              BTN_OK_LONG,            set_dev_id_ok_long},
    {M_SET_DEV_ID,              BTN_ESC,                set_dev_id_esc},
    {M_SET_FREQ_CH,             BTN_UP,                 set_freq_ch_up},
    {M_SET_FREQ_CH,             BTN_DOWN,               set_freq_ch_down},
    {M_SET_FREQ_CH,             BTN_OK,                 set_freq_ch_ok},
    {M_SET_FREQ_CH,             BTN_ESC,                set_freq_ch_esc},
    {M_SET_TX_POW,              BTN_UP,                 set_tx_pow_up},
    {M_SET_TX_POW,              BTN_DOWN,               set_tx_pow_down},
    {M_SET_TX_POW,              BTN_OK,                 set_tx_pow_ok},
    {M_SET_TX_POW,              BTN_ESC,                set_tx_pow_esc},
	{M_SET_SEND_INTVL,          BTN_UP,                 set_send_intvl_up},
    {M_SET_SEND_INTVL,          BTN_DOWN,               set_send_intvl_down},
    {M_SET_SEND_INTVL,          BTN_OK,                 set_send_intvl_ok},
    {M_SET_SEND_INTVL,          BTN_ESC,                set_send_intvl_esc},
    {M_SET_TO_THR,              BTN_UP,                 set_to_thr_up},
    {M_SET_TO_THR,              BTN_DOWN,               set_to_thr_down},
    {M_SET_TO_THR,              BTN_OK,                 set_to_thr_ok},
    {M_SET_TO_THR,              BTN_ESC,                set_to_thr_esc},
    {M_SET_FNC_THR,             BTN_UP,                 set_fnc_thr_up},
    {M_SET_FNC_THR,             BTN_DOWN,               set_fnc_thr_down},
    {M_SET_FNC_THR,             BTN_OK,                 set_fnc_thr_ok},
    {M_SET_FNC_THR,             BTN_ESC,                set_fnc_thr_esc},
    {M_CONFIRM_SETTINGS_SAVE,   BTN_OK,                 confirm_settings_save_ok},
    {M_CONFIRM_SETTINGS_SAVE,   BTN_ESC,                confirm_settings_save_esc},
    {M_RESTORE_DEFAULTS,        BTN_OK,                 restore_defaults_ok},
    {M_ERASE_ALL,               BTN_OK,                 erase_all_ok},
    {0, 0, 0}   //end marker
};



//Defaul behaviour (non exclusive) when OK button has been pressed (move forward)
const struct
{
    uint8_t current_menu;
    uint8_t current_item;
    uint8_t next_menu;
} menu_forward_table[] = 
{
//  Current Menu                Current Item                	Next Menu
    {M_MAIN,                    M_MAIN_I_DEVICES,           	M_DEVICES},
	{M_MAIN,                    M_MAIN_I_RADAR,           		M_RADAR},
	{M_MAIN,                    M_MAIN_I_POINTS,           		M_POINTS},
    {M_MAIN,                    M_MAIN_I_SETTINGS,          	M_SETTINGS},
    {M_MAIN,                    M_MAIN_I_INFO,              	M_INFO},
	{M_EACH_DEVICE_SUBMENU,		M_EACH_DEVICE_SUBMENU_I_SAVE,	M_SAVE_DEVICE},
	{M_EACH_DEVICE_SUBMENU,		M_EACH_DEVICE_SUBMENU_I_DELETE,	M_DELETE_DEVICE},
	{M_EACH_POINT,				M_EACH_POINT_I_LOAD,			M_LOAD_POINT},
	{M_EACH_POINT,				M_EACH_POINT_I_DELETE,			M_DELETE_POINT},
    {M_SETTINGS,                M_SETTINGS_I_EDIT,          	M_EDIT_SETTINGS},
    {M_SETTINGS,                M_SETTINGS_I_RESTORE,       	M_RESTORE_DEFAULTS},
    {M_SETTINGS,                M_SETTINGS_I_ERASE,         	M_ERASE_ALL},
    {M_EDIT_SETTINGS,           M_EDIT_SETTINGS_I_DEV_NUM,  	M_SET_DEV_NUM},
    {M_EDIT_SETTINGS,           M_EDIT_SETTINGS_I_DEV_ID,   	M_SET_DEV_ID},
    {M_EDIT_SETTINGS,           M_EDIT_SETTINGS_I_FREQ_CH,  	M_SET_FREQ_CH},
    {M_EDIT_SETTINGS,           M_EDIT_SETTINGS_I_TX_POW,   	M_SET_TX_POW},
	{M_EDIT_SETTINGS,           M_EDIT_SETTINGS_I_SEND_INTVL,   M_SET_SEND_INTVL},
	{M_EDIT_SETTINGS,           M_EDIT_SETTINGS_I_TO_THR,   	M_SET_TO_THR},
	{M_EDIT_SETTINGS,           M_EDIT_SETTINGS_I_FNC_THR,  	M_SET_FNC_THR},
    {0, 0, 0}   //end marker
};



//Defaul behaviour (non exclusive) when ESC button has been pressed (move backward)
const struct
{
    uint8_t current_menu;
    uint8_t next_menu;
} menu_backward_table[] = 
{
//  Current Menu                Next Menu
    {M_DEVICES,                 M_MAIN},
    {M_EACH_DEVICE,             M_DEVICES},
	{M_EACH_DEVICE_SUBMENU,     M_EACH_DEVICE},
	{M_SAVE_DEVICE,				M_EACH_DEVICE_SUBMENU},
	{M_DELETE_DEVICE,			M_EACH_DEVICE_SUBMENU},
	{M_RADAR,					M_MAIN},
	{M_EACH_POINT,				M_POINTS},
	{M_DELETE_POINT,			M_EACH_POINT},
    {M_SETTINGS,                M_MAIN},
    {M_INFO,                    M_MAIN},
    {M_EDIT_SETTINGS,           M_CONFIRM_SETTINGS_SAVE},
    {M_RESTORE_DEFAULTS,        M_SETTINGS},
    {M_ERASE_ALL,               M_SETTINGS},
    {0, 0}      //end marker
};



//Struct with list of menus and real-time values of current item in current menu. Last Item is needed for scroll function
//note: if current menu has no items (like INFO menu) no need to put it in structure below, because item functions (get, get last, set) automatically return 0 (which is zero item)
struct
{
    const uint8_t curent_menu;
    uint8_t cur_item;
    const uint8_t last_item;
} item_table[] = 
{
//  Current Menu                Current Item                Last Item in Current Menu
    {M_MAIN,                    M_ALL_I_FIRST,              M_MAIN_I_LAST},
	{M_EACH_DEVICE_SUBMENU,		M_ALL_I_FIRST,				M_EACH_DEVICE_SUBMENU_I_LAST},
	{M_EACH_POINT,				M_ALL_I_FIRST,				M_EACH_POINT_I_LAST},
    {M_SETTINGS,                M_ALL_I_FIRST,              M_SETTINGS_I_LAST},
    {M_EDIT_SETTINGS,           M_ALL_I_FIRST,              M_EDIT_SETTINGS_I_LAST},
    {0, 0, 0}   //end marker
};



//List of menus with appropriate functions to draw it (show on screen)
const struct
{
    uint8_t current;
    void (*action)(void);
} menu_draw_table[] = 
{
//  Current Menu                Draw Function
    {M_MAIN,                    draw_main},
    {M_DEVICES,                 draw_devices},
    {M_EACH_DEVICE,             draw_each_device},
	{M_EACH_DEVICE_SUBMENU,     draw_each_device_submenu},
	{M_SAVE_DEVICE,				draw_save_device},
	{M_SAVE_DEVICE_AS,			draw_save_device_as},
	{M_SAVED_POPUP,				draw_saved_popup},
	{M_DELETE_DEVICE,			draw_delete_device},
	{M_RADAR,					draw_radar},
	{M_POINTS,					draw_points},
	{M_EACH_POINT,				draw_each_point},
	{M_LOAD_POINT,				draw_load_point},
	{M_DELETE_POINT,			draw_delete_point},
    {M_SETTINGS,                draw_settings},
    {M_INFO,                    draw_info},
    {M_EDIT_SETTINGS,           draw_edit_settings},
    {M_SET_DEV_NUM,             draw_set_dev_num},
    {M_SET_DEV_ID,              draw_set_dev_id},
    {M_SET_FREQ_CH,             draw_set_freq_ch},
    {M_SET_TX_POW,              draw_set_tx_pow},
	{M_SET_SEND_INTVL,          draw_set_send_intvl},
	{M_SET_TO_THR,				draw_set_to_thr},
	{M_SET_FNC_THR,				draw_set_fnc_thr},
    {M_CONFIRM_SETTINGS_SAVE,   draw_confirm_settings_save},
    {M_RESTORE_DEFAULTS,        draw_restore_defaults},
    {M_ERASE_ALL,               draw_erase_all},
    {0, 0}      //end marker
};



struct settings_struct *p_settings;
struct settings_struct settings_copy;

struct gps_raw_struct *p_gps_raw;
struct gps_num_struct *p_gps_num;
struct gps_air_struct **pp_gps_air;
struct gps_rel_struct **pp_gps_rel;
struct dev_aux_struct **pp_dev_aux;
struct memory_slot_struct **pp_memory_slot;



uint8_t *p_send_interval_values;
uint8_t *p_get_tx_power_values;



uint8_t current_menu;                               //Actually Current Menu value (real-time)
char buf[21];                                       //temporary char buffer for screen text fragments
float tmpf;                                         //temporary float variable
uint32_t tmpui32;                                   //temporary uint32
int16_t tmpi16;                                     //temporary int16
uint8_t flag_settings_changed = 0;                  //is settings changed?
uint8_t device_id_current_symbol = 0;               //current editing symbol in device_id[]



const char point_to_save_default_name[MEMORY_POINT_NAME_LENGTH + 1] = MEMORY_POINT_DEFAULT_NAME;
char point_to_save_name[MEMORY_POINT_NAME_LENGTH + 1];
uint8_t point_name_current_symbol = 0;



uint8_t current_each_device = DEVICE_NUMBER_FIRST;  //current device number in EACH DEVICE menu
uint8_t current_radar_device = 0;  					//current device number in RADAR menu, set to 0, see draw_radar()
uint8_t current_device_to_load = 0;
uint8_t current_slot_to_load = MEMORY_SLOT_FIRST;	//currently selected point slot in points menu
uint8_t current_slot_to_save = 0;
uint8_t point_to_save_list[MEMORY_SLOTS_TOTAL + 1];
uint8_t device_to_load_list[DEVICES_IN_GROUP + 1]; //todo: why DEVICES_IN_GROUP + 1 ??? should be DEVICES_IN_GROUP
uint8_t radar_list[DEVICES_IN_GROUP + 1]; 			//list of devices in radar menu, 5 devices total (because of except me); radar_list[device_number] = item; items start from 0
uint8_t radar_list_hide[DEVICES_IN_GROUP + 1];		//if == 1 then hide device cross on the radar screen
uint8_t device_number;								//this device number



const uint8_t sx0 = 24;	//radar center pixel on screen (i.e. my position)
const uint8_t sy0 = 31;
const uint8_t r_circ_dots = 20;		//screen circle radius in dots
uint8_t custom_exclam_mark[2] = {0, 0x5F};



//Init and show MAIN menu
void init_menu(void)
{
	p_gps_raw = get_gps_raw();
	p_gps_num = get_gps_num();
	pp_gps_air = get_gps_air();
	pp_gps_rel = get_gps_rel();
	pp_dev_aux = get_dev_aux();

	pp_memory_slot = get_memory_slot();

	p_settings = get_settings();
	settings_copy = *p_settings;

	device_number = p_settings->device_number;

	p_send_interval_values = get_send_interval_values();
	p_get_tx_power_values = get_tx_power_values();

    //init variables
    current_each_device = device_number;   //set me as current

    current_menu = M_MAIN;
    set_current_item(M_MAIN_I_DEVICES);
}



//Check for buttons and change menu if needed
void change_menu(uint8_t button_code)
{
    if (button_code) //if any button was pressed
    {
    	if (ssd1306_get_display_status() == SSD1306_DISPLAY_ON) //if lcd is on
    	{
			//search for exclusive operation for this case
			for (uint8_t i = 0; menu_exclusive_table[i].current_menu; i++)     //until end marker
			{
				if (current_menu == menu_exclusive_table[i].current_menu &&
					button_code == menu_exclusive_table[i].button_pressed)
				{
					menu_exclusive_table[i].execute_function();
					return;         //exit
				}
			}

			//well, there is no exclusive operations for that case, perform default action
			switch (button_code)
			{
				case BTN_UP:
					scroll_up();
					break;

				case BTN_DOWN:
					scroll_down();
					break;

				case BTN_OK:
					switch_forward();
					break;

				case BTN_ESC:
					switch_backward();
					break;

				case BTN_PWR:
					ssd1306_toggle_display();
					break;

				case BTN_PWR_LONG:
					toggle_alarm();
					break;

				case BTN_ESC_LONG:
					toggle_mute();
					draw_current_menu();
					break;
			}
    	}
    	else if (button_code == BTN_PWR)	//if lcd is off then check for PRW button was pressed. If so - toggle the lcd
    	{
    		ssd1306_toggle_display();
    	}
    }
}



//Switch alarm status
void toggle_alarm(void)
{
    if (get_device_flags(device_number, FLAGS_ALARM) == FLAG_ALARM_OFF)
    {
        set_device_flags(FLAGS_ALARM, FLAG_ALARM_ON);
    }
    else
    {
        set_device_flags(FLAGS_ALARM, FLAG_ALARM_OFF);
    }
    
    check_alarms();	//immediately update alarm indication
    draw_current_menu();
}



//Scroll current menu Up
void scroll_up(void)
{
    uint8_t current = get_current_item();
    uint8_t last = get_last_item();
    
    if (current == M_ALL_I_FIRST)
    {
        set_current_item(last);
    }
    else
    {
        set_current_item(current - 1);
    }
    
    draw_current_menu();
}



//Scroll current menu Down
void scroll_down(void)
{
    uint8_t current = get_current_item();
    uint8_t last = get_last_item();
    
    if (current == last)
    {
        set_current_item(M_ALL_I_FIRST);
    }
    else
    {
        set_current_item(current + 1);
    }
    
    draw_current_menu();
}



//Switch menu forward by default
void switch_forward(void)
{
    for (uint8_t i = 0; menu_forward_table[i].current_menu; i++)
    {
        if (current_menu == menu_forward_table[i].current_menu &&
            get_current_item() == menu_forward_table[i].current_item)
        {
            current_menu = menu_forward_table[i].next_menu;
            break;
        }
    }
    draw_current_menu();
}



//Switch menu backward by default
void switch_backward(void)
{
    for (uint8_t i = 0; menu_backward_table[i].current_menu; i++)
    {
        if (current_menu == menu_backward_table[i].current_menu)
        {
            set_current_item(M_ALL_I_FIRST);        //reset current item before exit
            current_menu = menu_backward_table[i].next_menu;
            break;
        }
    }
    draw_current_menu();
}



//Get currently selected item in current menu
uint8_t get_current_item(void)
{
    for (uint8_t i = 0; item_table[i].curent_menu; i++)
    {
        if (current_menu == item_table[i].curent_menu)
        {
            return item_table[i].cur_item;
        }
    }
    return 0;       //automatically return 0 if item not found in item_table[]
}



//Get last item in current menu
uint8_t get_last_item(void)
{
    for (uint8_t i = 0; item_table[i].curent_menu; i++)
    {
        if (current_menu == item_table[i].curent_menu)
        {
            return item_table[i].last_item;
        }
    }
    return 0;       //automatically return 0 if item not found in item_table[]
}



//Set item to be current in current menu
void set_current_item(uint8_t new_value)
{
    for (uint8_t i = 0; item_table[i].curent_menu; i++)
    {
        if (current_menu == item_table[i].curent_menu)
        {
            item_table[i].cur_item = new_value;
            break;
        }
    }
}



//Reset item in any menu
void reset_current_item_in_menu(uint8_t menu)
{
    for (uint8_t i = 0; item_table[i].curent_menu; i++)
    {
        if (menu == item_table[i].curent_menu)
        {
            item_table[i].cur_item = M_ALL_I_FIRST;
            break;
        }
    }
}



//Draw current menu
void draw_current_menu(void)
{
    for (uint8_t i = 0; menu_draw_table[i].current; i++)
    {
        if (current_menu == menu_draw_table[i].current)
        {
            menu_draw_table[i].action();
            break;
        }
    }
}



//MAIN
void draw_main(void)
{
    #define MAIN_ROW               (2)
    #define MAIN_COL               (1)
    
    ssd1306_clear();
    ssd1306_print(0, MAIN_COL, "MENU", 0);
    ssd1306_print(MAIN_ROW, MAIN_COL, "Devices", 0);
    ssd1306_print(MAIN_ROW + 1, MAIN_COL, "Radar", 0);
    ssd1306_print(MAIN_ROW + 2, MAIN_COL, "Points", 0);
    ssd1306_print(MAIN_ROW + 3, MAIN_COL, "Settings", 0);
    ssd1306_print(MAIN_ROW + 4, MAIN_COL, "Info", 0);
    ssd1306_print(MAIN_ROW + get_current_item(), MAIN_COL - 1, ">", 0);

    ssd1306_char_pos(0, 20, SYMB_NOTE, 0);
    if (get_mute_flag() == 1)
    {
    	ssd1306_char_pos(0, 19, '!', 0);
    }

    ssd1306_update();
}



//DEVICES
void draw_devices(void)
{
    ssd1306_clear();
    ssd1306_bitmap(&devices_blank[0]);
    
    //TRX
    if (get_main_flags()->gps_sync && p_gps_num->status == GPS_DATA_VALID)
    {
        ssd1306_char_pos(0, 18, SYMB_ARROW_UP, 0);
        ssd1306_char_pos(0, 19, SYMB_ARROW_DOWN, 0);
    }
    
    //Sattelites
    if (p_gps_raw->time[0] == 0)                           //if no time received then no sattelites at all
    {
        ssd1306_char_pos(0, 20, SYMB_NO_SATT, 0);
    }
    else
    {
        if (p_gps_num->mode == GPS_POSITION_3DFIX && p_gps_num->status == GPS_DATA_VALID)
        {
            ssd1306_char_pos(0, 20, SYMB_SATT_3D, 0);   //3D when data is valid
        }
        else if (p_gps_num->mode == GPS_POSITION_2DFIX && p_gps_num->status == GPS_DATA_VALID)
        {
            ssd1306_char_pos(0, 20, SYMB_SATT_2D, 0);   //2D when data is valid
        }
        else
        {
            ssd1306_char_pos(0, 20, SYMB_SATT_1D, 0);   //if not 2D or 3D then only time is avaliable
        }
    }
    
    uint8_t icon_col = LCD_LAST_COL;
    
    //Battery
    switch (get_device_flags(device_number, FLAGS_BATTERY))
    {
        case FLAG_BATTERY_75_TO_100:
            ssd1306_char_pos(1, icon_col--, SYMB_BAT_4OF4, 0);
            break;
        
        case FLAG_BATTERY_50_TO_75:
            ssd1306_char_pos(1, icon_col--, SYMB_BAT_3OF4, 0);
            break;
        
        case FLAG_BATTERY_25_TO_50:
            ssd1306_char_pos(1, icon_col--, SYMB_BAT_2OF4, 0);
            break;
        
        case FLAG_BATTERY_10_TO_25:
            ssd1306_char_pos(1, icon_col--, SYMB_BAT_1OF4, 0);
            break;
        
        case FLAG_BATTERY_0_TO_10:
            ssd1306_char_pos(1, icon_col--, SYMB_BAT_0OF4, 0);
            break;
    }
    
    

    //Timeout
    if (pp_dev_aux[device_number]->timeout_flag)
    {
        ssd1306_char_pos(1, icon_col--, SYMB_TIMEOUT, 0);
    }



    //Alarm
    if (get_device_flags(device_number, FLAGS_ALARM) == FLAG_ALARM_ON)
    {
        ssd1306_char_pos(1, icon_col--, SYMB_ALARM, 0);
    }



    //Devices
    uint8_t another_dev_row = ANOTHER_DEVICE_START_ROW;    //start to print another devices from row 3
    
    for (uint8_t dev = 1; dev <= DEVICES_IN_GROUP; dev++)
    {
        if (dev == device_number)      //if me
        {
            //Number
            itoa32(dev, &buf[0]);
            ssd1306_print(1, 0, &buf[0], 0);
            
            //ID
            ssd1306_char_pos(1, 2, p_settings->device_id[0], 0);
            ssd1306_char_pos(1, 3, p_settings->device_id[1], 0);
            
            //Speed
            if (p_gps_num->speed < 10.0)   //if speed is small show x.x format
            {
                ftoa32(p_gps_num->speed, 1, &buf[0]);
                ssd1306_print_viceversa(1, 7, &buf[0], 0);
            }
            else    //else show int format
            {
                itoa32(pp_gps_air[dev]->speed, &buf[0]);
                ssd1306_print_viceversa(1, 7, &buf[0], 0);
            }
            
            //Course
            itoa32(pp_gps_air[dev]->course.as_integer, &buf[0]);
            ssd1306_print_viceversa(1, 11, &buf[0], 0);
            
            //Altitude
            itoa32(pp_gps_air[dev]->altitude.as_integer, &buf[0]);
            ssd1306_print_viceversa(1, 16, &buf[0], 0);
        }
        else
        {
            if (pp_dev_aux[dev]->exist_flag == 1)        //if not me & exist
            {
                //Number
                itoa32(dev, &buf[0]);
                ssd1306_print(another_dev_row, 0, &buf[0], 0);
                
                //ID
                ssd1306_char_pos(another_dev_row, 2, pp_gps_air[dev]->device_id[0], 0);
                ssd1306_char_pos(another_dev_row, 3, pp_gps_air[dev]->device_id[1], 0);
                
                //Distance
                if (pp_gps_rel[dev]->distance < 1000)           //1-999 m
                {
                    itoa32(pp_gps_rel[dev]->distance, &buf[0]);
                    ssd1306_print_viceversa(another_dev_row, 7, &buf[0], 0);
                }
                else if (pp_gps_rel[dev]->distance < 10000)     //1.0-9.9 km
                {
                    tmpf = pp_gps_rel[dev]->distance / 1000.0;
                    ftoa32(tmpf, 1, &buf[0]);
                    ssd1306_print_viceversa(another_dev_row, 7, &buf[0], 0);
                }
                else if (pp_gps_rel[dev]->distance < 100000)    //10.-99. km
                {
                    tmpf = pp_gps_rel[dev]->distance / 1000.0;
                    itoa32((uint32_t)tmpf, &buf[0]);
                    ssd1306_print_viceversa(another_dev_row, 6, &buf[0], 0);
                    ssd1306_char_pos(another_dev_row, 7, '.', 0);
                }
                else                                        // >100 km
                {
                    ssd1306_print(another_dev_row, 5, "...", 0);
                }
                
                //Heading
                itoa32(pp_gps_rel[dev]->heading, &buf[0]);
                ssd1306_print_viceversa(another_dev_row, 11, &buf[0], 0);
                
                //Delta Altitude
                tmpi16 = pp_gps_rel[dev]->altitude_diff;
                if (tmpi16 < 0)
                {
                    tmpi16 *= -1;
                }
                
                if (tmpi16 < 1000)           //1-999 m
                {
                    itoa32(pp_gps_rel[dev]->altitude_diff, &buf[0]);
                    ssd1306_print_viceversa(another_dev_row, 16, &buf[0], 0);
                }
                else if (tmpi16 < 10000)     //1.0-9.9 km
                {
                    tmpf = pp_gps_rel[dev]->altitude_diff / 1000.0;
                    ftoa32(tmpf, 1, &buf[0]);
                    ssd1306_print_viceversa(another_dev_row, 16, &buf[0], 0);
                }
                else                         //10.-99. km
                {
                    tmpf = pp_gps_rel[dev]->altitude_diff / 1000.0;
                    itoa32((int32_t)tmpf, &buf[0]);
                    ssd1306_char_pos(another_dev_row, 16, '.', 0);
                    ssd1306_print_viceversa(another_dev_row, 15, &buf[0], 0);
                }
                
                if (pp_gps_rel[dev]->altitude_diff > 0)
                {
                    ssd1306_char('+', 0);
                }
                
                
                
                uint8_t icon_col = LCD_LAST_COL;
                
                //Battery
                if (pp_dev_aux[dev]->memory_point_flag == 0) //dont show battery icon for memory points
                {
					switch (get_device_flags(dev, FLAGS_BATTERY))
					{
						case FLAG_BATTERY_75_TO_100:
							ssd1306_char_pos(another_dev_row, icon_col--, SYMB_BAT_4OF4, 0);
							break;

						case FLAG_BATTERY_50_TO_75:
							ssd1306_char_pos(another_dev_row, icon_col--, SYMB_BAT_3OF4, 0);
							break;

						case FLAG_BATTERY_25_TO_50:
							ssd1306_char_pos(another_dev_row, icon_col--, SYMB_BAT_2OF4, 0);
							break;

						case FLAG_BATTERY_10_TO_25:
							ssd1306_char_pos(another_dev_row, icon_col--, SYMB_BAT_1OF4, 0);
							break;

						case FLAG_BATTERY_0_TO_10:
							ssd1306_char_pos(another_dev_row, icon_col--, SYMB_BAT_0OF4, 0);
							break;
					}
                }


                //Timeout
                if (pp_dev_aux[dev]->memory_point_flag == 0) //dont show timeout icon for memory points
                {
					if (pp_dev_aux[dev]->timeout_flag)
					{
						ssd1306_char_pos(another_dev_row, icon_col--, SYMB_TIMEOUT, 0);
					}
                }


                //Fence
                if (pp_dev_aux[dev]->fence_flag)
                {
                	ssd1306_char_pos(another_dev_row, icon_col--, SYMB_FENCE, 0);
                }


                //Alarm
                if (pp_dev_aux[dev]->memory_point_flag == 0) //dont show alarm icon for memory points
                {
					if (get_device_flags(dev, FLAGS_ALARM) == FLAG_ALARM_ON)
					{
						ssd1306_char_pos(another_dev_row, icon_col--, SYMB_ALARM, 0);
					}
                }

                
                another_dev_row++;      //set next row
            }
        }
    }
    
    ssd1306_update();
}



//EACH DEVICE
void draw_each_device(void)
{
    ssd1306_clear();
    
    //My status row
    uint8_t icon_col = LCD_LAST_COL;

    //Battery
    switch (get_device_flags(device_number, FLAGS_BATTERY))
    {
        case FLAG_BATTERY_75_TO_100:
            ssd1306_char_pos(0, icon_col--, SYMB_BAT_4OF4, 0);
            break;

        case FLAG_BATTERY_50_TO_75:
            ssd1306_char_pos(0, icon_col--, SYMB_BAT_3OF4, 0);
            break;

        case FLAG_BATTERY_25_TO_50:
            ssd1306_char_pos(0, icon_col--, SYMB_BAT_2OF4, 0);
            break;

        case FLAG_BATTERY_10_TO_25:
            ssd1306_char_pos(0, icon_col--, SYMB_BAT_1OF4, 0);
            break;

        case FLAG_BATTERY_0_TO_10:
            ssd1306_char_pos(0, icon_col--, SYMB_BAT_0OF4, 0);
            break;
    }

    //Sattelites
    if (p_gps_raw->time[0] == 0)                           //if no time received then no sattelites at all
    {
        ssd1306_char_pos(0, icon_col--, SYMB_NO_SATT, 0);
    }
    else
    {
        if (p_gps_num->mode == GPS_POSITION_3DFIX && p_gps_num->status == GPS_DATA_VALID)
        {
            ssd1306_char_pos(0, icon_col--, SYMB_SATT_3D, 0);   //3D when data is valid
        }
        else if (p_gps_num->mode == GPS_POSITION_2DFIX && p_gps_num->status == GPS_DATA_VALID)
        {
            ssd1306_char_pos(0, icon_col--, SYMB_SATT_2D, 0);   //2D when data is valid
        }
        else
        {
            ssd1306_char_pos(0, icon_col--, SYMB_SATT_1D, 0);   //if not 2D or 3D then only time is avaliable
        }
    }
    

    //TRX
    if (get_main_flags()->gps_sync && p_gps_num->status == GPS_DATA_VALID)
    {
        ssd1306_char_pos(0, icon_col--, SYMB_ARROW_DOWN, 0);
        ssd1306_char_pos(0, icon_col--, SYMB_ARROW_UP, 0);
    }

    
    //Timeout
    if (pp_dev_aux[device_number]->timeout_flag)
    {
        ssd1306_char_pos(0, icon_col--, SYMB_TIMEOUT, 0);
    }
    

    //Alarm
    if (get_device_flags(device_number, FLAGS_ALARM) == FLAG_ALARM_ON)
    {
        ssd1306_char_pos(0, icon_col--, SYMB_ALARM, 0);
    }
    
    
    if (current_each_device == device_number)      //if me
    {
        ssd1306_print(0, 0, "#", 0);
        itoa32(current_each_device, &buf[0]);
        ssd1306_print(0, 1, &buf[0], 0);
        
        ssd1306_char_pos(0, 3, p_settings->device_id[0], 0);
        ssd1306_char_pos(0, 4, p_settings->device_id[1], 0);
        
        ssd1306_print(0, 6, "(me)", 0);
        
        ssd1306_char_pos(1, 0, p_gps_raw->date[0], 0);
        ssd1306_char_pos(1, 1, p_gps_raw->date[1], 0);
        ssd1306_print(1, 2, ".", 0);
        
        ssd1306_char_pos(1, 3, p_gps_raw->date[2], 0);
        ssd1306_char_pos(1, 4, p_gps_raw->date[3], 0);
        ssd1306_print(1, 5, ".", 0);
        
        ssd1306_char_pos(1, 6, p_gps_raw->date[4], 0);
        ssd1306_char_pos(1, 7, p_gps_raw->date[5], 0);
        
        ssd1306_char_pos(1, 9, p_gps_raw->time[0], 0);
        ssd1306_char_pos(1, 10, p_gps_raw->time[1], 0);
        ssd1306_print(1, 11, ":", 0);
        
        ssd1306_char_pos(1, 12, p_gps_raw->time[2], 0);
        ssd1306_char_pos(1, 13, p_gps_raw->time[3], 0);
        ssd1306_print(1, 14, ":", 0);
        
        ssd1306_char_pos(1, 15, p_gps_raw->time[4], 0);
        ssd1306_char_pos(1, 16, p_gps_raw->time[5], 0);
        
        ssd1306_print(1, 18, "UTC", 0);
        
        ssd1306_print(2, 0, "LAT", 0);
        ftoa32(p_gps_num->latitude.in_deg, 6, &buf[0]);
        ssd1306_print_viceversa(2, 14, &buf[0], 0);
        if (p_gps_num->latitude.in_deg >= 0)
        {
            ssd1306_char('+', 0);
            ssd1306_print(2, 16, "(N)", 0);
        }
        else
        {
            ssd1306_print(2, 16, "(S)", 0);
        }
        
        ssd1306_print(3, 0, "LON", 0);
        ftoa32(p_gps_num->longitude.in_deg, 6, &buf[0]);
        ssd1306_print_viceversa(3, 14, &buf[0], 0);
        if (p_gps_num->longitude.in_deg >= 0)
        {
            ssd1306_char('+', 0);
            ssd1306_print(3, 16, "(E)", 0);
        }
        else
        {
            ssd1306_print(3, 16, "(W)", 0);
        }
        
        ssd1306_print(4, 0, "ALT", 0);
        itoa32(p_gps_num->altitude, &buf[0]);
        ssd1306_print(4, 4, &buf[0], 0);
        ssd1306_print_next("m", 0);
        
        ssd1306_print(4, 11, "FIX", 0);
        if (p_gps_num->status == GPS_DATA_VALID)
        {
            ssd1306_print(4, 15, "A/", 0);
        }
        else
        {
            ssd1306_print(4, 15, "V/", 0);
        }
        
        if (p_gps_num->mode == GPS_POSITION_2DFIX)
        {
            ssd1306_print(4, 17, "2D", 0);
        }
        else if (p_gps_num->mode == GPS_POSITION_3DFIX)
        {
            ssd1306_print(4, 17, "3D", 0);
        }
        else
        {
            ssd1306_print(4, 17, "NO", 0);
        }
        
        ssd1306_print(5, 0, "COG", 0);
        itoa32(p_gps_num->course, &buf[0]);
        ssd1306_print(5, 4, &buf[0], 0);
        ssd1306_char(SYMB_DEGREE, 0);
        
        ssd1306_print(5, 11, "DOP", 0);
        ftoa32(p_gps_num->pdop, 2, &buf[0]);
        ssd1306_print(5, 15, &buf[0], 0);
        
        ssd1306_print(6, 0, "SOG", 0);
        if (p_gps_num->speed < 10.0)   //if speed is small show x.x format
        {
            ftoa32(p_gps_num->speed, 1, &buf[0]);
            ssd1306_print(6, 4, &buf[0], 0);
        }
        else    //else show int format
        {
            itoa32((int16_t)p_gps_num->speed, &buf[0]);
            ssd1306_print(6, 4, &buf[0], 0);
        }
        ssd1306_print_next("kph", 0);
        
        ssd1306_print(6, 11, "SAT", 0);
        itoa32(p_gps_num->sat_used, &buf[0]);
        ssd1306_print(6, 15, &buf[0], 0);
        ssd1306_print_next("/", 0);
        itoa32(p_gps_num->sat_view, &buf[0]);
        ssd1306_print_next(&buf[0], 0);
        
        if (pp_dev_aux[device_number]->timeout_flag == 1)
        {
        	ssd1306_print(7, 0, "TOC", 1);
        }
        else
        {
        	ssd1306_print(7, 0, "TOC", 0);
        }
        convert_timeout(pp_dev_aux[device_number]->timeout, &buf[0]);
        ssd1306_print(7, 4, &buf[0], 0);
        
        ssd1306_print(7, 11, "BAT", 0);
        ftoa32(get_bat_voltage_value(), 2, &buf[0]);
        ssd1306_print(7, 15, &buf[0], 0);
        ssd1306_char('V', 0);
    }
    else                                    //if not me
    {
        ssd1306_print(0, 1, "#", 0);
        itoa32(current_each_device, &buf[0]);
        ssd1306_print(0, 2, &buf[0], 0);
        
        ssd1306_char_pos(0, 4, pp_gps_air[current_each_device]->device_id[0], 0);
        ssd1306_char_pos(0, 5, pp_gps_air[current_each_device]->device_id[1], 0);
        
        if (pp_dev_aux[current_each_device]->memory_point_flag == 1) //print memory point name
        {
        	ssd1306_print(0, 7, pp_dev_aux[current_each_device]->point_name, 0);
        }

        ssd1306_print(1, 0, "LAT", 0);
        ftoa32(pp_gps_air[current_each_device]->latitude.as_float, 6, &buf[0]);
        ssd1306_print_viceversa(1, 14, &buf[0], 0);
        if (pp_gps_air[current_each_device]->latitude.as_float >= 0)
        {
            ssd1306_char('+', 0);
            ssd1306_print(1, 16, "(N)", 0);
        }
        else
        {
            ssd1306_print(1, 16, "(S)", 0);
        }
        
        ssd1306_print(2, 0, "LON", 0);
        ftoa32(pp_gps_air[current_each_device]->longitude.as_float, 6, &buf[0]);
        ssd1306_print_viceversa(2, 14, &buf[0], 0);
        if (pp_gps_air[current_each_device]->longitude.as_float >= 0)
        {
            ssd1306_char('+', 0);
            ssd1306_print(2, 16, "(E)", 0);
        }
        else
        {
            ssd1306_print(2, 16, "(W)", 0);
        }
        
        ssd1306_print(3, 0, "ALT", 0);
        itoa32(pp_gps_air[current_each_device]->altitude.as_integer, &buf[0]);
        ssd1306_print(3, 4, &buf[0], 0);
        ssd1306_print_next("m", 0);
        
        
        ssd1306_print(3, 11, "DLT", 0);
        tmpi16 = pp_gps_rel[current_each_device]->altitude_diff;
        
        if (tmpi16 < 0)
        {
            tmpi16 *= -1;
            ssd1306_char_pos(3, 14, ' ', 0);
        }
        else if (tmpi16 > 0)
        {
            ssd1306_char_pos(3, 15, '+', 0);
        }
        else
        {
            ssd1306_char_pos(3, 14, ' ', 0);
        }
        
        if (tmpi16 < 10000)           //0-9999 m
        {
            itoa32(pp_gps_rel[current_each_device]->altitude_diff, &buf[0]);
            ssd1306_print_next(&buf[0], 0);
            ssd1306_print_next("m", 0);
        }
        else                          // >=10000 km
        {
            tmpf = pp_gps_rel[current_each_device]->altitude_diff / 1000.0;
            ftoa32(tmpf, 1, &buf[0]);
            ssd1306_print_next(&buf[0], 0);
            ssd1306_print_next("k", 0);
        }
        
        if (pp_dev_aux[current_each_device]->memory_point_flag == 0)
        {
			ssd1306_print(4, 0, "COG", 0);
			itoa32(pp_gps_air[current_each_device]->course.as_integer, &buf[0]);
			ssd1306_print(4, 4, &buf[0], 0);
			ssd1306_char(SYMB_DEGREE, 0);
        }
        
        ssd1306_print(4, 11, "BRG", 0);
        itoa32(pp_gps_rel[current_each_device]->heading, &buf[0]);
        ssd1306_print(4, 15, &buf[0], 0);
        ssd1306_char(SYMB_DEGREE, 0);
        
        if (pp_dev_aux[current_each_device]->memory_point_flag == 0)
        {
			ssd1306_print(5, 0, "SOG", 0);
			itoa32(pp_gps_air[current_each_device]->speed, &buf[0]);
			ssd1306_print(5, 4, &buf[0], 0);
			ssd1306_print_next("kph", 0);
        }
        
        
        ssd1306_print(5, 11, "DST", 0);
        
        if (pp_gps_rel[current_each_device]->distance < 100000)           // 0-99999 m
        {
            itoa32(pp_gps_rel[current_each_device]->distance, &buf[0]);
            ssd1306_print(5, 15, &buf[0], 0);
            ssd1306_print_next("m", 0);
        }
        else if (pp_gps_rel[current_each_device]->distance < 1000000)     // 100-999.9 km
        {
            tmpf = pp_gps_rel[current_each_device]->distance / 1000.0;
            ftoa32(tmpf, 1, &buf[0]);
            ssd1306_print(5, 15, &buf[0], 0);
            ssd1306_print_next("k", 0);
        }
        else                                                          // 1000-... km
        {
            tmpf = pp_gps_rel[current_each_device]->distance / 1000000.0;
            ftoa32(tmpf, 1, &buf[0]);
            ssd1306_print(5, 15, &buf[0], 0);
            ssd1306_print_next("M", 0);
        }
        
        
        if (pp_dev_aux[current_each_device]->memory_point_flag == 0)
        {
			ssd1306_print(6, 0, "FIX", 0);
			if (get_device_flags(current_each_device, FLAGS_GPS_FIX) == FLAG_GPS_FIX_3D)
			{
				ssd1306_print(6, 4, "3D/", 0);
			}
			else
			{
				ssd1306_print(6, 4, "2D/", 0);
			}

			if (get_device_flags(current_each_device, FLAGS_PDOP) == FLAG_PDOP_GOOD)
			{
				ssd1306_print(6, 7, "HI", 0);
			}
			else
			{
				ssd1306_print(6, 7, "LO", 0);
			}
        }
        

        if (pp_dev_aux[current_each_device]->memory_point_flag == 0)
        {
			ssd1306_print(6, 11, "BAT", 0);
			switch (get_device_flags(current_each_device, FLAGS_BATTERY))
			{
				case FLAG_BATTERY_75_TO_100:
					ssd1306_print(6, 15, "4/4", 0);
					break;

				case FLAG_BATTERY_50_TO_75:
					ssd1306_print(6, 15, "3/4", 0);
					break;

				case FLAG_BATTERY_25_TO_50:
					ssd1306_print(6, 15, "2/4", 0);
					break;

				case FLAG_BATTERY_10_TO_25:
					ssd1306_print(6, 15, "1/4", 0);
					break;

				case FLAG_BATTERY_0_TO_10:
					ssd1306_print(6, 15, "0/4", 0);
					break;
			}
        }
        
        if (pp_dev_aux[current_each_device]->timeout_flag == 1)
        {
        	ssd1306_print(7, 0, "TOC", 1);
        }
        else
        {
        	ssd1306_print(7, 0, "TOC", 0);
        }
        convert_timeout(pp_dev_aux[current_each_device]->timeout, &buf[0]);
        ssd1306_print(7, 4, &buf[0], 0);
        
        

		if (pp_dev_aux[current_each_device]->fence_flag)
		{
			ssd1306_print(7, 11, "FNC", 1);		//print inverted
		}
		else
		{
			ssd1306_print(7, 11, "FNC", 0);
		}


        if (pp_dev_aux[current_each_device]->memory_point_flag == 0)
        {
			if (get_device_flags(current_each_device, FLAGS_ALARM) == FLAG_ALARM_ON)
			{
				ssd1306_print(7, 15, "ALR", 1);		//print inverted
			}
			else
			{
				ssd1306_print(7, 15, "ALR", 0);
			}
        }

    }
    ssd1306_update();
}



void draw_each_device_submenu(void)
{
	#define EACH_DEV_SM_ROW               (2)
	#define EACH_DEV_SM_COL               (1)

	ssd1306_clear();

	ssd1306_print(0, EACH_DEV_SM_COL, "DEVICE", 0);

	ssd1306_print(0, EACH_DEV_SM_COL + 7, "#", 0);
    itoa32(current_each_device, &buf[0]);
    ssd1306_print(0, EACH_DEV_SM_COL + 8, &buf[0], 0);

    ssd1306_char_pos(0, EACH_DEV_SM_COL + 10, pp_gps_air[current_each_device]->device_id[0], 0);
    ssd1306_char_pos(0, EACH_DEV_SM_COL + 11, pp_gps_air[current_each_device]->device_id[1], 0);

	ssd1306_print(EACH_DEV_SM_ROW, EACH_DEV_SM_COL, "Save", 0);
	ssd1306_print(EACH_DEV_SM_ROW + 1, EACH_DEV_SM_COL, "Delete", 0);
	ssd1306_print(EACH_DEV_SM_ROW + get_current_item(), EACH_DEV_SM_COL - 1, ">", 0);

	ssd1306_update();
}



void draw_delete_device(void)
{
    ssd1306_clear();
    ssd1306_print(0, 1, "Delete device", 0);

	ssd1306_print(0, 15, "#", 0);
    itoa32(current_each_device, &buf[0]);
    ssd1306_print(0, 16, &buf[0], 0);

    ssd1306_char_pos(0, 18, pp_gps_air[current_each_device]->device_id[0], 0);
    ssd1306_char_pos(0, 19, pp_gps_air[current_each_device]->device_id[1], 0);

    ssd1306_print_next("?", 0);


    if (current_each_device == device_number)
    {
    	ssd1306_print(3, 1, "Can't del yourself", 0);
    }
    else
    {
		ssd1306_print(3, 1, "OK - delete", 0);
    }

    ssd1306_print(4, 1, "ESC - cancel", 0);

    ssd1306_update();
}



void delete_device_ok(void)
{
	if (current_each_device != device_number)
	{
		pp_dev_aux[current_each_device]->exist_flag = 0;	//delete device just by resetting exist flag
		current_each_device = device_number;	//also reset the current device
		current_radar_device = 0;				//reset current dev in radar menu
		radar_list_hide[current_each_device] = 0;//reset hide flag

		reset_current_item_in_menu(M_EACH_DEVICE_SUBMENU);
		current_menu = M_DEVICES;
		draw_current_menu();
	}
}



void draw_save_device(void)
{
	#define SAVE_DEVICE_ROW               (2)
	#define SAVE_DEVICE_COL               (1)

	memcpy(point_to_save_name, point_to_save_default_name, sizeof(point_to_save_default_name)); //init default name here

	ssd1306_clear();

	ssd1306_print(0, SAVE_DEVICE_COL, "Where to save", 0);

	ssd1306_print(0, SAVE_DEVICE_COL + 14, "#", 0);
    itoa32(current_each_device, &buf[0]);
    ssd1306_print(0, SAVE_DEVICE_COL + 15, &buf[0], 0);

    ssd1306_char_pos(0, SAVE_DEVICE_COL + 17, pp_gps_air[current_each_device]->device_id[0], 0);
    ssd1306_char_pos(0, SAVE_DEVICE_COL + 18, pp_gps_air[current_each_device]->device_id[1], 0);

    ssd1306_print_next("?", 0);

    read_memory_slots();

    uint8_t another_point_to_save_row = SAVE_DEVICE_ROW;
    uint8_t points_enlisted_cntr = 0;

    for (uint8_t s = 1; s <= MEMORY_SLOTS_TOTAL; s++)
    {
		if (pp_memory_slot[s]->exist_flag == 0)
		{
			point_to_save_list[s] = points_enlisted_cntr++; 		//first nonexisting point will get index 0

			if (current_slot_to_save == 0) //if for the first time
			{
				current_slot_to_save = s;
			}

			//Slot name
			ssd1306_print(another_point_to_save_row, SAVE_DEVICE_COL, pp_memory_slot[s]->slot_name, 0);

			another_point_to_save_row++;
		}
    }

    if (points_enlisted_cntr > 0)
    {
    	ssd1306_print(SAVE_DEVICE_ROW + point_to_save_list[current_slot_to_save], SAVE_DEVICE_COL - 1, ">", 0);
    }
    else
    {
    	ssd1306_print(SAVE_DEVICE_ROW, SAVE_DEVICE_COL, "no empty slots", 0);
    	current_slot_to_save = 0;
    }

    ssd1306_update();
}



void draw_save_device_as(void)
{
	#define SAVE_DEV_AS_ROW                 (2)
	#define SAVE_DEV_AS_COL                 (1)
	#define SAVE_DEV_AS_PARAM_COL           (11)

	ssd1306_clear();
	ssd1306_print(0, SAVE_DEV_AS_COL, "Set point name", 0);

	ssd1306_print(SAVE_DEV_AS_ROW, SAVE_DEV_AS_COL, "Name", 0);
	ssd1306_print(SAVE_DEV_AS_ROW, SAVE_DEV_AS_PARAM_COL, point_to_save_name, 0);
	ssd1306_print(SAVE_DEV_AS_ROW + 1, SAVE_DEV_AS_PARAM_COL + point_name_current_symbol, "^", 0);
	ssd1306_print(SAVE_DEV_AS_ROW + 3, SAVE_DEV_AS_COL, "Long OK - save", 0);
	ssd1306_update();
}



void draw_saved_popup(void)
{
	ssd1306_clear();
	ssd1306_print(0, 1, "Saved!", 0);
	ssd1306_update();
}



//RADAR
void draw_radar(void)
{
	ssd1306_clear();
    ssd1306_bitmap(&radar_blank[0]);

    //My status row
    uint8_t icon_col = LCD_LAST_COL;


    //Battery
    switch (get_device_flags(device_number, FLAGS_BATTERY))
    {
        case FLAG_BATTERY_75_TO_100:
            ssd1306_char_pos(0, icon_col--, SYMB_BAT_4OF4, 0);
            break;

        case FLAG_BATTERY_50_TO_75:
            ssd1306_char_pos(0, icon_col--, SYMB_BAT_3OF4, 0);
            break;

        case FLAG_BATTERY_25_TO_50:
            ssd1306_char_pos(0, icon_col--, SYMB_BAT_2OF4, 0);
            break;

        case FLAG_BATTERY_10_TO_25:
            ssd1306_char_pos(0, icon_col--, SYMB_BAT_1OF4, 0);
            break;

        case FLAG_BATTERY_0_TO_10:
            ssd1306_char_pos(0, icon_col--, SYMB_BAT_0OF4, 0);
            break;
    }


    //Sattelites
    if (p_gps_raw->time[0] == 0)                           //if no time received then no sattelites at all
    {
        ssd1306_char_pos(0, icon_col--, SYMB_NO_SATT, 0);
    }
    else
    {
        if (p_gps_num->mode == GPS_POSITION_3DFIX && p_gps_num->status == GPS_DATA_VALID)
        {
            ssd1306_char_pos(0, icon_col--, SYMB_SATT_3D, 0);   //3D when data is valid
        }
        else if (p_gps_num->mode == GPS_POSITION_2DFIX && p_gps_num->status == GPS_DATA_VALID)
        {
            ssd1306_char_pos(0, icon_col--, SYMB_SATT_2D, 0);   //2D when data is valid
        }
        else
        {
            ssd1306_char_pos(0, icon_col--, SYMB_SATT_1D, 0);   //if not 2D or 3D then only time is avaliable
        }
    }

    //TRX
    if (get_main_flags()->gps_sync && p_gps_num->status == GPS_DATA_VALID)
    {
        ssd1306_char_pos(0, icon_col--, SYMB_ARROW_DOWN, 0);
        ssd1306_char_pos(0, icon_col--, SYMB_ARROW_UP, 0);
    }



    //Timeout
    if (pp_dev_aux[device_number]->timeout_flag)
    {
        ssd1306_char_pos(0, icon_col--, SYMB_TIMEOUT, 0);
    }

    //Alarm
    if (get_device_flags(device_number, FLAGS_ALARM) == FLAG_ALARM_ON)
    {
        ssd1306_char_pos(0, icon_col--, SYMB_ALARM, 0);
    }



    //COG
    itoa32(pp_gps_air[device_number]->course.as_integer, &buf[0]);
    ssd1306_print_viceversa(0, 7, &buf[0], 0);



    uint8_t another_radar_dev_row = ANOTHER_RADAR_DEVICE_START_ROW;
    uint32_t max_distance = 0;
    uint8_t devices_enlisted_cntr = 0;

    for (uint8_t dev = 1; dev <= DEVICES_IN_GROUP; dev++)
    {
    	if (dev != device_number)
    	{
    		if (pp_dev_aux[dev]->exist_flag == 1)
    		{
    			radar_list[dev] = devices_enlisted_cntr++; //first existing device will get index 0

    			if (current_radar_device == 0) //if for the first time
    			{
    				current_radar_device = dev;
    			}

                //Number
    			itoa32(dev, &buf[0]);
    			if (radar_list_hide[dev] == 1)
    			{
					ssd1306_print(another_radar_dev_row, 9, &buf[0], 1);
    			}
    			else
    			{
					ssd1306_print(another_radar_dev_row, 9, &buf[0], 0);
    			}

                //ID
                ssd1306_char_pos(another_radar_dev_row, 11, pp_gps_air[dev]->device_id[0], 0);
                ssd1306_char_pos(another_radar_dev_row, 12, pp_gps_air[dev]->device_id[1], 0);

                //Distance
                if (pp_gps_rel[dev]->distance < 1000)           //1-999 m
                {
                    itoa32(pp_gps_rel[dev]->distance, &buf[0]);
                    ssd1306_print_viceversa(another_radar_dev_row, 16, &buf[0], 0);
                }
                else if (pp_gps_rel[dev]->distance < 10000)     //1.0-9.9 km
                {
                    tmpf = pp_gps_rel[dev]->distance / 1000.0;
                    ftoa32(tmpf, 1, &buf[0]);
                    ssd1306_print_viceversa(another_radar_dev_row, 16, &buf[0], 0);
                }
                else if (pp_gps_rel[dev]->distance < 100000)    //10.-99. km
                {
                    tmpf = pp_gps_rel[dev]->distance / 1000.0;
                    itoa32((uint32_t)tmpf, &buf[0]);
                    ssd1306_print_viceversa(another_radar_dev_row, 16, &buf[0], 0);
                    ssd1306_char_pos(another_radar_dev_row, 17, '.', 0);
                }
                else                                        // >100 km
                {
                    ssd1306_print(another_radar_dev_row, 14, "...", 0);
                }

                //Search max distance
                if (pp_gps_rel[dev]->distance > max_distance)
                {
                	if (radar_list_hide[dev] == 0)
                	{
                		max_distance = pp_gps_rel[dev]->distance;		//fix max distance only if device is not hided
                	}
                }

                //Heading
                itoa32(pp_gps_rel[dev]->heading, &buf[0]);
                ssd1306_print_viceversa(another_radar_dev_row, 20, &buf[0], 0);


                //Warning sign in case of timeout, alarm or fence
                if (get_device_flags(dev, FLAGS_ALARM) || pp_dev_aux[dev]->timeout_flag || pp_dev_aux[dev]->fence_flag)
                {
                	ssd1306_print_byte(another_radar_dev_row, 21, custom_exclam_mark, 2);
                }

                another_radar_dev_row++;
    		}
    		else	//if device is not exist
    		{
    			radar_list_hide[dev] = 0;	//clear hide flag, so when device disappear, and appear again it will be not hided
    		}

    	}
    }


    //Pointer and bottom row information bar
    if (devices_enlisted_cntr > 0)
    {
    	ssd1306_print(2 + radar_list[current_radar_device], 8, ">", 0);

    	uint8_t icon_col = LCD_LAST_COL;

        if (pp_dev_aux[current_radar_device]->memory_point_flag == 0) //dont show battery icon for memory points
        {
			//Battery
			switch (get_device_flags(current_radar_device, FLAGS_BATTERY))
			{
				case FLAG_BATTERY_75_TO_100:
					ssd1306_char_pos(7, icon_col--, SYMB_BAT_4OF4, 0);
					break;

				case FLAG_BATTERY_50_TO_75:
					ssd1306_char_pos(7, icon_col--, SYMB_BAT_3OF4, 0);
					break;

				case FLAG_BATTERY_25_TO_50:
					ssd1306_char_pos(7, icon_col--, SYMB_BAT_2OF4, 0);
					break;

				case FLAG_BATTERY_10_TO_25:
					ssd1306_char_pos(7, icon_col--, SYMB_BAT_1OF4, 0);
					break;

				case FLAG_BATTERY_0_TO_10:
					ssd1306_char_pos(7, icon_col--, SYMB_BAT_0OF4, 0);
					break;
			}
        }


        if (pp_dev_aux[current_radar_device]->memory_point_flag == 0)
        {
			//Timeout
			if (pp_dev_aux[current_radar_device]->timeout_flag)
			{
				ssd1306_char_pos(7, icon_col--, SYMB_TIMEOUT, 0);
			}
        }


        //Fence
        if (pp_dev_aux[current_radar_device]->fence_flag)
        {
        	ssd1306_char_pos(7, icon_col--, SYMB_FENCE, 0);
        }


        if (pp_dev_aux[current_radar_device]->memory_point_flag == 0)
        {
			//Alarm
			if (get_device_flags(current_radar_device, FLAGS_ALARM) == FLAG_ALARM_ON)
			{
				ssd1306_char_pos(7, icon_col--, SYMB_ALARM, 0);
			}
        }


        //if mem point then print it name
        if (pp_dev_aux[current_radar_device]->memory_point_flag == 1)
        {
        	ssd1306_print(7, 9, pp_dev_aux[current_radar_device]->point_name, 0);
        }


    }
    else
    {
    	current_radar_device = 0;
    }


    //Radar implementation is here
    float mpd_ratio_f = 0.0;		//meters per dot ratio
    uint32_t mpd_ratio_i = 0;

    if (max_distance < r_circ_dots)
    {
    	max_distance = r_circ_dots;
    }

    mpd_ratio_f = (float)max_distance / (float)r_circ_dots;	//calc ratio
    mpd_ratio_i = (uint32_t)mpd_ratio_f;			//extract int part

    mpd_ratio_f -= mpd_ratio_i;					//extract frac part

    if (mpd_ratio_f > 0.001)					//if frac part is non-zero, then inc ratio
    {
    	mpd_ratio_i++;
    }

    //print radar range
    uint32_t radar_range = mpd_ratio_i * r_circ_dots;	//i.e. radar screen radius

    if (radar_range < 10000) // 0...9999 meters, screen fits 4 characters max
    {
    	itoa32(radar_range, &buf[0]);
    	ssd1306_print_viceversa(7, 7, &buf[0], 0);
    }
    else if (radar_range < 1000000)	// 10000...999999 meters
    {
    	itoa32((radar_range / 1000), &buf[0]);
    	ssd1306_char_pos(7, 7, 'k', 0);
    	ssd1306_print_viceversa(7, 6, &buf[0], 0);
    }
    else
    {
    	ssd1306_print_viceversa(7, 7, "...", 0);
    }


    //plot on radar
    for (uint8_t dev = 1; dev <= DEVICES_IN_GROUP; dev++)
    {
    	if (dev != device_number)
    	{
    		if (pp_dev_aux[dev]->exist_flag == 1)
    		{
    			if (radar_list_hide[dev] == 0)		//if not hided
    			{
					uint8_t sx1 = 0;		//device coordinates on the screen
					uint8_t sy1 = 0;

					double x1 = 0.0;	//device coordinates relative from my position (0, 0)
					double y1 = 0.0;

					x1 = -((pp_gps_rel[dev]->distance)/(double)mpd_ratio_i) * cos((pp_gps_rel[dev]->heading + 90) * deg_to_rad);
					y1 = ((pp_gps_rel[dev]->distance)/(double)mpd_ratio_i) * sin((pp_gps_rel[dev]->heading + 90) * deg_to_rad);

					sx1 = sx0 + (int8_t)x1;
					sy1 = sy0 - (int8_t)y1;		//note: Y axis is inverted (counts from top to the bottom)


					ssd1306_pixel(sx1, sy1+1, 1);	//pixel cross
					ssd1306_pixel(sx1+1, sy1, 1);
					ssd1306_pixel(sx1, sy1-1, 1);
					ssd1306_pixel(sx1-1, sy1, 1);

					//center pixel if device is selected
					if (dev == current_radar_device)
					{
						ssd1306_pixel(sx1, sy1, 1);
					}
    			}
    		}
    	}
    }

	ssd1306_update();
}



//POINTS
void draw_points(void)
{
	#define POINTS_ROW               (2)
	#define POINTS_COL               (1)
	#define POINTS_NAME_COL          (4)
	#define POINTS_DATE_COL          (10)

	read_memory_slots();

	ssd1306_clear();
	ssd1306_print(0, POINTS_COL, "POINTS", 0);

	for (uint8_t s = 1; s <= MEMORY_SLOTS_TOTAL; s++)
	{
		ssd1306_print(POINTS_ROW + s - 1, POINTS_COL, pp_memory_slot[s]->slot_name, 0);	//print slot name

		if (pp_memory_slot[s]->exist_flag == 1)
		{
			ssd1306_print(POINTS_ROW + s - 1, POINTS_NAME_COL, pp_memory_slot[s]->point_name, 0);	//print point name

	        ssd1306_char_pos(POINTS_ROW + s - 1, POINTS_DATE_COL, pp_memory_slot[s]->save_date[0], 0);	//print point save date
	        ssd1306_char_pos(POINTS_ROW + s - 1, POINTS_DATE_COL + 1, pp_memory_slot[s]->save_date[1], 0);
	        ssd1306_print(POINTS_ROW + s - 1, POINTS_DATE_COL + 2, ".", 0);

	        ssd1306_char_pos(POINTS_ROW + s - 1, POINTS_DATE_COL + 3, pp_memory_slot[s]->save_date[2], 0);
	        ssd1306_char_pos(POINTS_ROW + s - 1, POINTS_DATE_COL + 4, pp_memory_slot[s]->save_date[3], 0);
	        ssd1306_print(POINTS_ROW + s - 1, POINTS_DATE_COL + 5, ".", 0);

	        ssd1306_char_pos(POINTS_ROW + s - 1, POINTS_DATE_COL + 6, pp_memory_slot[s]->save_date[4], 0);
	        ssd1306_char_pos(POINTS_ROW + s - 1, POINTS_DATE_COL + 7, pp_memory_slot[s]->save_date[5], 0);
		}
		else
		{
			ssd1306_print(POINTS_ROW + s - 1, POINTS_NAME_COL, "empty", 0);
		}
	}

	ssd1306_print(POINTS_ROW + current_slot_to_load - 1, POINTS_COL - 1, ">", 0);	//print pointer

	ssd1306_update();
}



void draw_each_point(void)
{
    #define EACH_POINT_ROW               (2)
    #define EACH_POINT_COL               (1)

    ssd1306_clear();

	ssd1306_print(0, EACH_POINT_COL, "POINT ", 0);
	ssd1306_print_next(pp_memory_slot[current_slot_to_load]->slot_name, 0);

	ssd1306_print(EACH_POINT_ROW, EACH_POINT_COL, "Load", 0);
	ssd1306_print(EACH_POINT_ROW + 1, EACH_POINT_COL, "Delete", 0);
	ssd1306_print(EACH_POINT_ROW + get_current_item(), EACH_POINT_COL - 1, ">", 0);

    ssd1306_update();
}



void draw_load_point(void)
{
	#define LOAD_POINT_ROW               (2)
	#define LOAD_POINT_COL               (1)

	ssd1306_clear();

	ssd1306_print(0, LOAD_POINT_COL, "Where to load ", 0);
	ssd1306_print_next(pp_memory_slot[current_slot_to_load]->slot_name, 0);
	ssd1306_print_next("?", 0);

    uint8_t another_device_to_load_row = LOAD_POINT_ROW;
    uint8_t devices_enlisted_cntr = 0;

    for (uint8_t dev = 1; dev <= DEVICES_IN_GROUP; dev++)
    {
		if (pp_dev_aux[dev]->exist_flag == 0)
		{
			device_to_load_list[dev] = devices_enlisted_cntr++; 		//first nonexisting device will get index 0

			if (current_device_to_load == 0) //if for the first time
			{
				current_device_to_load = dev;
			}

			//Device
			ssd1306_print(another_device_to_load_row, LOAD_POINT_COL, "Device ", 0);

            //Number
			itoa32(dev, &buf[0]);
			ssd1306_print_next(&buf[0], 0);

			another_device_to_load_row++;
		}
    }

    if (devices_enlisted_cntr > 0)
    {
    	ssd1306_print(LOAD_POINT_ROW + device_to_load_list[current_device_to_load], LOAD_POINT_COL - 1, ">", 0);
    }
    else
    {
    	ssd1306_print(LOAD_POINT_ROW, LOAD_POINT_COL, "no empty devices", 0);
    	current_device_to_load = 0;
    }

	ssd1306_update();
}



void draw_delete_point(void)
{
    ssd1306_clear();
    ssd1306_print(0, 1, "Delete point ", 0);
    ssd1306_print_next(pp_memory_slot[current_slot_to_load]->slot_name, 0);
    ssd1306_print_next("?", 0);
    ssd1306_print(3, 1, "OK - delete", 0);
    ssd1306_print(4, 1, "ESC - cancel", 0);
    ssd1306_update();
}



//SETTINGS
void draw_settings(void)
{
    #define SETTINGS_ROW               (2)
    #define SETTINGS_COL               (1)
    
    ssd1306_clear();
    ssd1306_print(0, SETTINGS_COL, "SETTINGS", 0);
    ssd1306_print(SETTINGS_ROW, SETTINGS_COL, "Edit", 0);
    ssd1306_print(SETTINGS_ROW + 1, SETTINGS_COL, "Restore", 0);
    ssd1306_print(SETTINGS_ROW + 2, SETTINGS_COL, "Erase", 0);
    ssd1306_print(SETTINGS_ROW + get_current_item(), SETTINGS_COL - 1, ">", 0);
    ssd1306_update();
}



//INFO
void draw_info(void)
{
    ssd1306_clear();

    ssd1306_bitmap(&info_blank[0]);

    ssd1306_print(0, 0, "LRNS Eleph", 0);

    ssd1306_print(2, 0, "HW/FW: ", 0);
    ssd1306_print_next(HW_VERSION, 0);
    ssd1306_print_next("/", 0);
    ssd1306_print_next(FW_VERSION, 0);

    ssd1306_print(3, 0, __TIME__, 0);
    ssd1306_print(4, 0, __DATE__, 0);

    ssd1306_print(7, 0, "(C)2021 Feruz Topalov", 0);

    ssd1306_update();
}



//EDIT SETTINGS
void draw_edit_settings(void)
{
    #define EDIT_SETTINGS_ROW               (1)
    #define EDIT_SETTINGS_COL               (1)
    #define EDIT_SETTINGS_PARAM_COL         (15)
    
    ssd1306_clear();
    ssd1306_print(0, EDIT_SETTINGS_COL + 1, "EDIT SETTINGS", 0);
    
    ssd1306_print(EDIT_SETTINGS_ROW, EDIT_SETTINGS_COL, "Device number", 0);
    itoa32(settings_copy.device_number, &buf[0]);
    ssd1306_print(EDIT_SETTINGS_ROW, EDIT_SETTINGS_PARAM_COL, &buf[0], 0);
    
    ssd1306_print(EDIT_SETTINGS_ROW + 1, EDIT_SETTINGS_COL, "Device ID", 0);
    buf[0] = settings_copy.device_id[0];
    buf[1] = settings_copy.device_id[1];
    buf[2] = 0;
    ssd1306_print(EDIT_SETTINGS_ROW + 1, EDIT_SETTINGS_PARAM_COL, &buf[0], 0);
    
    ssd1306_print(EDIT_SETTINGS_ROW + 2, EDIT_SETTINGS_COL, "Freq channel", 0);
    itoa32(settings_copy.freq_channel, &buf[0]);
    ssd1306_print(EDIT_SETTINGS_ROW + 2, EDIT_SETTINGS_PARAM_COL, &buf[0], 0);
    
    ssd1306_print(EDIT_SETTINGS_ROW + 3, EDIT_SETTINGS_COL, "TX power", 0);
    itoa32(p_get_tx_power_values[settings_copy.tx_power_opt], &buf[0]);
    ssd1306_print(EDIT_SETTINGS_ROW + 3, EDIT_SETTINGS_PARAM_COL, &buf[0], 0);
    ssd1306_print_next(" mW", 0);
    
    ssd1306_print(EDIT_SETTINGS_ROW + 4, EDIT_SETTINGS_COL, "Send interval", 0);
    itoa32(p_send_interval_values[settings_copy.send_interval_opt], &buf[0]);
    ssd1306_print(EDIT_SETTINGS_ROW + 4, EDIT_SETTINGS_PARAM_COL, &buf[0], 0);
    ssd1306_print_next(" s", 0);

    ssd1306_print(EDIT_SETTINGS_ROW + 5, EDIT_SETTINGS_COL, "Timeout thr", 0);
    itoa32(settings_copy.timeout_threshold.as_integer, &buf[0]);
    ssd1306_print(EDIT_SETTINGS_ROW + 5, EDIT_SETTINGS_PARAM_COL, &buf[0], 0);
    ssd1306_print_next(" s", 0);

    ssd1306_print(EDIT_SETTINGS_ROW + 6, EDIT_SETTINGS_COL, "Fence thr", 0);
    itoa32(settings_copy.fence_threshold.as_integer, &buf[0]);
    ssd1306_print(EDIT_SETTINGS_ROW + 6, EDIT_SETTINGS_PARAM_COL, &buf[0], 0);
    ssd1306_print_next(" m", 0);

    ssd1306_print(EDIT_SETTINGS_ROW + get_current_item(), EDIT_SETTINGS_COL - 1, ">", 0);
    ssd1306_update();
}



//SET DEV NUM
void draw_set_dev_num(void)
{
    #define SET_DEV_NUM_ROW                 (2)
    #define SET_DEV_NUM_COL                 (1)
    #define SET_DEV_NUM_PARAM_COL           (15)
    
    ssd1306_clear();
    ssd1306_print(0, SET_DEV_NUM_COL, "SET DEV NUM", 0);
    
    ssd1306_print(SET_DEV_NUM_ROW, SET_DEV_NUM_COL, "Device number", 0);
    itoa32(settings_copy.device_number, &buf[0]);
    ssd1306_print(SET_DEV_NUM_ROW, SET_DEV_NUM_PARAM_COL, &buf[0], 0);
    ssd1306_print(SET_DEV_NUM_ROW + 1, SET_DEV_NUM_PARAM_COL, "^", 0);
    ssd1306_update();
}



//SET DEV ID
void draw_set_dev_id(void)
{
    #define SET_DEV_ID_ROW                 (2)
    #define SET_DEV_ID_COL                 (1)
    #define SET_DEV_ID_PARAM_COL           (15)
    
    ssd1306_clear();
    ssd1306_print(0, SET_DEV_NUM_COL, "SET DEV ID", 0);
    
    ssd1306_print(SET_DEV_ID_ROW, SET_DEV_ID_COL, "Device ID", 0);
    buf[0] = settings_copy.device_id[0];
    buf[1] = settings_copy.device_id[1];
    buf[2] = 0;
    ssd1306_print(SET_DEV_ID_ROW, SET_DEV_ID_PARAM_COL, &buf[0], 0);
    ssd1306_print(SET_DEV_ID_ROW + 1, SET_DEV_ID_PARAM_COL + device_id_current_symbol, "^", 0);
    ssd1306_print(SET_DEV_ID_ROW + 3, SET_DEV_ID_COL, "Long OK - apply", 0);
    ssd1306_update();
}



//SET FREQ CH
void draw_set_freq_ch(void)
{
    #define SET_FREQ_CH_ROW         (2)
    #define SET_FREQ_CH_COL         (1)
    #define SET_FREQ_CH_PARAM_COL   (16)
    
    ssd1306_clear();
    ssd1306_print(0, SET_FREQ_CH_COL, "SET FREQ CH", 0);
    
    ssd1306_print(SET_FREQ_CH_ROW, SET_FREQ_CH_COL, "Freq channel", 0);
    itoa32(settings_copy.freq_channel, &buf[0]);
    ssd1306_print_viceversa(SET_FREQ_CH_ROW, SET_FREQ_CH_PARAM_COL, &buf[0], 0);
    ssd1306_print(SET_FREQ_CH_ROW + 1, SET_FREQ_CH_PARAM_COL, "^", 0);
    ssd1306_update();
}



//SET TX POW
void draw_set_tx_pow(void)
{
    #define SET_TX_POW_ROW          (2)
    #define SET_TX_POW_COL          (1)
    #define SET_TX_POW_PARAM_COL    (15)
    
    ssd1306_clear();
    ssd1306_print(0, SET_TX_POW_COL, "SET TX POW", 0);
    
    ssd1306_print(SET_TX_POW_ROW, SET_TX_POW_COL, "TX power", 0);
    itoa32(p_get_tx_power_values[settings_copy.tx_power_opt], &buf[0]);
    ssd1306_print(SET_TX_POW_ROW, SET_TX_POW_PARAM_COL, &buf[0], 0);
    ssd1306_print_next(" mW", 0);
    ssd1306_print(SET_TX_POW_ROW + 1, SET_TX_POW_PARAM_COL, "^", 0);
    ssd1306_update();
}



//SET SEND INTRVL
void draw_set_send_intvl(void)
{
    #define SET_SEND_INTVL_ROW                 (2)
    #define SET_SEND_INTVL_COL                 (1)
    #define SET_SEND_INTVL_PARAM_COL           (16)

    ssd1306_clear();
    ssd1306_print(0, SET_SEND_INTVL_COL, "SET SEND INTVL", 0);

    ssd1306_print(SET_SEND_INTVL_ROW, SET_SEND_INTVL_COL, "Send interval", 0);
    itoa32(p_send_interval_values[settings_copy.send_interval_opt], &buf[0]);
    ssd1306_print(SET_SEND_INTVL_ROW, SET_SEND_INTVL_PARAM_COL, &buf[0], 0);
    ssd1306_print_next(" s", 0);
    ssd1306_print(SET_SEND_INTVL_ROW + 1, SET_SEND_INTVL_PARAM_COL, "^", 0);
    ssd1306_update();
}



//SET TIMEOUT THR
void draw_set_to_thr(void)
{
    #define SET_TO_THR_ROW                 (2)
    #define SET_TO_THR_COL                 (1)
    #define SET_TO_THR_PARAM_COL           (14)

    ssd1306_clear();
    ssd1306_print(0, SET_TO_THR_COL, "SET TO THR", 0);

    ssd1306_print(SET_TO_THR_ROW, SET_TO_THR_COL, "Timeout thr", 0);
    itoa32(settings_copy.timeout_threshold.as_integer, &buf[0]);
    ssd1306_print(SET_TO_THR_ROW, SET_TO_THR_PARAM_COL, &buf[0], 0);
    ssd1306_print_next(" s", 0);
    ssd1306_print(SET_TO_THR_ROW + 1, SET_TO_THR_PARAM_COL, "^", 0);
    ssd1306_update();
}



//SET TIMEOUT THR
void draw_set_fnc_thr(void)
{
    #define SET_FNC_THR_ROW                 (2)
    #define SET_FNC_THR_COL                 (1)
    #define SET_FNC_THR_PARAM_COL           (14)

    ssd1306_clear();
    ssd1306_print(0, SET_FNC_THR_COL, "SET FNC THR", 0);

    ssd1306_print(SET_FNC_THR_ROW, SET_FNC_THR_COL, "Fence thr", 0);
    itoa32(settings_copy.fence_threshold.as_integer, &buf[0]);
    ssd1306_print(SET_FNC_THR_ROW, SET_FNC_THR_PARAM_COL, &buf[0], 0);
    ssd1306_print_next(" m", 0);
    ssd1306_print(SET_FNC_THR_ROW + 1, SET_FNC_THR_PARAM_COL, "^", 0);
    ssd1306_update();
}



//CONFIRM SETTINGS SAVE
void draw_confirm_settings_save(void)
{
    if (flag_settings_changed)
    {
        ssd1306_clear();
        ssd1306_print(0, 1, "Settings changed", 0);
        ssd1306_print(3, 1, "OK - save & restart", 0);
        ssd1306_print(4, 1, "ESC - cancel changes", 0);
        ssd1306_update();
    }
    else
    {
        current_menu = M_SETTINGS;
        draw_current_menu();
    }
}



//RESTORE DEFAULTS
void draw_restore_defaults(void)
{
    ssd1306_clear();
    ssd1306_print(0, 1, "Restore defaults?", 0);
    ssd1306_print(3, 1, "OK - restore & reset", 0);
    ssd1306_print(4, 1, "ESC - cancel", 0);
    ssd1306_update();
}



//ERASE ALL
void draw_erase_all(void)
{
    ssd1306_clear();
    ssd1306_print(0, 1, "Erase all EEPROM?", 0);
    ssd1306_print(3, 1, "OK - erase & reset", 0);
    ssd1306_print(4, 1, "ESC - cancel", 0);
    ssd1306_update();
}



void devices_ok(void)
{
    if (pp_dev_aux[current_each_device]->exist_flag == 0)
    {
        each_device_up();
    }
    
    current_menu = M_EACH_DEVICE;
    draw_current_menu();
}



void each_device_up(void)
{
    do
    {
        if (current_each_device == DEVICE_NUMBER_FIRST)
        {
            current_each_device = DEVICE_NUMBER_LAST;
        }
        else
        {
            current_each_device--;
        }
    }
    while (pp_dev_aux[current_each_device]->exist_flag == 0);
    
    draw_current_menu();
}



void each_device_down(void)
{
    do
    {
        if (current_each_device == DEVICE_NUMBER_LAST)
        {
            current_each_device = DEVICE_NUMBER_FIRST;
        }
        else
        {
            current_each_device++;
        }
    }
    while (pp_dev_aux[current_each_device]->exist_flag == 0);
    
    draw_current_menu();
}



void each_device_ok(void)
{
	current_menu = M_EACH_DEVICE_SUBMENU;
	draw_current_menu();
}



void save_device_up(void)
{
	if (current_slot_to_save != 0)
	{
		do
		{
			if (current_slot_to_save == MEMORY_SLOT_FIRST)
			{
				current_slot_to_save = MEMORY_SLOT_LAST;
			}
			else
			{
				current_slot_to_save--;
			}
		}
		while (pp_memory_slot[current_slot_to_save]->exist_flag == 1);

		draw_current_menu();
	}
}



void save_device_down(void)
{
	if (current_slot_to_save != 0)
	{
		do
		{
			if (current_slot_to_save == MEMORY_SLOT_LAST)
			{
				current_slot_to_save = MEMORY_SLOT_FIRST;
			}
			else
			{
				current_slot_to_save++;
			}
		}
		while (pp_memory_slot[current_slot_to_save]->exist_flag == 1);

		draw_current_menu();
	}
}



void save_device_ok(void)
{
	if (current_slot_to_save != 0)
	{
		current_menu = M_SAVE_DEVICE_AS;
		draw_current_menu();
	}
}



void save_device_esc(void)
{
	current_slot_to_save = 0;
	current_menu = M_EACH_DEVICE_SUBMENU;
	draw_current_menu();
}



void save_device_as_up(void)
{
    if (point_to_save_name[point_name_current_symbol] == POINT_NAME_LAST_SYMBOL)
    {
    	point_to_save_name[point_name_current_symbol] = POINT_NAME_FIRST_SYMBOL;
    }
    else
    {
    	point_to_save_name[point_name_current_symbol]++;
    }

    draw_current_menu();
}



void save_device_as_down(void)
{
    if (point_to_save_name[point_name_current_symbol] == POINT_NAME_FIRST_SYMBOL)
    {
    	point_to_save_name[point_name_current_symbol] = POINT_NAME_LAST_SYMBOL;
    }
    else
    {
    	point_to_save_name[point_name_current_symbol]--;
    }

    draw_current_menu();
}



void save_device_as_ok(void)
{
    if (++point_name_current_symbol == MEMORY_POINT_NAME_LENGTH)
    {
    	point_name_current_symbol = 0;
    }

    draw_current_menu();
}



void save_device_as_ok_long(void)
{
	save_memory_point(current_each_device, &point_to_save_name[0], current_slot_to_save);

	current_each_device = device_number;
	current_slot_to_save = 0;
	point_name_current_symbol = 0;

	current_menu = M_SAVED_POPUP;
	draw_current_menu();
}



void save_device_as_esc(void)
{
	point_name_current_symbol = 0;
	current_menu = M_SAVE_DEVICE;
	draw_current_menu();
}



void saved_popup_esc(void)
{
	current_each_device = device_number;
	current_slot_to_save = 0;
	point_name_current_symbol = 0;

	current_menu = M_DEVICES;
	draw_current_menu();
}



void radar_up(void)
{
	if (current_radar_device != 0)
	{
		do
		{
			do
			{
				if (current_radar_device == DEVICE_NUMBER_FIRST)
				{
					current_radar_device = DEVICE_NUMBER_LAST;
				}
				else
				{
					current_radar_device--;
				}
			}
			while (pp_dev_aux[current_radar_device]->exist_flag == 0);
		}
		while (current_radar_device == device_number);

		draw_current_menu();
	}
}



void radar_down(void)
{
	if (current_radar_device != 0)
	{
		do
		{
			do
			{
				if (current_radar_device == DEVICE_NUMBER_LAST)
				{
					current_radar_device = DEVICE_NUMBER_FIRST;
				}
				else
				{
					current_radar_device++;
				}
			}
			while (pp_dev_aux[current_radar_device]->exist_flag == 0);
		}
		while (current_radar_device == device_number);

		draw_current_menu();
	}
}



void radar_ok(void)
{
	if (current_radar_device != 0)
	{
		radar_list_hide[current_radar_device] ^= 1;		//Invert hide flag

		draw_current_menu();
	}
}



void points_up(void)
{
	if (current_slot_to_load == MEMORY_SLOT_FIRST)
    {
		current_slot_to_load = MEMORY_SLOT_LAST;
    }
    else
    {
    	current_slot_to_load--;
    }
    
    draw_current_menu();
}



void points_down(void)
{
	if (current_slot_to_load == MEMORY_SLOT_LAST)
    {
		current_slot_to_load = MEMORY_SLOT_FIRST;
    }
    else
    {
    	current_slot_to_load++;
    }

    draw_current_menu();
}



void points_ok(void)
{
    if (pp_memory_slot[current_slot_to_load]->exist_flag == 1)
    {
		current_menu = M_EACH_POINT;
		draw_current_menu();
    }
}



void points_esc(void)
{
	current_slot_to_load = MEMORY_SLOT_FIRST;
	current_menu = M_MAIN;
	draw_current_menu();
}



void load_point_up(void)
{
	if (current_device_to_load != 0)
	{
		do
		{
			do
			{
				if (current_device_to_load == DEVICE_NUMBER_FIRST)
				{
					current_device_to_load = DEVICE_NUMBER_LAST;
				}
				else
				{
					current_device_to_load--;
				}
			}
			while (pp_dev_aux[current_device_to_load]->exist_flag == 1);
		}
		while (current_device_to_load == device_number);

		draw_current_menu();
	}
}



void load_point_down(void)
{
	if (current_device_to_load != 0)
	{
		do
		{
			do
			{
				if (current_device_to_load == DEVICE_NUMBER_LAST)
				{
					current_device_to_load = DEVICE_NUMBER_FIRST;
				}
				else
				{
					current_device_to_load++;
				}
			}
			while (pp_dev_aux[current_device_to_load]->exist_flag == 1);
		}
		while (current_device_to_load == device_number);

		draw_current_menu();
	}
}



void load_point_ok(void)
{
	if (current_device_to_load != 0)
	{
		load_memory_point(current_device_to_load, current_slot_to_load);

		current_slot_to_load = MEMORY_SLOT_FIRST;
		current_device_to_load = 0;
		current_menu = M_MAIN;
		draw_current_menu();
	}
}



void load_point_esc(void)
{
	current_device_to_load = 0;
	current_menu = M_EACH_POINT;
	draw_current_menu();
}



void delete_point_ok(void)
{
	delete_memory_point(current_slot_to_load);

	reset_current_item_in_menu(M_EACH_POINT);
	current_slot_to_load = MEMORY_SLOT_FIRST;
	current_menu = M_POINTS;
	draw_current_menu();
}



void set_dev_num_up(void)
{
    if (settings_copy.device_number == DEVICE_NUMBER_LAST)
    {
        settings_copy.device_number = DEVICE_NUMBER_FIRST;
    }
    else
    {
        settings_copy.device_number++;
    }

    draw_current_menu();
}



void set_dev_num_down(void)
{
    if (settings_copy.device_number == DEVICE_NUMBER_FIRST)
    {
        settings_copy.device_number = DEVICE_NUMBER_LAST;
    }
    else
    {
        settings_copy.device_number--;
    }
    
    draw_current_menu();
}



void set_dev_num_ok(void)
{
    if (settings_copy.device_number != device_number)
    {
        flag_settings_changed = 1;
    }
    
    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void set_dev_num_esc(void)
{
    settings_copy.device_number = device_number;   //exit no save, reset value
    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void confirm_settings_save_ok(void)
{
    settings_save(&settings_copy);
    NVIC_SystemReset();
}



void set_dev_id_up(void)
{
    if (settings_copy.device_id[device_id_current_symbol] == DEVICE_ID_LAST_SYMBOL)
    {
        settings_copy.device_id[device_id_current_symbol] = DEVICE_ID_FIRST_SYMBOL;
    }
    else
    {
        settings_copy.device_id[device_id_current_symbol]++;
    }
    
    draw_current_menu();
}



void set_dev_id_down(void)
{
    if (settings_copy.device_id[device_id_current_symbol] == DEVICE_ID_FIRST_SYMBOL)
    {
        settings_copy.device_id[device_id_current_symbol] = DEVICE_ID_LAST_SYMBOL;
    }
    else
    {
        settings_copy.device_id[device_id_current_symbol]--;
    }
    
    draw_current_menu();
}



void set_dev_id_ok(void)
{
    if (++device_id_current_symbol == DEVICE_ID_LEN)
    {
        device_id_current_symbol = 0;
    }
    
    draw_current_menu();
}



void set_dev_id_ok_long(void)
{
    for (uint8_t i = 0; i < DEVICE_ID_LEN; i++)
    {
        if (settings_copy.device_id[i] != p_settings->device_id[i])
        {
            flag_settings_changed = 1;
            break;
        }
    }
    
    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void set_dev_id_esc(void)
{
    for (uint8_t i = 0; i < DEVICE_ID_LEN; i++)
    {
        settings_copy.device_id[i] = p_settings->device_id[i];   //exit no save, reset value
    }
    device_id_current_symbol = 0;
    
    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void set_freq_ch_up(void)
{
    if (settings_copy.freq_channel == FREQ_CHANNEL_LAST)
    {
        settings_copy.freq_channel = FREQ_CHANNEL_FIRST;
    }
    else
    {
        settings_copy.freq_channel++;
    }
    
    draw_current_menu();
}



void set_freq_ch_down(void)
{
    if (settings_copy.freq_channel == FREQ_CHANNEL_FIRST)
    {
        settings_copy.freq_channel = FREQ_CHANNEL_LAST;
    }
    else
    {
        settings_copy.freq_channel--;
    }
    
    draw_current_menu();
}



void set_freq_ch_ok(void)
{
    if (settings_copy.freq_channel != p_settings->freq_channel)
    {
        flag_settings_changed = 1;
    }
    
    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void set_freq_ch_esc(void)
{
    settings_copy.freq_channel = p_settings->freq_channel;   //exit no save, reset value
    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void set_tx_pow_up(void)
{
    if (settings_copy.tx_power_opt == TX_POWER_LAST_OPTION)
    {
        settings_copy.tx_power_opt = TX_POWER_FIRST_OPTION;
    }
    else
    {
        settings_copy.tx_power_opt++;
    }
    
    draw_current_menu();
}



void set_tx_pow_down(void)
{
    if (settings_copy.tx_power_opt == TX_POWER_FIRST_OPTION)
    {
        settings_copy.tx_power_opt = TX_POWER_LAST_OPTION;
    }
    else
    {
        settings_copy.tx_power_opt--;
    }
    
    draw_current_menu();
}



void set_tx_pow_ok(void)
{
    if (settings_copy.tx_power_opt != p_settings->tx_power_opt)
    {
        flag_settings_changed = 1;
    }
    
    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void set_tx_pow_esc(void)
{
    settings_copy.tx_power_opt = p_settings->tx_power_opt;   //exit no save, reset value
    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void set_send_intvl_up(void)
{
    if (settings_copy.send_interval_opt == SEND_INTERVAL_LAST_OPTION)
    {
    	settings_copy.send_interval_opt = SEND_INTERVAL_FIRST_OPTION;
    }
    else
    {
    	settings_copy.send_interval_opt++;
    }

    draw_current_menu();
}



void set_send_intvl_down(void)
{
    if (settings_copy.send_interval_opt == SEND_INTERVAL_FIRST_OPTION)
    {
    	settings_copy.send_interval_opt = SEND_INTERVAL_LAST_OPTION;
    }
    else
    {
    	settings_copy.send_interval_opt--;
    }

    draw_current_menu();
}



void set_send_intvl_ok(void)
{
    if (settings_copy.send_interval_opt != p_settings->send_interval_opt)
    {
        flag_settings_changed = 1;
    }

    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void set_send_intvl_esc(void)
{
    settings_copy.send_interval_opt = p_settings->send_interval_opt;   //exit no save, reset value
    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void set_to_thr_up(void)
{
	#define TIMEOUT_THRESHOLD_MAX 	(3600)
	#define TIMEOUT_THRESHOLD_STEP 	(10)

	if (settings_copy.timeout_threshold.as_integer < TIMEOUT_THRESHOLD_MAX)
	{
		settings_copy.timeout_threshold.as_integer += TIMEOUT_THRESHOLD_STEP;
	}

    draw_current_menu();
}



void set_to_thr_down(void)
{
	#define TIMEOUT_THRESHOLD_MIN 	(0)

	if (settings_copy.timeout_threshold.as_integer > TIMEOUT_THRESHOLD_MIN)
	{
		settings_copy.timeout_threshold.as_integer -= TIMEOUT_THRESHOLD_STEP;
	}

    draw_current_menu();
}



void set_to_thr_ok(void)
{
    if (settings_copy.timeout_threshold.as_integer != p_settings->timeout_threshold.as_integer)
    {
        flag_settings_changed = 1;
    }

    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void set_to_thr_esc(void)
{
	settings_copy.timeout_threshold.as_integer = p_settings->timeout_threshold.as_integer;   //exit no save, reset value
    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void set_fnc_thr_up(void)
{
	#define FENCE_THRESHOLD_MAX 	(3600)
	#define FENCE_THRESHOLD_STEP 	(10)

	if (settings_copy.fence_threshold.as_integer < FENCE_THRESHOLD_MAX)
	{
		settings_copy.fence_threshold.as_integer += FENCE_THRESHOLD_STEP;
	}

    draw_current_menu();
}



void set_fnc_thr_down(void)
{
	#define FENCE_THRESHOLD_MIN 	(0)

	if (settings_copy.fence_threshold.as_integer > FENCE_THRESHOLD_MIN)
	{
		settings_copy.fence_threshold.as_integer -= FENCE_THRESHOLD_STEP;
	}

    draw_current_menu();
}



void set_fnc_thr_ok(void)
{
    if (settings_copy.fence_threshold.as_integer != p_settings->fence_threshold.as_integer)
    {
        flag_settings_changed = 1;
    }

    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void set_fnc_thr_esc(void)
{
	settings_copy.fence_threshold.as_integer = p_settings->fence_threshold.as_integer;   //exit no save, reset value
    current_menu = M_EDIT_SETTINGS;
    draw_current_menu();
}



void confirm_settings_save_esc(void)
{
    settings_copy = *p_settings;   //reset to no changes state
    flag_settings_changed = 0;  //clear flag
    current_menu = M_SETTINGS;
    draw_current_menu();
}



void restore_defaults_ok(void)
{
    settings_save_default();
    NVIC_SystemReset();
}



void erase_all_ok(void)
{
    ssd1306_clear();
    ssd1306_print(0, 1, "WAIT...", 0);
    ssd1306_update();
    m24c64_erase_all();
    NVIC_SystemReset();
}
