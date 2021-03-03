/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: adc.c
*/

#include "stm32f10x.h"
#include "adc.h"
#include "service.h"
#include "settings.h"
#include "points.h"
#include "lrns.h"
#include "gpio.h"



#define GET_BAT_VOLTAGE_INTERVAL    (10)

#define V_BATTERY_0_TO_10       	(3.0)
#define V_BATTERY_10_TO_25      	(3.1)
#define V_BATTERY_25_TO_50      	(3.3)
#define V_BATTERY_50_TO_75      	(3.6)
#define V_BATTERY_75_TO_100     	(3.9)



const float vref = 3.3;
uint8_t bat_interval_counter = 0;
float bat_voltage;



//ADC Init
void adc_init(void)
{
    //ADC prescaller
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;      //div by 8
    
    //ADC clock on
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    
    //Sample time
    ADC1->SMPR2 |= ADC_SMPR2_SMP0_1;    //13.5 cycles
    
    //Number of conversions in regular sequence
    ADC1->SQR1 &= ~ADC_SQR1_L;          //1 conversion
    
    //First channel in regular sequence
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;        //channel #0
    
    //Extermal trigger enable for regular sequence
    ADC1->CR2 |= ADC_CR2_EXTTRIG;
    
    //Event to start regular sequence
    ADC1->CR2 |= ADC_CR2_EXTSEL;        //start by software
    
    //ADC enable
    ADC1->CR2 |= ADC_CR2_ADON;
    
    //Calibration
    delay_cyc(100000);
    ADC1->CR2 |= ADC_CR2_CAL;           //start cal
    while (ADC1->CR2 & ADC_CR2_CAL);    //wait
}



//Get battery voltage
uint8_t adc_get_bat_voltage(void)
{
	bat_interval_counter++;

    if (bat_interval_counter >= GET_BAT_VOLTAGE_INTERVAL)
    {
    	bat_interval_counter = 0;

    	bat_mon_on();	//Enable resistive divider and wait a bit
    	delay_cyc(100);

		//Start conversation
		ADC1->CR2 |= ADC_CR2_SWSTART;

		//Wait for conversation end
		while (!(ADC1->SR & ADC_SR_EOC));

		bat_mon_off();	//Disable resistive divider

		//Convert
		bat_voltage = 2 * ((ADC1->DR * vref) / 4096);     //x2 due to resistive voltage divider before ADC input

		//Refresh flags
		if (bat_voltage > V_BATTERY_75_TO_100)
		{
			set_device_flags(FLAGS_BATTERY, FLAG_BATTERY_75_TO_100);
		}
		else if (bat_voltage > V_BATTERY_50_TO_75)
		{
			set_device_flags(FLAGS_BATTERY, FLAG_BATTERY_50_TO_75);
		}
		else if (bat_voltage > V_BATTERY_25_TO_50)
		{
			set_device_flags(FLAGS_BATTERY, FLAG_BATTERY_25_TO_50);
		}
		else if (bat_voltage > V_BATTERY_10_TO_25)
		{
			set_device_flags(FLAGS_BATTERY, FLAG_BATTERY_10_TO_25);
		}
		else
		{
			set_device_flags(FLAGS_BATTERY, FLAG_BATTERY_0_TO_10);
			return 1;
		}
    }

    return 0;
}



float get_bat_voltage(void)
{
	return bat_voltage;
}
