/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: m24c64.h
*/

#define M24C64_PAGE_SIZE                (32)
#define M24C64_EMPTY_CELL_VALUE         (0xFF)

uint8_t m24c64_poll(void);
uint8_t m24c64_read_byte(uint16_t memory_address);
void m24c64_write_byte(uint8_t data_byte, uint16_t memory_address);
void m24c64_read_page(uint8_t data_array[], uint8_t page_address);
void m24c64_write_page(uint8_t data_array[], uint8_t page_address);
void m24c64_erase_page(uint8_t page_address);
void m24c64_erase_all(void);
