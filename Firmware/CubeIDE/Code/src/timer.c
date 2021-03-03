/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: timer.c
*/

#include "stm32f10x.h"
#include "timer.h"
#include "gpio.h"



void systick_init(void);
void timer1_init(void);
void timer2_init(void);
void timer2_start(void);
void timer3_init(void);
void timer3_start(void);



uint8_t beep_mute = 0;



//Init all timers together
void timers_init(void)
{
	systick_init();
	timer1_init();
	timer2_init();
	timer3_init();
}



void make_a_beep(void)
{
	led_board_on();
	timer2_start(); //in order to switch off led_board

	if (beep_mute == 0)
	{
		timer3_start();
	}
}



void toggle_mute(void)
{
	beep_mute ^= (uint8_t)1;
}



uint8_t get_mute_flag(void)
{
	return beep_mute;
}



//SysTick timer init (tick every 1s to count uptime)
void systick_init(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;   //clock source = AHB/8 = 72MHz/8 = 9MHz
    SysTick->LOAD = (uint32_t)8999999;              //9000000Hz-1
    SysTick->VAL = 0;                               //reset counter value
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;      //enable interrupt
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;       //enable counter
}



//Timer1 init (time slot counter)
void timer1_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;     //enable timer 1 clock
    TIM1->PSC = (uint16_t)35999;            // 72MHz/(35999+1)=2kHz
    TIM1->ARR = (uint16_t)199;              // 2kHz/(199+1)=10Hz(100ms)
    TIM1->CR1 |= TIM_CR1_URS;               //only overflow generates interrupt
    TIM1->EGR = TIM_EGR_UG;                 //software update generation
    TIM1->SR &= ~TIM_SR_UIF;                //clear update interrupt
    TIM1->DIER |= TIM_DIER_UIE;             //update interrupt enable
    
    NVIC_EnableIRQ(TIM1_UP_IRQn);           //enable interrupt
}



//Timer1 start
void timer1_start(void)
{
    TIM1->CR1 |= TIM_CR1_CEN;               //enable counter
}



//Timer1 stop and reload
void timer1_stop_reload(void)
{
    TIM1->CR1 &= ~TIM_CR1_CEN;              //disable counter
    TIM1->EGR = TIM_EGR_UG;                 //software update generation
}



//Timer 2 init (gating timer, the length of the "beep")
void timer2_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //enable timer clock
	TIM2->PSC = (uint16_t)35999;        //72MHz/(35999+1)=2kHz
	TIM2->ARR = (uint16_t)199;          //2kHz/(199+1)=10Hz(100ms)
	TIM2->EGR = TIM_EGR_UG;             //software update generation
	TIM2->DIER |= TIM_DIER_UIE;         //update interrupt enable

	NVIC_EnableIRQ(TIM2_IRQn);
}



void timer2_stop(void)
{
	TIM2->CR1 &= ~TIM_CR1_CEN;      //stop gating timer
}



void timer2_start(void)
{
	TIM2->CR1 |= TIM_CR1_CEN;   	//start gating timer
}



//Timer 3 init (pwm timer, the frequency of the "beep")
void timer3_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; //enable timer clock
	TIM3->PSC = (uint16_t)8999;         //72MHz/(8999+1)=8kHz
	TIM3->ARR = (uint16_t)3;            //8kHz/(3+1)=2kHz
	TIM3->CCR2 = (uint16_t)2;           //duty cycle 2/(3+1)=0.5
	TIM3->CCMR1 |= TIM_CCMR1_OC2M_2;    //PWM mode 2
	TIM3->CCMR1 |= TIM_CCMR1_OC2M_1;
	TIM3->CCMR1 |= TIM_CCMR1_OC2M_0;
	TIM3->CCER |= TIM_CCER_CC2E;        //CH2 output enable
}



void timer3_stop(void)
{
	TIM3->CR1 &= ~TIM_CR1_CEN;      //disable PWM timer
	TIM3->CNT = 0;                  //force output low
}



void timer3_start(void)
{
	TIM3->CR1 |= TIM_CR1_CEN;   	//enable PWM timer
}
