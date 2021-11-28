/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: service.h
*/



#ifndef SERVICE_HEADER
#define SERVICE_HEADER



void debug_message(char *message);
void convert_timeout(uint32_t timeout_val, char *buffer);

void delay_cyc(uint32_t cycles);
void copy_string(char *from, char *to);

float atof32(char *input);
void ftoa32(float value, uint8_t precision, char *buffer);
int32_t atoi32(char *input);
void itoa32(int32_t value, char *buffer);



#endif /*SERVICE_HEADER*/
