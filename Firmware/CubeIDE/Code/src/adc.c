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
#include "bit_band.h"



void adc_start_bat_voltage_reading(void);
void adc_clock_disable(void);
void adc_clock_enable(void);



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
    delay_cyc(10000);
    ADC1->CR2 |= ADC_CR2_CAL;           //start cal
    while (ADC1->CR2 & ADC_CR2_CAL);    //wait

    //Interrupt at the end of the conversion
    ADC1->CR1 |= ADC_CR1_EOCIE;
    NVIC_EnableIRQ(ADC1_2_IRQn);

    adc_clock_disable();
}



void adc_clock_disable(void)
{
	BIT_BAND_PERI(RCC->APB2ENR, RCC_APB2ENR_ADC1EN) = 0;
}



void adc_clock_enable(void)
{
	BIT_BAND_PERI(RCC->APB2ENR, RCC_APB2ENR_ADC1EN) = 1;
}



//Start ADC reading
void adc_start_bat_voltage_reading(void)
{
	adc_clock_enable();
	delay_cyc(10);

	//Start conversion
	ADC1->CR2 |= ADC_CR2_SWSTART;
}



//Read the ADC conversion result; return 1 if battery low is detected
uint8_t adc_read_bat_voltage_result(void)
{
	//Convert
	bat_voltage = 2 * ((ADC1->DR * vref) / 4096);     //x2 due to resistive voltage divider before ADC input

	adc_clock_disable();

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

    return 0;
}



//Check battery voltage with predefined interval
void adc_check_bat_voltage(void)
{
	bat_interval_counter++;

    if (bat_interval_counter >= GET_BAT_VOLTAGE_INTERVAL)
    {
    	bat_interval_counter = 0;

    	adc_start_bat_voltage_reading();
    }
}



float get_bat_voltage_value(void)
{
	return bat_voltage;
}
