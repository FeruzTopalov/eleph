/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: gpio.c
*/



#include "stm32f10x.h"
#include "gpio.h"



//Initialization of all used ports
void gpio_init(void)
{
    //Port A
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    
    //PA0 - ADC0 (Battery voltage)
    GPIOA->CRL &= ~GPIO_CRL_MODE0;      //input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF0;       //analog input
    
    //PA1 - Button UP
    GPIOA->CRL &= ~GPIO_CRL_MODE1;      //input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF1_0;     //input with pull
    GPIOA->CRL |= GPIO_CRL_CNF1_1; 
    GPIOA->ODR |= GPIO_ODR_ODR1;        //pull-up on
    
    //PA2 - Button DOWN
    GPIOA->CRL &= ~GPIO_CRL_MODE2;      //input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF2_0;     //input with pull
    GPIOA->CRL |= GPIO_CRL_CNF2_1; 
    GPIOA->ODR |= GPIO_ODR_ODR2;        //pull-up on
    
    //PA3 - Button OK
    GPIOA->CRL &= ~GPIO_CRL_MODE3;      //input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF3_0;     //input with pull
    GPIOA->CRL |= GPIO_CRL_CNF3_1; 
    GPIOA->ODR |= GPIO_ODR_ODR3;        //pull-up on
    
    //PA4 - Button LOCK
    GPIOA->CRL &= ~GPIO_CRL_MODE4;      //input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF4_0;     //input with pull
    GPIOA->CRL |= GPIO_CRL_CNF4_1; 
    GPIOA->ODR |= GPIO_ODR_ODR4;        //pull-up on
    
    //PA5 - Button ESC
    GPIOA->CRL &= ~GPIO_CRL_MODE5;      //input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF5_0;     //input with pull
    GPIOA->CRL |= GPIO_CRL_CNF5_1; 
    GPIOA->ODR |= GPIO_ODR_ODR5;        //pull-up on
    
    //PA6 - Piezo Buzzer (PWM) P
    GPIOA->CRL &= ~GPIO_CRL_MODE6_0;    //output 2 MHz
    GPIOA->CRL |= GPIO_CRL_MODE6_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF6;       //alternate output push-pull
    GPIOA->CRL |= GPIO_CRL_CNF6_1;

    //PA7 - Piezo Buzzer (PWM) N
    GPIOA->CRL &= ~GPIO_CRL_MODE7_0;  	//output 2 MHz
    GPIOA->CRL |= GPIO_CRL_MODE7_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF7_0;    	//alternate output push-pull
    GPIOA->CRL |= GPIO_CRL_CNF7_1;

    //PA8 - RES (SSD1306)
    GPIOA->CRH &= ~GPIO_CRH_MODE8_0;    //output 2 MHz
    GPIOA->CRH |= GPIO_CRH_MODE8_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF8;      	//output push-pull
    
    //PA9 - USART TX1 (GPS config)
    GPIOA->CRH &= ~GPIO_CRH_MODE9_0;    //output 2 MHz
    GPIOA->CRH |= GPIO_CRH_MODE9_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF9_0;     //alternate output push-pull
    GPIOA->CRH |= GPIO_CRH_CNF9_1;
    
    //PA10 - USART RX1 (GPS NMEA stream)
    GPIOA->CRH &= ~GPIO_CRH_MODE10;     //input
    GPIOA->CRH |= GPIO_CRH_CNF10_0;     //alternate input floating
    GPIOA->CRH &= ~GPIO_CRH_CNF10_1;
    
    //PA11 - GPS (PPS interrupt)
    GPIOA->CRH &= ~GPIO_CRH_MODE11;     //input mode
    GPIOA->CRH &= ~GPIO_CRH_CNF11_0;    //input with pull
    GPIOA->CRH |= GPIO_CRH_CNF11_1; 
    GPIOA->ODR &= ~GPIO_ODR_ODR11;      //pull-down
    
    //PA12 - RF Reset
    //GPIOA->CRH &= ~GPIO_CRH_MODE12_0;   //output 2 MHz
    //GPIOA->CRH |= GPIO_CRH_MODE12_1;
    //GPIOA->CRH &= ~GPIO_CRH_CNF12;		//output push-pull
    
    //PA15 - RF Chip Select
    GPIOA->CRH &= ~GPIO_CRH_MODE15_0;   //output 2 MHz
    GPIOA->CRH |= GPIO_CRH_MODE15_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF15;      //output push-pull
    
    

    //Port B
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    
    //PB0 - Green LED
    GPIOB->CRL &= ~GPIO_CRL_MODE0_0;    //output 2 MHz
    GPIOB->CRL |= GPIO_CRL_MODE0_1;
    GPIOB->CRL &= ~GPIO_CRL_CNF0;       //output push-pull
    
    //PB1 - Red LED
    GPIOB->CRL &= ~GPIO_CRL_MODE1_0;    //output 2 MHz
    GPIOB->CRL |= GPIO_CRL_MODE1_1;
    GPIOB->CRL &= ~GPIO_CRL_CNF1;       //output push-pull
    
    //PB3 - RF Clock
    GPIOB->CRL &= ~GPIO_CRL_MODE3_0;    //output 2 MHz
    GPIOB->CRL |= GPIO_CRL_MODE3_1;
    GPIOB->CRL &= ~GPIO_CRL_CNF3_0;     //alternate output push-pull
    GPIOB->CRL |= GPIO_CRL_CNF3_1;
    
    //PB4 - RF MISO
    GPIOB->CRL &= ~GPIO_CRL_MODE4;      //input mode
    GPIOB->CRL |= GPIO_CRL_CNF4_0;      //floating input
    GPIOB->CRL &= ~GPIO_CRL_CNF4_1;
    
    //PB5 - RF MOSI
    GPIOB->CRL &= ~GPIO_CRL_MODE5_0;    //output 2 MHz
    GPIOB->CRL |= GPIO_CRL_MODE5_1;
    GPIOB->CRL &= ~GPIO_CRL_CNF5_0;     //alternate output push-pull
    GPIOB->CRL |= GPIO_CRL_CNF5_1;
    
    //PB6 - RF IRQ2
    GPIOB->CRL &= ~GPIO_CRL_MODE6;      //input mode
    GPIOB->CRL &= ~GPIO_CRL_CNF6_0;     //input with pull
    GPIOB->CRL |= GPIO_CRL_CNF6_1;
    GPIOB->ODR |= GPIO_ODR_ODR6;        //pull-up
    
    //PB7 - RF IRQ1
    GPIOB->CRL &= ~GPIO_CRL_MODE7;      //input mode
    GPIOB->CRL &= ~GPIO_CRL_CNF7_0;     //input with pull
    GPIOB->CRL |= GPIO_CRL_CNF7_1;
    GPIOB->ODR |= GPIO_ODR_ODR7;        //pull-up

    //PB8 - RF IRQ0
    GPIOB->CRH &= ~GPIO_CRH_MODE8;      //input mode
    GPIOB->CRH &= ~GPIO_CRH_CNF8_0;     //input with pull
    GPIOB->CRH |= GPIO_CRH_CNF8_1;
    GPIOB->ODR |= GPIO_ODR_ODR8;        //pull-up

    //PB9 - Power Switch
    GPIOB->CRH &= ~GPIO_CRH_MODE9_0;   	//output 2 MHz
    GPIOB->CRH |= GPIO_CRH_MODE9_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF9;		//output push-pull
    
    //PB10 - I2C SCL
    GPIOB->CRH &= ~GPIO_CRH_MODE10_0;   //output 2 MHz
    GPIOB->CRH |= GPIO_CRH_MODE10_1;
    GPIOB->CRH |= GPIO_CRH_CNF10;       //alternate function open-drain
    
    //PB11 - I2C SDA
    GPIOB->CRH &= ~GPIO_CRH_MODE11_0;   //output 2 MHz
    GPIOB->CRH |= GPIO_CRH_MODE11_1;
    GPIOB->CRH |= GPIO_CRH_CNF11;       //alternate function open-drain
    
    //PB12 - CS (SSD1306)
    GPIOB->CRH &= ~GPIO_CRH_MODE12_0;   //output 2 MHz
    GPIOB->CRH |= GPIO_CRH_MODE12_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF12;      //output push-pull
    
    //PB13 - SCK (SSD1306)
    GPIOB->CRH &= ~GPIO_CRH_MODE13_0;   //output 2 MHz
    GPIOB->CRH |= GPIO_CRH_MODE13_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF13_0;    //alternate output push-pull
    GPIOB->CRH |= GPIO_CRH_CNF13_1;
    
    //PB14 - D/C (SSD1306)
    GPIOB->CRH &= ~GPIO_CRH_MODE14_0;   //output 2 MHz
    GPIOB->CRH |= GPIO_CRH_MODE14_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF14;      //output push-pull
    
    //PB15 - MOSI (SSD1306)
    GPIOB->CRH &= ~GPIO_CRH_MODE15_0;   //output 2 MHz
    GPIOB->CRH |= GPIO_CRH_MODE15_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF15_0;    //alternate output push-pull
    GPIOB->CRH |= GPIO_CRH_CNF15_1;
}



//Init external interrupts
void ext_int_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;         //enable afio clock
    
    //PA11 - GPS PPS interrupt on rising edge
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI11_PA;  //exti 11 source is port A
    EXTI->RTSR |= EXTI_RTSR_TR11;               //interrupt 11 on rising edge
    EXTI->IMR |= EXTI_IMR_MR11;                 //unmask interrupt 11
    NVIC_EnableIRQ(EXTI15_10_IRQn);             //enable interrupt

    //PB8 - IRQ0 interrupt on rising edge
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PB;	//exti 8 source is port B
    EXTI->RTSR |= EXTI_RTSR_TR8;				//interrupt 8 on rising edge
    EXTI->IMR |= EXTI_IMR_MR8;					//unmask interrupt 8

    NVIC_EnableIRQ(EXTI9_5_IRQn);               //enable IRQn interrupts

    EXTI->PR = (uint32_t)0x0007FFFF;            //clear all pending interrupts
}



//Red led on
void led_red_on(void)
{
    GPIOB->BSRR = GPIO_BSRR_BS1;
}



//Red led off
void led_red_off(void)
{
    GPIOB->BSRR = GPIO_BSRR_BR1;
}



//Red green on
void led_green_on(void)
{
    GPIOB->BSRR = GPIO_BSRR_BS0;
}



//Red green off
void led_green_off(void)
{
    GPIOB->BSRR = GPIO_BSRR_BR0;
}



//RF CS active
void cs_rfm98_active(void)
{
    GPIOA->BSRR = GPIO_BSRR_BR15;
}



//RF CS inactive
void cs_rfm98_inactive(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS15;
}



//RF RES active
void res_rfm98_active(void)
{


    GPIOA->CRH &= ~GPIO_CRH_MODE12_0;   //output 2 MHz
    GPIOA->CRH |= GPIO_CRH_MODE12_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF12;		//output push-pull


    GPIOA->BSRR = GPIO_BSRR_BR12;
}



//RF RES inactive
void res_rfm98_inactive(void)
{
    GPIOA->CRH &= ~GPIO_CRH_MODE12;   	//input
    GPIOA->CRH &= ~GPIO_CRH_CNF12_1;	//float
    GPIOA->CRH |= GPIO_CRH_CNF12_0;
}



//RES SSD1306 active
void res_ssd1306_active(void)
{
    GPIOA->BSRR = GPIO_BSRR_BR8;
}



//RES SSD1306 inactive
void res_ssd1306_inactive(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS8;
}



//Data mode SSD1306
void ssd1306_data_mode(void)
{
    GPIOB->BSRR = GPIO_BSRR_BS14;
}



//Command mode SSD1306
void ssd1306_command_mode(void)
{
    GPIOB->BSRR = GPIO_BSRR_BR14;
}



//CS SSD1306 active
void cs_ssd1306_active(void)
{
    GPIOB->BSRR = GPIO_BSRR_BR12;
}



//CS SSD1306 inactive
void cs_ssd1306_inactive(void)
{
    GPIOB->BSRR = GPIO_BSRR_BS12;
}



//Battery monitoring on
void hold_power(void)
{
    GPIOB->BSRR = GPIO_BSRR_BS9;
}



//Battery monitoring off
void release_power(void)
{
    GPIOB->BSRR = GPIO_BSRR_BR9;
}

