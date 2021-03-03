/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: gpio.c
*/

#include "stm32f10x.h"
#include "i2c.h"
#include "service.h"



#define I2C_CLOCK_DELAY             (500)



void i2c_init(void)
{
    //I2C config
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;     //enable i2c clock
    
    I2C2->CR2 &= ~I2C_CR2_FREQ;             //clear bits before set
    I2C2->CR2 |= (uint16_t)10;              //10 MHz periph clock
    
    //Stndard mode
    I2C2->CCR |= (uint16_t)180;             //CCR = TSCL/(2 * TPCLK1); TSCL = 1 / 100kHz standard mode freq; TPCLK1 = 1 / APB1 clock 36 MHz
    I2C2->TRISE |= (uint16_t)37;            //TRISE = (Tr max/TPCLK1)+1; Tr max = 1000nS for standard mode
    
    I2C2->CR1 |= I2C_CR1_PE;                //enable i2c2
}
