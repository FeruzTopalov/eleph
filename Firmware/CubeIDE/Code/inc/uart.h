/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: uart.h
*/



#ifndef UART_HEADER
#define UART_HEADER



#define UART_BUF_LEN        (1024)



void uart_dma_init(void);
void uart_dma_stop(void);
void uart_dma_restart(void);
void backup_and_clear_uart_buffer(void);
void clear_uart_buffer(void);
void uart_tx(uint8_t data);



#endif /*UART_HEADER*/
