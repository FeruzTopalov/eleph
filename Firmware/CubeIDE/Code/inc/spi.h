/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: spi.h
*/

void spi1_init(void);
void spi2_init(void);
uint8_t spi1_trx(uint8_t send_data);
uint8_t spi2_trx(uint8_t send_data);
