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
#include "si4463.h"
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



//TIMERS
uint32_t uptime = 0;
uint32_t pps_counter = 0;
uint8_t overflow_counter = 0;
uint8_t time_slot = 0;



//MAIN
struct main_flags_struct main_flags = {0, 0, 0, 0, 0, 0};
struct settings_struct *p_settings;
struct gps_num_struct *p_gps_num;
uint8_t *p_send_interval_values;



//PROGRAM
int main(void)
{
    gpio_init();
    timers_init();
    spi1_init();
    spi2_init();
    i2c_init();
    uart_dma_init();
    settings_load();
    ssd1306_init();
    si4463_init();
    ext_int_init();
    adc_init();
    adc_get_bat_voltage();
    init_lrns();
    init_menu();
    init_points();

    p_settings = get_settings();
    p_gps_num = get_gps_num();
    p_send_interval_values = get_send_interval_values();

    ssd1306_bitmap(&startup_screen[0]);
    ssd1306_update();
    delay_cyc(5000000);
    draw_current_menu();

    __enable_irq();
    make_a_beep();
    
    while (1)
    {
    	//Scan Keys
        change_menu(scan_buttons());
        
        
        //Parse GPS after PPS interrupt or UART DMA overflow
        if (main_flags.gps_ready == 1)
        {
            main_flags.gps_ready = 0;
            

            if (parse_gps() == 1)
            {
                if (main_flags.gps_sync == 1)
                {
                    if (get_gps_status() == GPS_DATA_VALID)
                    {
                        gps_air_update_my_data(uptime);
                        fill_air_packet_with_struct_data();    	//fill air data with coordinates of this device (this occur before first time slot interrupt)
                    }
                    else                            			//if PPS exist but data is invalid (rare situation)
                    {
                        timer1_stop_reload();       			//stop time slot timer due to nothing to transmitt
                    }
                }
                
                draw_current_menu();
            }
            else if (main_flags.gps_sync == 1)
            {
            	timer1_stop_reload();       					//stop time slot timer due to nothing to transmitt
            }
        }
        
        
        //Extract received packet
        if (main_flags.rx_ready == 1)
        {
            main_flags.rx_ready = 0;
            
            if (si4463_get_rx_packet())
            {
            	fill_struct_with_air_packet_data(uptime);   	//parse air data from another device (which has ended TX in the current time_slot)
            }
        }
        
        
        //Checks after receiving packets from all devices; performing beep
        if (main_flags.time_slots_end == 1)
        {
        	main_flags.time_slots_end = 0;

        	process_all_devices();								//calculate relative position for each active device

        	uint8_t any_alarm_status = 0;
        	any_alarm_status += check_alarms();
        	any_alarm_status += check_timeout();				//check timeout flags and get the result only after the end of the TRX sequence
        	any_alarm_status += check_fence();

        	if (any_alarm_status > 0)
        	{
        		make_a_beep();
        	}

        }
        else if ((main_flags.battery_low == 1) && (main_flags.gps_sync == 0))	//else check battery low flag
        {
        	make_a_beep();
        	main_flags.battery_low = 0;
        }

    }
}





//DMA UART RX overflow
void DMA1_Channel5_IRQHandler(void)
{
    DMA1->IFCR = DMA_IFCR_CGIF5;               	//clear all interrupt flags for DMA channel 5
    
    uart_dma_stop();
    backup_and_clear_uart_buffer();
    uart_dma_restart();
    
    main_flags.gps_ready = 1;
    main_flags.gps_sync = 0;   					//no pps signal
    pps_counter = 0;
    led_green_off();
}



//GPS PPS interrupt
void EXTI15_10_IRQHandler(void)
{
	EXTI->PR = EXTI_PR_PR11;        			//clear interrupt
	timer1_start();                 			//the first thing to do is start time slot timer right after PPS

	uart_dma_stop();							//fix the data
	backup_and_clear_uart_buffer();
	uart_dma_restart();

	pps_counter++;
	switch (pps_counter)
	{
		case 1:									//skip first PPS, ignore previous nmea data
			timer1_stop_reload();
			main_flags.gps_ready = 0;
			main_flags.gps_sync = 0;
			break;

		case 2:									//skip second PPS, but fix the nmea data acquired after first PPS
			timer1_stop_reload();
			main_flags.gps_ready = 1;
			main_flags.gps_sync = 0;
			break;

		default:								//at the moment, the nmea data, captured after first PPS, is parsed
			main_flags.gps_ready = 1;
			main_flags.gps_sync = 1;

			if ((p_gps_num->second % p_send_interval_values[p_settings->send_interval_opt]) == 0) //calc division remainder
			{
				main_flags.act_status = 1; 		//we are ready to show we are in act
			}
			else
			{
				timer1_stop_reload();
			}

			break;
	}
}



//SI4463 RX interrupt (RX valid or CRC error)
void EXTI9_5_IRQHandler(void)
{
    EXTI->PR = EXTI_PR_PR6;         //clear interrupt
    
    main_flags.rx_ready = 1;
}


/*
				  TIMINGS LEGEND (DEVICES_IN_GROUP = 6)
		+----+                                            +----+
		|    |                                            |    |
		|    |                                            |    |			PPS Signal
		|    |                                            |    |
+-------+    +--------------------------------------------+    +------+

		^    ^    ^    ^    ^    ^    ^    ^
		|    |    |    |    |    |    |    |
		|    |    |    |    |    |    |    |        				Time Slot #		Timer Interrupt #		Action
		|    |    |    |    |    |    |    |        				(time_slot)		(overflow_counter)
		|    |    |    |    |    |    |    |
		|    |    |    |    |    |    |    |
		|    |    |    |    |    |    |    |
		|    |    |    |    |    |    |    |
		|    |    |    |    |    |    |    |
		|    |    |    |    |    |    |    |
		|    |    |    |    |    |    |    +---------------------+					7						Stop Timer1, Set main_flags.time_slots_end
		|    |    |    |    |    |    |								6
		|    |    |    |    |    |    +--------------------------+					6
		|    |    |    |    |    |									5
		|    |    |    |    |    +-------------------------------+					5
		|    |    |    |    |										4
		|    |    |    |    +------------------------------------+					4
		|    |    |    |											3
		|    |    |    +-----------------------------------------+					3
		|    |    |													2
		|    |    +----------------------------------------------+					2
		|    |														1
		|    +---------------------------------------------------+					1
		|				Timer1 interval
		+--------------------------------------------------------+											PPS Interrupt, Start Timer1
*/

//Time slot interrupt
void TIM1_UP_IRQHandler(void)
{
    TIM1->SR &= ~TIM_SR_UIF;                    		//clear interrupt
    
    overflow_counter++;             					//increment ovf counter (starts from 1)
    
    if(overflow_counter == (DEVICES_IN_GROUP + 1))    	//if interrupt at the end of the last time slot
    {
        timer1_stop_reload();
        overflow_counter = 0;
        main_flags.time_slots_end = 1;
        led_green_off();
    }
    else
    {
    	time_slot = overflow_counter;

		if (time_slot == p_settings->device_number)
		{
			si4463_tx_packet();
		}
		else
		{
			si4463_start_rx();
		}
    }

    if (main_flags.act_status == 1)
    {
    	main_flags.act_status = 0;
    	led_green_on();						//make ACT led on only here, after we are shure that gps data is valid (otherwise we would never reach this interrupt)
    }
}



//Uptime counter (every 1 second)
void SysTick_Handler(void)
{
    uptime++;

    main_flags.battery_low = adc_get_bat_voltage();
    
    calc_timeout(uptime);			//always calculate timeout for each device, even if this function is disabled
    check_timeout(); 				//also check timeout in order to set/reset timeout flags
}



//End of "beep"
void TIM2_IRQHandler(void)
{
	timer2_stop();
	TIM2->SR &= ~TIM_SR_UIF;        //clear gating timer int

	timer3_stop();
	led_board_off();
}



uint32_t get_uptime(void)
{
	return uptime;
}



struct main_flags_struct *get_main_flags(void)
{
	return &main_flags;
}
