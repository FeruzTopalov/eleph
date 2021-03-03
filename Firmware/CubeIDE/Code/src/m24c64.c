/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: m24c64.c
*/

#include "stm32f10x.h"
#include "m24c64.h"
#include "i2c.h"



#define M24C64_POLL_ATTEMPTS            (100)   //takes ~10ms @ 100 kHz I2C

#define M24C64_ADDRESS_WRITE_MODE       (0xA0)
#define M24C64_ADDRESS_READ_MODE        (0xA1)

#define M24C64_WHOLE_SIZE               (8192)
#define M24C64_PAGES_TOTAL              (256)



uint8_t m24c64_poll(void)
{
    uint16_t SR1_tmp;
    uint16_t SR2_tmp;
    uint8_t attempts = M24C64_POLL_ATTEMPTS;
    
    while (attempts)
    {
        //Start
        I2C2->CR1 |= I2C_CR1_START;
        //Wait for start generated
        while (!(I2C2->SR1 & I2C_SR1_SB))
        {
        }
        //Clear
        SR1_tmp = I2C2->SR1;
        
        //Device address
        I2C2->DR = (uint8_t)M24C64_ADDRESS_WRITE_MODE;
        //Wait for address end of transmission or NAK reception
        do
        {
            SR1_tmp = I2C2->SR1;
        }
        while (!(SR1_tmp & I2C_SR1_ADDR) && !(SR1_tmp & I2C_SR1_AF));    //"ADDR is not set after a NACK reception"
        //Clear
        SR1_tmp = I2C2->SR1;
        SR2_tmp = I2C2->SR2;
        
        //Check acknowledge failure
        if (SR1_tmp & I2C_SR1_AF)
        {
            //no acknowledge is returned, slave is busy
            I2C2->SR1 = ~(I2C_SR1_AF);   //write 0 to clear AF bit
            
            attempts--;   //slave is not ready, decrement attmepts counter
        }
        else if (SR1_tmp & I2C_SR1_ADDR)
        {
            //Stop
            I2C2->CR1 |= I2C_CR1_STOP;
            
            return 1;   //slave is ready
        }
        else
        {
            attempts--; //???
        }
    }

    SR2_tmp = SR2_tmp + 1;
    
    //Stop before end
    I2C2->CR1 |= I2C_CR1_STOP;
    
    return 0;       //end of attmepts, slave is busy or absent
}



uint8_t m24c64_read_byte(uint16_t memory_address)
{
    uint8_t result = 0;
    
    if (m24c64_poll())
    {
        uint16_t SR_tmp;
        
        //Start
        I2C2->CR1 |= I2C_CR1_START;
        //Wait for start generated
        while (!(I2C2->SR1 & I2C_SR1_SB))
        {
        }
        //Clear
        SR_tmp = I2C2->SR1;
        
        //Device address
        I2C2->DR = (uint8_t)M24C64_ADDRESS_WRITE_MODE;
        //Wait for address end of transmission
        while (!(I2C2->SR1 & I2C_SR1_ADDR))
        {
        }
        //Clear
        SR_tmp = I2C2->SR1;
        SR_tmp = I2C2->SR2;
        
        //Memory address high byte
        I2C2->DR = (uint8_t)(memory_address >> 8);
        //Wait for data register empty
        while (!(I2C2->SR1 & I2C_SR1_TXE))
        {
        }
        
        //Memory address low byte
        I2C2->DR = (uint8_t)memory_address;
        //Wait for data register empty
        while (!(I2C2->SR1 & I2C_SR1_TXE))
        {
        }
        
        
        //Start (restart actually)
        I2C2->CR1 |= I2C_CR1_START;
        //Wait for start generated
        while (!(I2C2->SR1 & I2C_SR1_SB))
        {
        }
        //Clear
        SR_tmp = I2C2->SR1;
        
        //Device address
        I2C2->DR = (uint8_t)M24C64_ADDRESS_READ_MODE;
        //Wait for address end of transmission
        while (!(I2C2->SR1 & I2C_SR1_ADDR))
        {
        }
        //Clear
        SR_tmp = I2C2->SR1;
        SR_tmp = I2C2->SR2;
        
        //NACK next byte
        I2C2->CR1 &= ~I2C_CR1_ACK;
        //Stop
        I2C2->CR1 |= I2C_CR1_STOP;
        //Wait for data register not empty
        while (!(I2C2->SR1 & I2C_SR1_RXNE))
        {
        }

        SR_tmp = SR_tmp + 1;
        
        //Read requested byte
        result = I2C2->DR;
    }
    else
    {
        result = 0;
    }
    
    return result;
}



void m24c64_write_byte(uint8_t data_byte, uint16_t memory_address)
{
    if (m24c64_poll())
    {
        uint8_t SR_tmp;
        
        //Start
        I2C2->CR1 |= I2C_CR1_START;
        //Wait for start generated
        while (!(I2C2->SR1 & I2C_SR1_SB))
        {
        }
        //Clear
        SR_tmp = I2C2->SR1;
        
        //Device address
        I2C2->DR = (uint8_t)M24C64_ADDRESS_WRITE_MODE;
        //Wait for address end of transmission
        while (!(I2C2->SR1 & I2C_SR1_ADDR))
        {
        }
        //Clear
        SR_tmp = I2C2->SR1;
        SR_tmp = I2C2->SR2;
        
        //Memory address high byte
        I2C2->DR = (uint8_t)(memory_address >> 8);
        //Wait for data register empty
        while (!(I2C2->SR1 & I2C_SR1_TXE))
        {
        }
        
        //Memory address low byte
        I2C2->DR = (uint8_t)memory_address;
        //Wait for data register empty
        while (!(I2C2->SR1 & I2C_SR1_TXE))
        {
        }
        
        //Write byte
        I2C2->DR = data_byte;
        //Wait byte transfer finish
        while (!(I2C2->SR1 & I2C_SR1_BTF))
        {
        }

        SR_tmp = SR_tmp + 1;
        
        //Stop
        I2C2->CR1 |= I2C_CR1_STOP;
    }
}



void m24c64_read_page(uint8_t data_array[], uint8_t page_address)
{
    if (m24c64_poll())
    {
        uint8_t SR_tmp;
        uint16_t memory_address = page_address * M24C64_PAGE_SIZE;
        
        //Start
        I2C2->CR1 |= I2C_CR1_START;
        //Wait for start generated
        while (!(I2C2->SR1 & I2C_SR1_SB))
        {
        }
        //Clear
        SR_tmp = I2C2->SR1;
        
        //Device address
        I2C2->DR = (uint8_t)M24C64_ADDRESS_WRITE_MODE;
        //Wait for address end of transmission
        while (!(I2C2->SR1 & I2C_SR1_ADDR))
        {
        }
        //Clear
        SR_tmp = I2C2->SR1;
        SR_tmp = I2C2->SR2;
        
        //Memory address high byte
        I2C2->DR = (uint8_t)(memory_address >> 8);
        //Wait for data register empty
        while (!(I2C2->SR1 & I2C_SR1_TXE))
        {
        }
        
        //Memory address low byte
        I2C2->DR = (uint8_t)memory_address;
        //Wait for data register empty
        while (!(I2C2->SR1 & I2C_SR1_TXE))
        {
        }
        
        
        //Start (restart actually)
        I2C2->CR1 |= I2C_CR1_START;
        //Wait for start generated
        while (!(I2C2->SR1 & I2C_SR1_SB))
        {
        }
        //Clear
        SR_tmp = I2C2->SR1;
        
        //Device address
        I2C2->DR = (uint8_t)M24C64_ADDRESS_READ_MODE;
        //Wait for address end of transmission
        while (!(I2C2->SR1 & I2C_SR1_ADDR))
        {
        }
        //Clear
        SR_tmp = I2C2->SR1;
        SR_tmp = I2C2->SR2;
        
        for (uint8_t i = 0; i < M24C64_PAGE_SIZE - 1; i++)
        {
            //ACK next byte
            I2C2->CR1 |= I2C_CR1_ACK;
            //Wait for data register not empty
            while (!(I2C2->SR1 & I2C_SR1_RXNE))
            {
            }
            
            //Read byte
            data_array[i] = I2C2->DR;
        }
        
        //NACK last byte
        I2C2->CR1 &= ~I2C_CR1_ACK;
        //Stop
        I2C2->CR1 |= I2C_CR1_STOP;
        //Wait for data register not empty
        while (!(I2C2->SR1 & I2C_SR1_RXNE))
        {
        }
        
        //Read last byte
        data_array[M24C64_PAGE_SIZE - 1] = I2C2->DR;

        SR_tmp = SR_tmp + 1;
    }
}



void m24c64_write_page(uint8_t data_array[], uint8_t page_address)
{
    if (m24c64_poll())
    {
        uint8_t SR_tmp = 0;
        uint16_t memory_address = page_address * M24C64_PAGE_SIZE;
        
        //Start
        I2C2->CR1 |= I2C_CR1_START;
        //Wait for start generated
        while (!(I2C2->SR1 & I2C_SR1_SB))
        {
        }
        //Clear
        SR_tmp = I2C2->SR1;
        
        //Device address
        I2C2->DR = (uint8_t)M24C64_ADDRESS_WRITE_MODE;
        //Wait for address end of transmission
        while (!(I2C2->SR1 & I2C_SR1_ADDR))
        {
        }
        //Clear
        SR_tmp = I2C2->SR1;
        SR_tmp = I2C2->SR2;
        
        //Memory address high byte
        I2C2->DR = (uint8_t)(memory_address >> 8);
        //Wait for data register empty
        while (!(I2C2->SR1 & I2C_SR1_TXE))
        {
        }
        
        //Memory address low byte
        I2C2->DR = (uint8_t)memory_address;
        //Wait for data register empty
        while (!(I2C2->SR1 & I2C_SR1_TXE))
        {
        }
        
        for (uint8_t i = 0; i < M24C64_PAGE_SIZE; i++)
        {
            //Write byte
            I2C2->DR = data_array[i];
            //Wait byte transfer finish
            while (!(I2C2->SR1 & I2C_SR1_BTF))
            {
            }
        }

        SR_tmp = SR_tmp + 1;
        
        //Stop
        I2C2->CR1 |= I2C_CR1_STOP;
    }
}



void m24c64_erase_page(uint8_t page_address)
{
    uint8_t empty_page[M24C64_PAGE_SIZE];
    
    for (uint8_t i = 0; i < M24C64_PAGE_SIZE; i++)
    {
        empty_page[i] = M24C64_EMPTY_CELL_VALUE;
    }
    
    m24c64_write_page(&empty_page[0], page_address);
}



void m24c64_erase_all(void)
{
    for (uint16_t page = 0; page < M24C64_PAGES_TOTAL; page++)
    {
        m24c64_erase_page(page);
    }
}
