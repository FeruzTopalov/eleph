/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: settings.h
*/

#define DEVICE_ID_LEN       		(2)         //length of device ID, two ASCII characters
#define TIMEOUT_ALARM_DISABLED		(0)
#define FENCE_ALARM_DISABLED		(0)



//Send interval settings
#define SEND_INTERVAL_1S_SETTING		(0)
#define SEND_INTERVAL_5S_SETTING		(1)
#define SEND_INTERVAL_10S_SETTING		(2)
#define SEND_INTERVAL_30S_SETTING		(3)
#define SEND_INTERVAL_60S_SETTING		(4)



//POWER SETTINGS
#define TX_POWER_10MILLIW_SETTING   (0)
#define TX_POWER_25MILLIW_SETTING   (1)
#define TX_POWER_40MILLIW_SETTING   (2)
#define TX_POWER_100MILLIW_SETTING  (3)



//Structure with settings
struct settings_struct
{
    uint8_t device_number;              //this device number in group, 1...DEVICES_IN_GROUP

    char device_id[DEVICE_ID_LEN];      //user id, ASCII symbols

    uint8_t freq_channel;               //frequency tx/rx channel

    uint8_t tx_power_opt;               //tx power option, not an actual value

    uint8_t send_interval_opt;			//send interval option, not an actual value

    union
    {
        uint16_t as_integer;        	//timeout treshold in seconds, unsigned. if it == 0, then timeout alarm not trigger (but, anyway, timeout is counting). See TIMEOUT_ALARM_DISABLED
        uint8_t as_array[2];
    } timeout_threshold;

    union
    {
        uint16_t as_integer;        	//fence treshold in meters, unsigned. if it == 0, then fence alarm not trigger. See FENCE_ALARM_DISABLED
        uint8_t as_array[2];
    } fence_threshold;

};



struct settings_struct *get_settings(void);
uint8_t *get_send_interval_values(void);
uint8_t *get_tx_power_values(void);
void settings_save_default(void);
void settings_load(void);
void settings_save(struct settings_struct *settings);
