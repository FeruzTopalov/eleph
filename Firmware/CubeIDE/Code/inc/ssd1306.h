/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: ssd1306.h
*/



#ifndef SSD1306_HEADER
#define SSD1306_HEADER



#define SSD1306_DISPLAY_ON	(1)
#define SSD1306_DISPLAY_OFF	(0)



#define LCD_LAST_COL     	(20)
#define LCD_LAST_ROW       	(7)



//Special sympols
#define SYMB_ARROW_UP       ('\x80')
#define SYMB_ARROW_DOWN     ('\x81')
#define SYMB_NO_SATT        ('\x82')
#define SYMB_SATT_1D        ('\x83')
#define SYMB_SATT_2D        ('\x84')
#define SYMB_SATT_3D        ('\x85')
#define SYMB_TIMEOUT        ('\x86')
#define SYMB_ALARM          ('\x87')
#define SYMB_BAT_0OF4       ('\x88')
#define SYMB_BAT_1OF4       ('\x89')
#define SYMB_BAT_2OF4       ('\x8A')
#define SYMB_BAT_3OF4       ('\x8B')
#define SYMB_BAT_4OF4       ('\x8C')
#define SYMB_DEGREE         ('\x8D')
#define SYMB_FENCE          ('\x8E')
#define SYMB_NOTE           ('\x8F')



void ssd1306_init(void);
void ssd1306_on(void);
void ssd1306_off(void);
void ssd1306_toggle_display(void);
uint8_t ssd1306_get_display_status(void);
void ssd1306_fill(void);
void ssd1306_clear(void);
void ssd1306_pixel(uint8_t x, uint8_t y, int8_t action);
void ssd1306_pos(uint8_t row, uint8_t col);
void ssd1306_char(char chr, uint8_t inv);
void ssd1306_char_pos(uint8_t row, uint8_t col, char chr, uint8_t inv);
void ssd1306_print(uint8_t row, uint8_t col, char *p_str, uint8_t inv);
void ssd1306_print_next(char *p_str, uint8_t inv);
void ssd1306_print_viceversa(uint8_t row, uint8_t col, char *p_str, uint8_t inv);
void ssd1306_bitmap(const uint8_t arr[]);
void ssd1306_update(void);
void ssd1306_print_byte(uint8_t row, uint8_t col, uint8_t *p_byte, uint8_t amount);



#endif /*SSD1306_HEADER*/
