/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: main.c
*/



#include <string.h>
#include "stm32f10x.h"
#include "main.h"
#include "gpio.h"
#include "spi.h"
#include "ssd1306.h"
#include "service.h"
#include "uart.h"
#include "gps.h"
#include "rfm98.h"
#include "timer.h"
#include "i2c.h"
#include "m24c64.h"
#include "settings.h"
#include "buttons.h"
#include "menu.h"
#include "adc.h"
#include "points.h"
#include "lrns.h"
#include "ssd1306_bitmaps.h"
#include "rfm98_config.h"



void setup_interrupt_priorities(void);



//TIMERS
uint32_t uptime = 0;
uint32_t pps_counter = 0;
uint8_t overflow_counter = 0;
uint8_t time_slot = 0;



//MAIN
struct main_flags_struct main_flags = {0};
struct settings_struct *p_settings;
struct gps_num_struct *p_gps_num;
uint8_t *p_send_interval_values;
uint8_t current_radio_status = 0;

char buff[21];

//PROGRAM
int main(void)
{
    gpio_init();
    hold_power();
    timers_init();
    spi1_init();
    spi2_init();
    i2c_init();
    uart_dma_init();
    settings_load();
    ssd1306_init();
    rfm98_init();
    ext_int_init();
    adc_init();
    adc_start_bat_voltage_reading();
    init_lrns();
    init_menu();
    init_points();

    p_settings = get_settings();
    p_gps_num = get_gps_num();
    p_send_interval_values = get_send_interval_values();

    ssd1306_bitmap(&startup_screen[0]);
    ssd1306_update();
    delay_cyc(200000);
    draw_current_menu();

    setup_interrupt_priorities();
    __enable_irq();
    make_a_beep();
    gps_init();


    while (1)
    {

    	//Scan Keys
    	if (main_flags.begin_scan_buttons == 1)
    	{
    		main_flags.begin_scan_buttons = 0;
    		change_menu(scan_buttons());
    	}



        //Parse GPS after PPS interrupt or UART DMA overflow
        if (main_flags.nmea_ready == 1)
        {
            main_flags.nmea_ready = 0;

            if (parse_gps() == 1)
            {
            	main_flags.do_screen_update = 1; //show results after successful parsing

				if (get_gps_status() == GPS_DATA_VALID)
				{
					main_flags.gps_valid = 1;

					if 	((main_flags.pps_exist == 1) &&
						((p_gps_num->second % p_send_interval_values[p_settings->send_interval_opt]) == 0)) //ready to txrx when pps exist and current second divides by send interval without remainder
					{
						gps_air_update_my_data(uptime);
						fill_air_packet_with_struct_data();
						main_flags.txrx_ready = 1;
					}
					else
					{
						main_flags.txrx_ready = 0;
					}
				}
				else
				{
					main_flags.gps_valid = 0;
				}
            }
        }





        //Process the radio interrupt
        if (main_flags.rx_packet_ready == 1)
        {
        	main_flags.rx_packet_ready = 0;

            current_radio_status = rfm98_get_irq_status();

            if (current_radio_status & RF_IRQFLAGS2_CRCOK)	//if CRC is ok
            {
            	rfm98_get_rx_packet();
            	fill_struct_with_air_packet_data(uptime);   //parse air data from another device (which has ended TX in the current time_slot)
            	led_green_on();
            }
            else if (current_radio_status & RF_IRQFLAGS2_PAYLOADREADY)	//if CRC is wrong but packet has been received
            {
            	rfm98_flush_fifo();
            }
        }



        //After each second
        if (main_flags.tick_1s == 1)
        {
        	main_flags.tick_1s = 0;
            adc_check_bat_voltage();
            calc_timeout(uptime);
        }


        
        //Checks after receiving packets from all devices or after no txrx; performing beep
        if (main_flags.time_slots_end == 1)
        {
        	main_flags.time_slots_end = 0;

        	process_all_devices();

        	uint8_t alarm_status = 0;
        	alarm_status += check_alarms();
        	alarm_status += check_timeout();	//check timeout flags and get the result only after the end of the TRX sequence
        	alarm_status += check_fence();

        	if (main_flags.battery_low == 1)	//when GPS is good and we are in the active state then check battery here
        	{
        		alarm_status += main_flags.battery_low;
        		main_flags.battery_low = 0;
        	}

        	if (alarm_status > 0)
        	{
        		main_flags.do_beep = 1;
        	}

        	main_flags.do_screen_update = 1;
        }
        else if ((main_flags.battery_low == 1) && (main_flags.pps_exist == 0)) 		//Check battery here when no GPS PPS (once in GET_BAT_VOLTAGE_INTERVAL)
        {
        	main_flags.battery_low = 0;
        	main_flags.do_beep = 1;
        }



        //Make some noise
        if (main_flags.do_beep == 1)
        {
        	main_flags.do_beep = 0;
        	make_a_beep();
        }



        //Finally update screen
        if (main_flags.do_screen_update == 1)
        {
        	main_flags.do_screen_update = 0;
        	draw_current_menu();
        }



    //Wait for interrupt
    __WFI();

    }
}



//DMA UART RX overflow
void DMA1_Channel5_IRQHandler(void)
{
    DMA1->IFCR = DMA_IFCR_CGIF5;     //clear all interrupt flags for DMA channel 5
    
    uart_dma_stop();
    backup_and_clear_uart_buffer();
    uart_dma_restart();

    if (main_flags.pps_exist == 1) 	//if last pps status was "sync" then make a beep because we lost PPS
    {
    	make_a_long_beep();			//make a long beep
    }

    pps_counter = 0;
    main_flags.pps_exist = 0;
    main_flags.nmea_ready = 1;
}



//GPS PPS interrupt
void EXTI15_10_IRQHandler(void)
{
	EXTI->PR = EXTI_PR_PR11;        //clear interrupt
	timer1_start();                 //the first thing to do is to start time slot timer right after PPS

	uart_dma_stop();				//fix the data
	backup_and_clear_uart_buffer();
	uart_dma_restart();

	pps_counter++;

	if (pps_counter > 2) //skip first two pps impulses: skip first PPS, ignore previous nmea data; skip second PPS, but fix the nmea data acquired after first PPS
	{
		main_flags.pps_exist = 1;
		main_flags.nmea_ready = 1;
	}
}



//RFM98 interrupt (PayloadReady or PacketSent)
void EXTI9_5_IRQHandler(void)
{
    EXTI->PR = EXTI_PR_PR8;         //clear interrupt

    if (main_flags.tx_state == 1)
    {
    	main_flags.tx_state = 0;
    	led_green_on();
    }
    else
    {
    	main_flags.rx_packet_ready = 1;
    }

}



//Time slot interrupt
void TIM1_UP_IRQHandler(void)
{
    TIM1->SR &= ~TIM_SR_UIF;                    //clear interrupt
    led_green_off();
    
    overflow_counter++;             			//increment ovf counter

    
    if (main_flags.txrx_ready == 1)
    {
		if (overflow_counter == (p_settings->devices_on_air + 1)) //end of the last time slot
		{
			timer1_stop_reload();
			overflow_counter = 0;
			main_flags.txrx_ready = 0;
			main_flags.time_slots_end = 1;
		}
		else
		{
			time_slot = overflow_counter;

			if (time_slot == p_settings->device_number)
			{
				if (rfm98_tx_packet())
				{
					main_flags.tx_state = 1;
				}
			}
			else
			{
				rfm98_start_rx();
			}
		}
    }
    else
    {
		timer1_stop_reload();
		overflow_counter = 0;

    	if (main_flags.gps_valid == 1)
    	{
    		main_flags.time_slots_end = 1; //if gps is valid then calculate rel pos for all devices
    	}
    	else
    	{
    		main_flags.time_slots_end = 0;
    	}
    }
}



//End of beep
void SysTick_Handler(void)
{
	timer3_stop();	//pwm
	timer3_clock_disable();
	systick_stop();	//gating
}



//Uptime counter (every 1 second)
void RTC_IRQHandler(void)
{
	RTC->CRL &= ~RTC_CRL_SECF;		//Clear interrupt

    uptime++;
    main_flags.tick_1s = 1;
}



//End of ADC conversion (battery voltage)
void ADC1_2_IRQHandler(void)
{
	main_flags.battery_low = adc_read_bat_voltage_result(); 			//EOC is cleared automatically after ADC_DR reading
}



//Scan buttons interval
void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF;        //clear gating timer int
	main_flags.begin_scan_buttons = 1;
}



void setup_interrupt_priorities(void)
{

#define GROUPS_8_SUBGROUPS_2	(4)

	NVIC_SetPriorityGrouping(GROUPS_8_SUBGROUPS_2);
	NVIC_SetPriority(EXTI15_10_IRQn, 	NVIC_EncodePriority(GROUPS_8_SUBGROUPS_2, 0, 0));		//PPS
	NVIC_SetPriority(TIM1_UP_IRQn, 		NVIC_EncodePriority(GROUPS_8_SUBGROUPS_2, 1, 0));		//Time slot
	NVIC_SetPriority(EXTI9_5_IRQn, 		NVIC_EncodePriority(GROUPS_8_SUBGROUPS_2, 2, 0));		//Radio
	NVIC_SetPriority(DMA1_Channel5_IRQn,NVIC_EncodePriority(GROUPS_8_SUBGROUPS_2, 3, 0));		//UART DMA
	NVIC_SetPriority(TIM2_IRQn, 		NVIC_EncodePriority(GROUPS_8_SUBGROUPS_2, 4, 0));		//Key scan
	NVIC_SetPriority(RTC_IRQn, 			NVIC_EncodePriority(GROUPS_8_SUBGROUPS_2, 5, 0));		//Uptime counter
	NVIC_SetPriority(ADC1_2_IRQn, 		NVIC_EncodePriority(GROUPS_8_SUBGROUPS_2, 6, 0));		//Battery voltage
	NVIC_SetPriority(SysTick_IRQn, 		NVIC_EncodePriority(GROUPS_8_SUBGROUPS_2, 7, 0));		//Beep end
}



uint32_t get_uptime(void)
{
	return uptime;
}



struct main_flags_struct *get_main_flags(void)
{
	return &main_flags;
}
