/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: adc.h
*/



#ifndef ADC_HEADER
#define ADC_HEADER



void adc_init(void);
void adc_check_bat_voltage(void);
void adc_start_bat_voltage_reading(void);
uint8_t adc_read_bat_voltage_result(void);
float get_bat_voltage_value(void);



#endif /*ADC_HEADER*/
