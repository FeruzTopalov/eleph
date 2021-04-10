/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: si4463.h
*/



#ifndef SI4463_HEADER
#define SI4463_HEADER



void si4463_init(void);
void si4463_tx_packet(void);
void si4463_start_rx(void);
uint8_t si4463_get_rx_packet(void);
uint8_t *get_air_packet_tx(void);
uint8_t *get_air_packet_rx(void);



#endif /*SI4463_HEADER*/
