/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: timer.c
*/

#include "stm32f10x.h"
#include "timer.h"
#include "gpio.h"
#include "bit_band.h"



void systick_init(void);
void systick_start(void);
void rtc_init(void);
void timer1_init(void);
void timer1_clock_disable(void);
void timer1_clock_enable(void);
void timer2_init(void);
void timer3_init(void);
void timer3_start(void);
void timer3_clock_enable(void);



uint8_t beep_mute = 0;



//Init all timers together
void timers_init(void)
{
	systick_init();
	rtc_init();
	timer1_init();
	timer2_init();
	timer3_init();
}



void make_a_beep(void)
{
	if (beep_mute == 0)
	{
		timer3_clock_enable();
		timer3_start();		//pwm
		systick_start();	//gating
	}
}



void toggle_mute(void)
{
	if (beep_mute == 0)
	{
		make_a_beep();
		beep_mute = 1;
	}
	else
	{
		beep_mute = 0;
		make_a_beep();
	}
}



uint8_t get_mute_flag(void)
{
	return beep_mute;
}



void rtc_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;				//Enable power interface
	RCC->APB1ENR |= RCC_APB1ENR_BKPEN;				//Enable backup interface
	PWR->CR |= PWR_CR_DBP;							//Disable backup domain write protection
    RCC->BDCR |=  RCC_BDCR_BDRST;					//Reset entire backup domain (reset RTC)
    RCC->BDCR &= ~RCC_BDCR_BDRST;

	RCC->CSR |= RCC_CSR_LSION;  					//Enable LSI 40 kHz
	while ((RCC->CSR & RCC_CSR_LSIRDY) == 0){}		//Wait for stabilize
	RCC->BDCR |= RCC_BDCR_RTCSEL_1;					//LSI is RTC clock source
	RCC->BDCR &= ~RCC_BDCR_RTCSEL_0;
	RCC->BDCR |= RCC_BDCR_RTCEN;					//Enable RTC

	while ((RTC->CRL & RTC_CRL_RTOFF) == 0){}		//Wait RTC ready to acquire new command
	RTC->CRL |= RTC_CRL_CNF;						//Enter config mode
	RTC->PRLH = 0;
	RTC->PRLL = 39999;								//(40000Hz-1)
	RTC->CRH |= RTC_CRH_SECIE;						//Enable Interrupt
	RTC->CRL &= ~RTC_CRL_CNF;						//Exit config mode
	while ((RTC->CRL & RTC_CRL_RTOFF) == 0){}

	PWR->CR &= ~PWR_CR_DBP;							//Enable backup domain write protection

	NVIC_EnableIRQ(RTC_IRQn);
}



//SysTick timer init (tick every 1s to count uptime)
void systick_init(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;   //clock source = AHB/8 = 3MHz/8 = 375 kHz
    SysTick->LOAD = (uint32_t)37499;              	//375000Hz/(37499+1)=10Hz
    SysTick->VAL = 0;                               //reset counter value
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;      //enable interrupt
}



//Stop buzzer gating timer
void systick_stop(void)
{
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}



//Start buzzer gating timer
void systick_start(void)
{
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}



//Timer1 init (time slot counter)
void timer1_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;     //enable timer 1 clock
    TIM1->PSC = (uint16_t)2999;            	// 3MHz/(2999+1)=1kHz
    TIM1->ARR = (uint16_t)99;              	// 1kHz/(99+1)=10Hz(100ms)
    TIM1->CR1 |= TIM_CR1_URS;               //only overflow generates interrupt
    TIM1->EGR = TIM_EGR_UG;                 //software update generation
    TIM1->SR &= ~TIM_SR_UIF;                //clear update interrupt
    TIM1->DIER |= TIM_DIER_UIE;             //update interrupt enable
    
    NVIC_EnableIRQ(TIM1_UP_IRQn);           //enable interrupt
    timer1_clock_disable();
}



void timer1_clock_disable(void)
{
	BIT_BAND_PERI(RCC->APB2ENR, RCC_APB2ENR_TIM1EN) = 0;
}



void timer1_clock_enable(void)
{
	BIT_BAND_PERI(RCC->APB2ENR, RCC_APB2ENR_TIM1EN) = 1;
}



//Timer1 start
void timer1_start(void)
{
	timer1_clock_enable();
    TIM1->CR1 |= TIM_CR1_CEN;               //enable counter
}



//Timer1 stop and reload
void timer1_stop_reload(void)
{
    TIM1->CR1 &= ~TIM_CR1_CEN;              //disable counter
    TIM1->EGR = TIM_EGR_UG;                 //software update generation
    timer1_clock_disable();
}



//Timer 2 init (buttons scan interval)
void timer2_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //enable timer clock
	TIM2->PSC = (uint16_t)299;         	// 3MHz/(299+1)=10kHz
	TIM2->ARR = (uint16_t)99;           // 10kHz/(99+1)=100Hz(10ms)
	TIM2->EGR = TIM_EGR_UG;             //software update generation
	TIM2->DIER |= TIM_DIER_UIE;         //update interrupt enable
	TIM2->CR1 |= TIM_CR1_CEN;   		//start timer

	NVIC_EnableIRQ(TIM2_IRQn);
}



//Timer 3 init (pwm timer, the frequency of the "beep")
void timer3_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; //enable timer clock
	TIM3->PSC = (uint16_t)374;         	//3MHz/(374+1)=8kHz
	TIM3->ARR = (uint16_t)3;            //8kHz/(3+1)=2kHz
	TIM3->CCR2 = (uint16_t)2;           //duty cycle 2/(3+1)=0.5
	TIM3->CCMR1 |= TIM_CCMR1_OC2M_2;    //PWM mode 2
	TIM3->CCMR1 |= TIM_CCMR1_OC2M_1;
	TIM3->CCMR1 |= TIM_CCMR1_OC2M_0;
	TIM3->CCER |= TIM_CCER_CC2E;        //CH2 output enable

	timer3_stop();
	timer3_clock_disable();
}



void timer3_clock_disable(void)
{
	BIT_BAND_PERI(RCC->APB1ENR, RCC_APB1ENR_TIM3EN) = 0;
}



void timer3_clock_enable(void)
{
	BIT_BAND_PERI(RCC->APB1ENR, RCC_APB1ENR_TIM3EN) = 1;
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
