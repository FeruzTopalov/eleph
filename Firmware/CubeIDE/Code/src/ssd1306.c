/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: ssd1306.c
*/

#include "stm32f10x.h"
#include "ssd1306.h"
#include "ssd1306_font6x8.h"
#include "gpio.h"
#include "spi.h"
#include "service.h"
#include "menu.h"



#define LCD_SIZE_BYTES    (1024)
#define LCD_SIZE_X        (128)
#define LCD_SIZE_Y        (64)
#define FONT_SIZE_X       (6)
#define FONT_SIZE_Y       (8)

#define SSD1306_COMMAND_ON	(0xAF)
#define SSD1306_COMMAND_OFF	(0xAE)



uint8_t screen_buf[LCD_SIZE_BYTES];     		//public array 128x64 pixels
uint16_t buf_pos = 0;                   		//public var 0 - 1023
uint8_t display_status = SSD1306_DISPLAY_ON;	//lcd panel status on/off



//SSD1306 init sequence (first byte in line = amount of config bytes in line)
const uint8_t ssd1306_conf[] =
{// len,  val1, val2, ...
    0x02, 0x20, 0x00,           /* horizontal adressing */ \
    0x02, 0xA1, 0xC8,           /* invert orientation */ \
	0x02, 0x81, 0xFF,			/* set contrast */ \
    0x02, 0x8D, 0x14,		    /* enable charge pump */ \
	0x01, 0xAF,     			/* enable display */ \
    0x00                        /* end of the sequence */
};



//SSD1306 Init
void ssd1306_init(void)
{
	spi2_clock_enable();

    cs_ssd1306_inactive();      //ports init state
    res_ssd1306_inactive();
    ssd1306_command_mode();
    delay_cyc(10000);
    
    res_ssd1306_active();       //reset ssd1306
    delay_cyc(10000);
    res_ssd1306_inactive();
    delay_cyc(10000);
    
    uint8_t i = 0;
    uint8_t len = 0;
    while (ssd1306_conf[i] != 0x00)
    {
        len = ssd1306_conf[i++];
        
        cs_ssd1306_active();
        while (len--)
        {
            spi2_trx(ssd1306_conf[i++]);
        }
        cs_ssd1306_inactive();
    }
    
    spi2_clock_disable();

    ssd1306_clear();
    ssd1306_update();
}



void ssd1306_on(void)
{
	spi2_clock_enable();
	ssd1306_command_mode();
	cs_ssd1306_active();
	spi2_trx(SSD1306_COMMAND_ON);
	cs_ssd1306_inactive();
	spi2_clock_disable();
}



void ssd1306_off(void)
{
	spi2_clock_enable();
	ssd1306_command_mode();
	cs_ssd1306_active();
	spi2_trx(SSD1306_COMMAND_OFF);
	cs_ssd1306_inactive();
	spi2_clock_disable();
}



//Toggle display on or off
void ssd1306_toggle_display(void)
{
	if (display_status == SSD1306_DISPLAY_ON)
	{
		display_status = SSD1306_DISPLAY_OFF;
		ssd1306_off();
	}
	else
	{
		display_status = SSD1306_DISPLAY_ON;
		draw_current_menu(); //update screen content before enabling the lcd
		ssd1306_on();
	}
}



uint8_t ssd1306_get_display_status(void)
{
	return display_status;
}



//Update screen with buffer content
void ssd1306_update(void)
{
	if (display_status == SSD1306_DISPLAY_ON) //update display content only if the display is on
	{
		spi2_clock_enable();

		ssd1306_data_mode();
		cs_ssd1306_active();
		for (uint16_t i = 0; i < LCD_SIZE_BYTES; i++)
		{
			spi2_trx(screen_buf[i]);
		}
		cs_ssd1306_inactive();

		spi2_clock_disable();
	}
}



//Fill all screen buffer pixels
void ssd1306_fill(void)
{
    for (uint16_t i = 0; i < LCD_SIZE_BYTES; i++)
    {
        screen_buf[i] = 0xFF;
    }
}



//Clear all screen buffer pixels
void ssd1306_clear(void)
{
    for (uint16_t i = 0; i < LCD_SIZE_BYTES; i++)
    {
        screen_buf[i] = 0x00;
    }
}



//Clear, Set or Reset any pixel on the screen (x 0-127, y 0-63)
void ssd1306_pixel(uint8_t x, uint8_t y, int8_t action)
{
    switch (action)
    {
        case 0:     //clear pixel
            screen_buf[x + (y / 8) * LCD_SIZE_X] &= ~(1 << (y % 8));
            break;
        case 1:     //set pixel
            screen_buf[x + (y / 8) * LCD_SIZE_X] |= 1 << (y % 8);
            break;
        default:    //invert pixel
            screen_buf[x + (y / 8) * LCD_SIZE_X] ^= 1 << (y % 8);
            break;
    }
}



//Set character position on screen (rows 0-7, cols 0-20)
void ssd1306_pos(uint8_t row, uint8_t col)
{
    buf_pos = (row * 21 + col) * FONT_SIZE_X + 2 * row;   //+2 bytes, because 128 - 21 * 6 = 2
}



//Put one char in buffer in position, defined previously via ssd1306_pos()
void ssd1306_char(char chr, uint8_t inv)
{
    if (inv)
    {
        inv = 0xFF;
    }
    else
    {
        inv = 0x00;
    }
    
    for (uint8_t i = 0; i < FONT_SIZE_X - 1; i++)
    {
        screen_buf[buf_pos++] = font[(uint8_t)chr][i] ^ inv;
    }
    screen_buf[buf_pos++] = 0x00 ^ inv;       //intercharacter space
}



//Put one char in defined pos
void ssd1306_char_pos(uint8_t row, uint8_t col, char chr, uint8_t inv)
{
    ssd1306_pos(row, col);
    ssd1306_char(chr, inv);
}



//Print string on screen (with position)
void ssd1306_print(uint8_t row, uint8_t col, char *p_str, uint8_t inv)
{
    ssd1306_pos(row, col);
    
    while (*p_str)
    {
        ssd1306_char(*p_str++, inv);
    }
}



//Print string on screen (with position) in viceversa direction (decrease collumn)
void ssd1306_print_viceversa(uint8_t row, uint8_t col, char *p_str, uint8_t inv)
{
    uint8_t symb_cntr = 0;
    
    ssd1306_pos(row, col);
    
    while (*p_str)
    {
        p_str++;
        symb_cntr++;
    }
    
    while (symb_cntr)
    {
        symb_cntr--;
        ssd1306_char(*--p_str, inv);
        buf_pos -= 2 * FONT_SIZE_X;         //minus two characters position
    }
}



//Print string on screen
void ssd1306_print_next(char *p_str, uint8_t inv)
{
    while (*p_str)
    {
        ssd1306_char(*p_str++, inv);
    }
}



//Show bitmap
void ssd1306_bitmap(const uint8_t arr[])
{
    for (uint16_t i = 0; i < LCD_SIZE_BYTES; i++)
    {
        screen_buf[i] = arr[i];
    }
}



//Print byte on screen (debug function)
void ssd1306_print_byte(uint8_t row, uint8_t col, uint8_t *p_byte, uint8_t amount)
{
    ssd1306_pos(row, col);
    
    while (amount--)
    {
        screen_buf[buf_pos++] = *p_byte++;
    }
}
