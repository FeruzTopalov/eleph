/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: spi.c
*/

#include "stm32f10x.h"
#include "spi.h"



//Init SPI1
void spi1_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;             //enable clock spi1
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;             //enable alternate function clock
    AFIO->MAPR |= AFIO_MAPR_SPI1_REMAP;             //remap spi
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;    //disable JTAG
    SPI1->CR1 &= ~SPI_CR1_BR;                       //clock/2
    SPI1->CR1 |= SPI_CR1_SSM;
    SPI1->CR1 |= SPI_CR1_SSI;
    SPI1->CR1 |= SPI_CR1_MSTR;                      //master mode
    SPI1->CR1 &= ~SPI_CR1_CPOL;                     //SCK = 0 in IDLE
    SPI1->CR1 &= ~SPI_CR1_CPHA;                     //first rising edge capture
    SPI1->CR1 |= SPI_CR1_SPE;                       //enable spi
}



//Init SPI2
void spi2_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;     //enable clock spi2
    SPI2->CR1 &= ~SPI_CR1_BR;               //clock/2
    SPI2->CR1 |= SPI_CR1_SSM;
    SPI2->CR1 |= SPI_CR1_SSI;
    SPI2->CR1 |= SPI_CR1_MSTR;              //master mode
    SPI2->CR1 &= ~SPI_CR1_CPOL;             //SCK = 0 in IDLE
    SPI2->CR1 &= ~SPI_CR1_CPHA;             //first rising edge capture
    SPI2->CR1 |= SPI_CR1_SPE;               //enable spi
}



//TRX one byte via SPI1
uint8_t spi1_trx(uint8_t send_data)
{
    while(!(SPI1->SR & SPI_SR_TXE)){}       //while TXE bit = 0
    SPI1->DR = send_data;
    
    while(!(SPI1->SR & SPI_SR_RXNE)){}      //while RXNE = 0
    return SPI1->DR;
}



//TRX one byte via SPI2
uint8_t spi2_trx(uint8_t send_data)
{
    while (!(SPI2->SR & SPI_SR_TXE)){}      //while TXE bit = 0
    SPI2->DR = send_data;
    
    while (!(SPI2->SR & SPI_SR_RXNE)){}     //while RXNE = 0
    return  SPI2->DR;
}
