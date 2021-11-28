/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: service.c
*/

#include <string.h>
#include "stm32f10x.h"
#include "service.h"
#include "ssd1306.h"



void debug_message(char *message)
{
	ssd1306_clear();
	ssd1306_print(0, 0, message, 0);
	ssd1306_update();
	delay_cyc(200000);
}



//Convert timeout in seconds to timeout in "XXdXXhXXmXXs"
void convert_timeout(uint32_t timeout_val, char *buffer)
{
    uint32_t sec = 0;
    uint32_t min = 0;
    uint32_t hour = 0;
    uint32_t day = 0;
    char buf[3];
    
    
    if (timeout_val >= 60)
    {
        min = timeout_val / 60;
        sec = timeout_val % 60;

        if (min >= 60)
        {
            hour = min / 60;
            min = min % 60;

            if (hour >= 24)
            {
                day = hour / 24;
                hour = hour % 24;
            }
        }
    }
    else
    {
        sec = timeout_val;
    }
    
    if (day)
    {
        //XXdXXh
        itoa32(day, &buf[0]);
        if (day > 9)
        {
            buffer[0] = buf[0];
            buffer[1] = buf[1];
            buffer[2] = 'd';
        }
        else
        {
            buffer[0] = '0';
            buffer[1] = buf[0];
            buffer[2] = 'd';
        }
        
        itoa32(hour, &buf[0]);
        if (hour > 9)
        {
            buffer[3] = buf[0];
            buffer[4] = buf[1];
            buffer[5] = 'h';
        }
        else
        {
            buffer[3] = '0';
            buffer[4] = buf[0];
            buffer[5] = 'h';
        }
    }
    else if (hour)
    {
        //XXhXXm
        itoa32(hour, &buf[0]);
        if (hour > 9)
        {
            buffer[0] = buf[0];
            buffer[1] = buf[1];
            buffer[2] = 'h';
        }
        else
        {
            buffer[0] = '0';
            buffer[1] = buf[0];
            buffer[2] = 'h';
        }
        
        itoa32(min, &buf[0]);
        if (min > 9)
        {
            buffer[3] = buf[0];
            buffer[4] = buf[1];
            buffer[5] = 'm';
        }
        else
        {
            buffer[3] = '0';
            buffer[4] = buf[0];
            buffer[5] = 'm';
        }
    }
    else
    {
        //XXmXXs
        itoa32(min, &buf[0]);
        if (min > 9)
        {
            buffer[0] = buf[0];
            buffer[1] = buf[1];
            buffer[2] = 'm';
        }
        else
        {
            buffer[0] = '0';
            buffer[1] = buf[0];
            buffer[2] = 'm';
        }
        
        itoa32(sec, &buf[0]);
        if (sec > 9)
        {
            buffer[3] = buf[0];
            buffer[4] = buf[1];
            buffer[5] = 's';
        }
        else
        {
            buffer[3] = '0';
            buffer[4] = buf[0];
            buffer[5] = 's';
        }
    }
    
    buffer[6] = 0;  //string end
}



//Simple delay in cycles
void delay_cyc(uint32_t cycles)
{
    while (cycles)
    {
        cycles--;
    }
}



//Copy string
void copy_string(char *from, char *to)
{
    while (*from)
    {
        *to++ = *from++;
    }
    *to = 0;    //end terminator
}



//Converts string to float
float atof32(char *input)
{
    uint8_t i = 0;
    int32_t sign = 1;
    float power = 1.0;
    float result = 0.0;
    
    if(input[0] == 0)
    {
        return 0.0;
    }
    
    if(input[i] == '-')
    {
        sign = -1;
        i++;
    }
    
    while(input[i] != '.')
    {
        result = result * 10.0 + (input[i] - '0');
        i++;
    }
    
    i++;
    
    while(input[i] != 0)
    {
        result = result * 10.0 + (input[i] - '0');
        power *= 10.0;
        i++;
    }
    
    return (sign * result / power);
}



//Converts float to string
void ftoa32(float value, uint8_t precision, char *buffer)
{
    uint8_t i = 0;
    uint32_t mod = 0;
    float pow = 1.0;
    char sgn = 0;
    float value_copy;
    
    if((value == 0.0) || (value == -0.0))
    {
        buffer[0] = '0';
        buffer[1] = 0;
        return;
    }
    
    if(value < 0)
    {
        sgn = '-';
        value *= -1.0;
    }
    
    value_copy = value;
    
    for(uint8_t p = 0; p < precision; p++)
    {
        pow = pow * 10.0;
    }
    
    value = value * pow;
    uint32_t ipart = value;
    
    buffer[i++] = 0;
    
    do
    {
        mod = ipart % 10;
        ipart /= 10;
        buffer[i++] = mod + '0';
    }
    while(ipart > 0);
    
    if (value_copy < 1.0)
    {
        for(uint8_t p = 0; p < precision; p++)
        {
            buffer[i] = '0';
            i++;
        }
    }
    
    if(sgn == '-')
    {
        buffer[i] = sgn;
    }
    else
    {
        i--;
    }
    
    for(uint8_t n = i + 1; n > precision + 1; n--)
    {
        buffer[n] = buffer[n - 1];
    }
    buffer[precision + 1] = '.';
    i++;
    
    char c;
    for(uint8_t j = 0; j < i; j++, i--)
    {
        c = buffer[j];
        buffer[j] = buffer[i];
        buffer[i] = c;
    }
}



//Converts string to integer
int32_t atoi32(char *input)
{
    uint8_t i = 0;
    int32_t sign = 1;
    int32_t result = 0;
    
    if(input[0] == 0)
    {
        return 0;
    }
    
    if((input[0] == '0') && (input[1] == 0))
    {
        return 0;
    }
    
    if(input[i] == '-')
    {
        sign = -1;
        i++;
    }
    
    while(input[i] != 0)
    {
        result = result * 10 + (input[i] - '0');
        i++;
    }
    
    return result * sign;
}



//Converts integer to string
void itoa32(int32_t value, char *buffer)
{
    uint8_t i = 0;
    uint8_t mod = 0;
    char sgn = 0;
    
    if(value == 0)
    {
        buffer[0] = '0';
        buffer[1] = 0;
        return;
    }
    
    if(value < 0)
    {
        sgn = '-';
        value *= -1;
    }
    
    buffer[i++] = 0;
    
    while(value > 0)
    {
        mod = value % 10;
        value /= 10;
        buffer[i++] = mod + '0';
    }
    
    if(sgn == '-')
    {
        buffer[i] = sgn;
    }
    else
    {
        i--;
    }
    
    char c;
    for(uint8_t j = 0; j < i; j++, i--)
    {
        c = buffer[j];
        buffer[j] = buffer[i];
        buffer[i] = c;
    }
}
