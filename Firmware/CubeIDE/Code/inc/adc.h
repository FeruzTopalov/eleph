/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: adc.h
*/

void adc_init(void);
uint8_t adc_get_bat_voltage(void);
float get_bat_voltage(void);
