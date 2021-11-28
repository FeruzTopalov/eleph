/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: m24c64.c
*/

#include "stm32f10x.h"
#include "main.h"
#include "settings.h"
#include "m24c64.h"
#include "rfm98.h"
#include "points.h"
#include "lrns.h"


#define SEND_INTERVAL_1S_VALUE		(1)
#define SEND_INTERVAL_5S_VALUE		(5)
#define SEND_INTERVAL_10S_VALUE		(10)
#define SEND_INTERVAL_30S_VALUE		(30)
#define SEND_INTERVAL_60S_VALUE		(60)

#define SEND_INTERVAL_VALUES_ARRAY 	{ 	SEND_INTERVAL_1S_VALUE, 	\
										SEND_INTERVAL_5S_VALUE, 	\
										SEND_INTERVAL_10S_VALUE, 	\
										SEND_INTERVAL_30S_VALUE, 	\
										SEND_INTERVAL_60S_VALUE	}



#define TX_POWER_1MILLIW_VALUE   	(1)
#define TX_POWER_10MILLIW_VALUE   	(10)
#define TX_POWER_50MILLIW_VALUE   	(50)
#define TX_POWER_100MILLIW_VALUE  	(100)

#define TX_POWER_VALUES_ARRAY 		{ 	TX_POWER_1MILLIW_VALUE, 	\
										TX_POWER_10MILLIW_VALUE, 	\
										TX_POWER_50MILLIW_VALUE, 	\
										TX_POWER_100MILLIW_VALUE	}



#define EEPROM_SETTINGS_PAGE_ADDRESS    (0)


//positions:
#define SETTINGS_INIT_FLAG_POS          	(0)
#define SETTINGS_DEVICE_NUMBER_POS      	(1)
#define SETTINGS_DEVICES_ON_AIR_POS			(2)
#define SETTINGS_DEVICE_ID_POS          	(3)
#define SETTINGS_FREQ_CHANNEL_POS       	(5)
#define SETTINGS_TX_POWER_POS           	(6)
#define SETTINGS_SEND_INTERVAL_POS        	(7)
#define SETTINGS_TIMEOUT_THRESHOLD_POS   	(8)
#define SETTINGS_FENCE_THRESHOLD_POS   		(10)

//default values:
#define SETTINGS_INIT_FLAG_DEFAULT      	(0xAA)
#define SETTINGS_DEVICE_NUMBER_DEFAULT  	(1)
#define SETTINGS_DEVICES_ON_AIR_DEFAULT		(DEVICES_IN_GROUP)
#define SETTINGS_DEVICE_ID_0_DEFAULT    	('I')
#define SETTINGS_DEVICE_ID_1_DEFAULT    	('D')
#define SETTINGS_FREQ_CHANNEL_DEFAULT   	(1)             //base freq is 433.050 and freq step is 25kHz, so CH0 - 433.050 (not valid, not used); CH1 - 433.075 (first LPD channel)
#define SETTINGS_TX_POWER_DEFAULT       	(TX_POWER_10MILLIW_SETTING)
#define SETTINGS_SEND_INTERVAL_DEFAULT     	(SEND_INTERVAL_1S_SETTING)
#define SETTINGS_TIMEOUT_THRESHOLD_DEFAULT  (60)
#define SETTINGS_FENCE_THRESHOLD_DEFAULT  	(100)



uint8_t settings_array[M24C64_PAGE_SIZE];
struct settings_struct settings;
uint8_t send_interval_values[] = SEND_INTERVAL_VALUES_ARRAY;
uint8_t tx_power_values[] = TX_POWER_VALUES_ARRAY;


uint8_t *get_send_interval_values(void)
{
	return &send_interval_values[0];
}



uint8_t *get_tx_power_values(void)
{
	return &tx_power_values[0];
}



struct settings_struct *get_settings(void)
{
	return &settings;
}



void settings_load(void)
{
    uint16_t init_flag_addr = EEPROM_SETTINGS_PAGE_ADDRESS * M24C64_PAGE_SIZE + SETTINGS_INIT_FLAG_POS;
    
    if (m24c64_read_byte(init_flag_addr) != SETTINGS_INIT_FLAG_DEFAULT)     //if first power-up or EEPROM had been erased
    {
        settings_save_default();
    }
    
    //read from EEPROM
    m24c64_read_page(&settings_array[0], EEPROM_SETTINGS_PAGE_ADDRESS);
    
    //load settings to struct
    settings.device_number = 					settings_array[SETTINGS_DEVICE_NUMBER_POS];
    settings.devices_on_air = 					settings_array[SETTINGS_DEVICES_ON_AIR_POS];
    settings.device_id[0] = 					settings_array[SETTINGS_DEVICE_ID_POS];
    settings.device_id[1] = 					settings_array[SETTINGS_DEVICE_ID_POS + 1];
    settings.freq_channel = 					settings_array[SETTINGS_FREQ_CHANNEL_POS];
    settings.tx_power_opt = 					settings_array[SETTINGS_TX_POWER_POS];
    settings.send_interval_opt = 				settings_array[SETTINGS_SEND_INTERVAL_POS];
    settings.timeout_threshold.as_array[0] = 	settings_array[SETTINGS_TIMEOUT_THRESHOLD_POS];
    settings.timeout_threshold.as_array[1] = 	settings_array[SETTINGS_TIMEOUT_THRESHOLD_POS + 1];
    settings.fence_threshold.as_array[0] = 		settings_array[SETTINGS_FENCE_THRESHOLD_POS];
    settings.fence_threshold.as_array[1] = 		settings_array[SETTINGS_FENCE_THRESHOLD_POS + 1];
}



void settings_save_default(void)
{
    for (uint8_t i = 0; i < M24C64_PAGE_SIZE; i++)
    {
        settings_array[i] = M24C64_EMPTY_CELL_VALUE;    //clear array
    }
    
    //assign default values
    settings_array[SETTINGS_INIT_FLAG_POS] = 			SETTINGS_INIT_FLAG_DEFAULT;
    settings_array[SETTINGS_DEVICE_NUMBER_POS] = 		SETTINGS_DEVICE_NUMBER_DEFAULT;
    settings_array[SETTINGS_DEVICES_ON_AIR_POS] = 		SETTINGS_DEVICES_ON_AIR_DEFAULT;
    settings_array[SETTINGS_DEVICE_ID_POS] = 			SETTINGS_DEVICE_ID_0_DEFAULT;
    settings_array[SETTINGS_DEVICE_ID_POS + 1] = 		SETTINGS_DEVICE_ID_1_DEFAULT;
    settings_array[SETTINGS_FREQ_CHANNEL_POS] = 		SETTINGS_FREQ_CHANNEL_DEFAULT;
    settings_array[SETTINGS_TX_POWER_POS] = 			SETTINGS_TX_POWER_DEFAULT;
    settings_array[SETTINGS_SEND_INTERVAL_POS] = 		SETTINGS_SEND_INTERVAL_DEFAULT;
    settings_array[SETTINGS_TIMEOUT_THRESHOLD_POS] = 	(uint8_t)((uint8_t)0xFF & (uint16_t)SETTINGS_TIMEOUT_THRESHOLD_DEFAULT);				//note: little-endian assumed
    settings_array[SETTINGS_TIMEOUT_THRESHOLD_POS + 1]= (uint8_t)((uint8_t)0xFF & ((uint16_t)SETTINGS_TIMEOUT_THRESHOLD_DEFAULT >> 8));
    settings_array[SETTINGS_FENCE_THRESHOLD_POS] = 		(uint8_t)((uint8_t)0xFF & (uint16_t)SETTINGS_FENCE_THRESHOLD_DEFAULT);				//note: little-endian assumed
    settings_array[SETTINGS_FENCE_THRESHOLD_POS + 1] = 	(uint8_t)((uint8_t)0xFF & ((uint16_t)SETTINGS_FENCE_THRESHOLD_DEFAULT >> 8));
    
    //write to EEPROM
    m24c64_write_page(&settings_array[0], EEPROM_SETTINGS_PAGE_ADDRESS);
}



void settings_save(struct settings_struct *p_settings)
{
    for (uint8_t i = 0; i < M24C64_PAGE_SIZE; i++)
    {
        settings_array[i] = M24C64_EMPTY_CELL_VALUE;    //clear array
    }
    
    //assign values
    settings_array[SETTINGS_INIT_FLAG_POS] = 			SETTINGS_INIT_FLAG_DEFAULT;
    settings_array[SETTINGS_DEVICE_NUMBER_POS] = 		p_settings->device_number;
    settings_array[SETTINGS_DEVICES_ON_AIR_POS] =		p_settings->devices_on_air;
    settings_array[SETTINGS_DEVICE_ID_POS] = 			p_settings->device_id[0];
    settings_array[SETTINGS_DEVICE_ID_POS + 1] = 		p_settings->device_id[1];
    settings_array[SETTINGS_FREQ_CHANNEL_POS] = 		p_settings->freq_channel;
    settings_array[SETTINGS_TX_POWER_POS] = 			p_settings->tx_power_opt;
    settings_array[SETTINGS_SEND_INTERVAL_POS] = 		p_settings->send_interval_opt;
    settings_array[SETTINGS_TIMEOUT_THRESHOLD_POS] = 	p_settings->timeout_threshold.as_array[0];
    settings_array[SETTINGS_TIMEOUT_THRESHOLD_POS + 1]= p_settings->timeout_threshold.as_array[1];
    settings_array[SETTINGS_FENCE_THRESHOLD_POS] = 		p_settings->fence_threshold.as_array[0];
    settings_array[SETTINGS_FENCE_THRESHOLD_POS + 1] = 	p_settings->fence_threshold.as_array[1];
    
    //write to EEPROM
    m24c64_write_page(&settings_array[0], EEPROM_SETTINGS_PAGE_ADDRESS);
}
