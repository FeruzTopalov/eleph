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
    
    //PA1 - Button 1
    GPIOA->CRL &= ~GPIO_CRL_MODE1;      //input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF1_0;     //input with pull
    GPIOA->CRL |= GPIO_CRL_CNF1_1; 
    GPIOA->ODR |= GPIO_ODR_ODR1;        //pull-up on
    
    //PA2 - Button 2
    GPIOA->CRL &= ~GPIO_CRL_MODE2;      //input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF2_0;     //input with pull
    GPIOA->CRL |= GPIO_CRL_CNF2_1; 
    GPIOA->ODR |= GPIO_ODR_ODR2;        //pull-up on
    
    //PA3 - Button 3
    GPIOA->CRL &= ~GPIO_CRL_MODE3;      //input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF3_0;     //input with pull
    GPIOA->CRL |= GPIO_CRL_CNF3_1; 
    GPIOA->ODR |= GPIO_ODR_ODR3;        //pull-up on
    
    //PA4 - Button 4
    GPIOA->CRL &= ~GPIO_CRL_MODE4;      //input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF4_0;     //input with pull
    GPIOA->CRL |= GPIO_CRL_CNF4_1; 
    GPIOA->ODR |= GPIO_ODR_ODR4;        //pull-up on
    
    //PA5 - Button 5
    GPIOA->CRL &= ~GPIO_CRL_MODE5;      //input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF5_0;     //input with pull
    GPIOA->CRL |= GPIO_CRL_CNF5_1; 
    GPIOA->ODR |= GPIO_ODR_ODR5;        //pull-up on
    
    //PA6 - Battery monitor switch
    GPIOA->CRL |= GPIO_CRL_MODE6;       //output 50 MHz
    GPIOA->CRL &= ~GPIO_CRL_CNF6;       //output push-pull

    //PA7 - Piezo Buzzer (PWM)
    GPIOA->CRL |= GPIO_CRL_MODE7;      //output mode
    GPIOA->CRL &= ~GPIO_CRL_CNF7_0;    //alternate output push-pull
    GPIOA->CRL |= GPIO_CRL_CNF7_1;

    //PA8 - RES (SSD1306)
    GPIOA->CRH |= GPIO_CRH_MODE8;      //output 50 MHz
    GPIOA->CRH &= ~GPIO_CRH_CNF8;      //output push-pull
    
    //PA9 - USART TX1 (Debug out)
    GPIOA->CRH |= GPIO_CRH_MODE9;       //output 50 MHz
    GPIOA->CRH &= ~GPIO_CRH_CNF9_0;     //alternate output push-pull
    GPIOA->CRH |= GPIO_CRH_CNF9_1;
    
    //PA10 - USART RX1 (GPS NMEA here)
    GPIOA->CRH &= ~GPIO_CRH_MODE10;     //input
    GPIOA->CRH |= GPIO_CRH_CNF10_0;     //alternate input floating
    GPIOA->CRH &= ~GPIO_CRH_CNF10_1;
    
    //PA11 - GPS (PPS interrupt)
    GPIOA->CRH &= ~GPIO_CRH_MODE11;     //input mode
    GPIOA->CRH &= ~GPIO_CRH_CNF11_0;    //input with pull
    GPIOA->CRH |= GPIO_CRH_CNF11_1; 
    GPIOA->ODR &= ~GPIO_ODR_ODR11;      //pull-down
    
    //PA12 - CTS (SI4463)
    GPIOA->CRH &= ~GPIO_CRH_MODE11;     //input mode
    GPIOA->CRH &= ~GPIO_CRH_CNF11_0;    //input with pull
    GPIOA->CRH |= GPIO_CRH_CNF11_1; 
    GPIOA->ODR &= ~GPIO_ODR_ODR11;      //pull-down
    
    //PA15 - CS (SI4463)
    GPIOA->CRH |= GPIO_CRH_MODE15;      //output 50 MHz
    GPIOA->CRH &= ~GPIO_CRH_CNF15;      //output push-pull
    
    
    //Port B
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    
    //PB0 - Red LED
    GPIOB->CRL |= GPIO_CRL_MODE0;       //output 50 MHz
    GPIOB->CRL &= ~GPIO_CRL_CNF0;       //output push-pull
    
    //PB1 - Green LED
    GPIOB->CRL |= GPIO_CRL_MODE1;       //output 50 MHz
    GPIOB->CRL &= ~GPIO_CRL_CNF1;       //output push-pull
    
    //PB3 - SCK (SI4463)
    GPIOB->CRL |= GPIO_CRL_MODE3;       //output 50 MHz
    GPIOB->CRL &= ~GPIO_CRL_CNF3_0;     //alternate output push-pull
    GPIOB->CRL |= GPIO_CRL_CNF3_1;
    
    //PB4 - MISO (SI4463)
    GPIOB->CRL &= ~GPIO_CRL_MODE4;      //input mode
    GPIOB->CRL |= GPIO_CRL_CNF4_0;      //floating input
    GPIOB->CRL &= ~GPIO_CRL_CNF4_1;
    
    //PB5 - MOSI (SI4463)
    GPIOB->CRL |= GPIO_CRL_MODE5;       //output 50 MHz
    GPIOB->CRL &= ~GPIO_CRL_CNF5_0;     //alternate output push-pull
    GPIOB->CRL |= GPIO_CRL_CNF5_1;
    
    //PB6 - IRQn (SI4463 interrupt)
    GPIOB->CRL &= ~GPIO_CRL_MODE6;      //input mode
    GPIOB->CRL &= ~GPIO_CRL_CNF6_0;     //input with pull
    GPIOB->CRL |= GPIO_CRL_CNF6_1;
    GPIOB->ODR |= GPIO_ODR_ODR6;        //pull-up
    
    //PB7 - SDN (SI4463)
    GPIOB->CRL |= GPIO_CRL_MODE7;       //output 50 MHz
    GPIOB->CRL &= ~GPIO_CRL_CNF7;       //output push-pull
    
    //PB10 - I2C SCL
    GPIOB->CRH |= GPIO_CRH_MODE10;       //output 50 MHz
    GPIOB->CRH |= GPIO_CRH_CNF10;        //alternate function open-drain
    
    //PB11 - I2C SDA
    GPIOB->CRH |= GPIO_CRH_MODE11;       //output max speed
    GPIOB->CRH |= GPIO_CRH_CNF11;        //alternate function open-drain
    
    //PB12 - CS (SSD1306)
    GPIOB->CRH |= GPIO_CRH_MODE12;      //output 50 MHz
    GPIOB->CRH &= ~GPIO_CRH_CNF12;      //output push-pull
    
    //PB13 - SCK (SSD1306)
    GPIOB->CRH |= GPIO_CRH_MODE13;      //output 50 MHz
    GPIOB->CRH &= ~GPIO_CRH_CNF13_0;    //alternate output push-pull
    GPIOB->CRH |= GPIO_CRH_CNF13_1;
    
    //PB14 - D/C (SSD1306)
    GPIOB->CRH |= GPIO_CRH_MODE14;      //output 50 MHz
    GPIOB->CRH &= ~GPIO_CRH_CNF14;      //output push-pull
    
    //PB15 - MOSI (SSD1306)
    GPIOB->CRH |= GPIO_CRH_MODE15;      //output 50 MHz
    GPIOB->CRH &= ~GPIO_CRH_CNF15_0;    //alternate output push-pull
    GPIOB->CRH |= GPIO_CRH_CNF15_1;
    
    
    //Port C
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    
    //PC13 - Blue led
    GPIOC->CRH |= GPIO_CRH_MODE13;      //output 50 MHz
    GPIOC->CRH &= ~GPIO_CRH_CNF13;      //output push-pull
    led_board_off();
}



//Init external interrupts
void ext_int_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;         //enable afio clock
    
    //PA11 GPS PPS interrupt on rising edge
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI11_PA;  //exti 11 source is port A
    EXTI->RTSR |= EXTI_RTSR_TR11;               //interrupt 11 on rising edge
    EXTI->IMR |= EXTI_IMR_MR11;                 //unmask interrupt 11
    NVIC_EnableIRQ(EXTI15_10_IRQn);             //enable interrupt
    
    //PB6 SI4463 RX interrupt on falling edge
    AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI6_PB;   //exti 6 source is port B
    EXTI->FTSR |= EXTI_FTSR_TR6;                //interrupt 6 on falling edge
    EXTI->IMR |= EXTI_IMR_MR6;                  //unmask interrupt 6
    NVIC_EnableIRQ(EXTI9_5_IRQn);               //enable interrupt
    
    EXTI->PR = (uint32_t)0x0007FFFF;            //clear all pending interrupts
}



//Red led on
void led_red_on(void)
{
    GPIOB->BSRR = GPIO_BSRR_BS0;
}



//Red led off
void led_red_off(void)
{
    GPIOB->BSRR = GPIO_BSRR_BR0;
}



//Red green on
void led_green_on(void)
{
    GPIOB->BSRR = GPIO_BSRR_BS1;
}



//Red green off
void led_green_off(void)
{
    GPIOB->BSRR = GPIO_BSRR_BR1;
}



//Blue led on
void led_board_on(void)
{
    GPIOC->BSRR = GPIO_BSRR_BR13;
}



//Blue led off
void led_board_off(void)
{
    GPIOC->BSRR = GPIO_BSRR_BS13;
}



//SDN SI4463 active
void sdn_si4463_active(void)
{
    GPIOB->BSRR = GPIO_BSRR_BS7;
}



//SDN SI4463 inactive
void sdn_si4463_inactive(void)
{
    GPIOB->BSRR = GPIO_BSRR_BR7;
}



//CS SI4463 active
void cs_si4463_active(void)
{
    GPIOA->BSRR = GPIO_BSRR_BR15;
}



//CS SI4463 inactive
void cs_si4463_inactive(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS15;
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
void bat_mon_on(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS6;
}



//Battery monitoring off
void bat_mon_off(void)
{
    GPIOA->BSRR = GPIO_BSRR_BR6;
}

