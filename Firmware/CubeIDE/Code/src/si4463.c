/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: si4463.c
*/

#include "stm32f10x.h"
#include "si4463.h"
#include "radio_config_Si4463.h"
#include "gpio.h"
#include "spi.h"
#include "service.h"
#include "main.h"
#include "settings.h"



void si4463_wait_cts(void);
void si4463_set_packet_len(uint8_t packet_len);
void si4463_set_tx_power(uint8_t tx_pow_reg_val);



#define SI4463_BYTE_DUMMY               (0x00)
#define SI4463_CMD_FIFO_INFO            (0x15)
#define SI4463_CMD_WRITE_TX_FIFO        (0x66)
#define SI4463_CMD_READ_RX_FIFO         (0x77)
#define SI4463_CMD_START_TX             (0x31)
#define SI4463_CMD_START_RX             (0x32)
#define SI4463_CMD_CHANGE_STATE         (0x34)
#define SI4463_CMD_GET_PH_STATUS        (0x21)
#define SI4463_CMD_READ_CMD_BUF         (0x44)
#define SI4463_CMD_SET_PROPERTY         (0x11)

#define SI4463_FIFO_INFO_TX_BIT         (0x01)
#define SI4463_FIFO_INFO_RX_BIT         (0x02)
#define SI4463_PH_PEND_CRC_ERROR_BIT    (0x08)
#define SI4463_PH_PEND_PACKET_RX_BIT    (0x10)

#define SI4463_NOCHANGE_STATE           (0x00)
#define SI4463_SLEEP_STATE              (0x01)
#define SI4463_READY_STATE              (0x03)

#define SI4463_PKT_FIELD_1_LENGTH_GROUP_ID          (0x12)
#define SI4463_PKT_FIELD_1_LENGTH_NUM_PROPS         (0x02)
#define SI4463_PKT_FIELD_1_LENGTH_START_PROP        (0x0D)

#define SI4463_PA_PWR_LVL_GROUP_ID      (0x22)
#define SI4463_PA_PWR_LVL_NUM_PROPS     (0x01)
#define SI4463_PA_PWR_LVL_START_PROP    (0x01)

#define TX_POWER_10MILLIW_REGISTER_VALUE       (20) //see datasheet graph
#define TX_POWER_25MILLIW_REGISTER_VALUE       (30)
#define TX_POWER_40MILLIW_REGISTER_VALUE       (40)
#define TX_POWER_100MILLIW_REGISTER_VALUE      (127)

#define TX_POWER_REGISTER_VALUES_ARRAY 		{ 	TX_POWER_10MILLIW_REGISTER_VALUE, 	\
												TX_POWER_25MILLIW_REGISTER_VALUE, 	\
												TX_POWER_40MILLIW_REGISTER_VALUE, 	\
												TX_POWER_100MILLIW_REGISTER_VALUE	}

#define AIR_PACKET_LEN      (17)        //bytes amount to tx/rx over air; does not include two bytes of CRC (refer to "radio_config_Si4463.h")



uint8_t air_packet_tx[AIR_PACKET_LEN];
uint8_t air_packet_rx[AIR_PACKET_LEN];
struct settings_struct *p_settings;
uint8_t tx_power_register_values[] = TX_POWER_REGISTER_VALUES_ARRAY;



//SI4463 Init
void si4463_init(void)
{
    cs_si4463_inactive();       //set pins initial state
    sdn_si4463_inactive();
    delay_cyc(1000000);
    
    sdn_si4463_active();        //reset the chip
    delay_cyc(1000000);
    sdn_si4463_inactive();
    
    uint8_t init_arr[] = RADIO_CONFIGURATION_DATA_ARRAY;    //array with init data, generated by WDS software
    uint8_t *p_init_arr = &init_arr[0];                     //pointer to the array
    
    //Send all commands while pointer not equal 0x00 (0x00 presented in the end of the configuration array)
    while(*p_init_arr != 0x00)
    {
        uint8_t len = *p_init_arr;      //command len (first byte of the each line in configuration array)
        p_init_arr++;                   //move pointer to the first data byte (which is next after the command len)
        
        si4463_wait_cts();              //check cts before any command
        cs_si4463_active();
        for(uint8_t i = 0; i < len; i++)
        {
            spi1_trx(*p_init_arr);      //send command byte
            p_init_arr++;               //move pointer to the next byte in command
        }
        cs_si4463_inactive();
    }

    //Set len of the packet
    si4463_set_packet_len(AIR_PACKET_LEN);

    //Get current settings
    p_settings = get_settings();

    //Set TX power
    si4463_set_tx_power(tx_power_register_values[p_settings->tx_power_opt]);
}



//Wait for hardware CTS pin
void si4463_wait_cts(void)
{
    while(!(GPIOA->IDR & GPIO_IDR_IDR12)){}      //while GPIO1 = 0
}



//Set air packet length
void si4463_set_packet_len(uint8_t packet_len)
{
    si4463_wait_cts();
    cs_si4463_active();
    spi1_trx(SI4463_CMD_SET_PROPERTY);
    spi1_trx(SI4463_PKT_FIELD_1_LENGTH_GROUP_ID);
    spi1_trx(SI4463_PKT_FIELD_1_LENGTH_NUM_PROPS);
    spi1_trx(SI4463_PKT_FIELD_1_LENGTH_START_PROP);
    spi1_trx(0x00);                 //packet len high byte
    spi1_trx(packet_len);           //packet len low byte
    cs_si4463_inactive();
}



void si4463_set_tx_power(uint8_t tx_pow_reg_val)
{
    si4463_wait_cts();
    cs_si4463_active();
    spi1_trx(SI4463_CMD_SET_PROPERTY);
    spi1_trx(SI4463_PA_PWR_LVL_GROUP_ID);
    spi1_trx(SI4463_PA_PWR_LVL_NUM_PROPS);
    spi1_trx(SI4463_PA_PWR_LVL_START_PROP);
    spi1_trx(tx_pow_reg_val);
    cs_si4463_inactive();
}



//SI4463 TX packet
void si4463_tx_packet(void)
{
    //reset TX FIFO
    si4463_wait_cts();
    cs_si4463_active();
    spi1_trx(SI4463_CMD_FIFO_INFO);
    spi1_trx(SI4463_FIFO_INFO_TX_BIT);
    cs_si4463_inactive();
    
    //fill TX FIFO buffer
    si4463_wait_cts();
    cs_si4463_active();
    spi1_trx(SI4463_CMD_WRITE_TX_FIFO);
    for (uint8_t i = 0; i < AIR_PACKET_LEN; i++)
    {
        spi1_trx(air_packet_tx[i]);
    }
    cs_si4463_inactive();
    
    //start TX
    si4463_wait_cts();
    cs_si4463_active();
    spi1_trx(SI4463_CMD_START_TX);
    spi1_trx(p_settings->freq_channel);                 //frequency channel
    spi1_trx(SI4463_SLEEP_STATE << 4);                  //return to sleep state, send FIFO content, start TX immediately
    spi1_trx(0x00);                                     //the number of data bytes to be transmitted
    spi1_trx(0x00);                                     //is specified by the value(s) of the PKT_FIELD_X_LENGTH properties
    cs_si4463_inactive();
}



//SI4463 start packet RX
void si4463_start_rx(void)
{
    si4463_wait_cts();
    cs_si4463_active();
    spi1_trx(SI4463_CMD_START_RX);
    spi1_trx(p_settings->freq_channel);                 //frequency channel
    spi1_trx(0x00);                                     //start RX immediately
    spi1_trx(0x00);                                     //RX packet len is specified
    spi1_trx(0x00);                                     //in packet handler configuration
    spi1_trx(SI4463_SLEEP_STATE);                       //if RX timeout then go to sleep (after RX_PREAMBLE_TIMEOUT)
    spi1_trx(SI4463_SLEEP_STATE);                       //if RX valid then go to sleep
    spi1_trx(SI4463_SLEEP_STATE);                       //if RX invalid then go to sleep
    cs_si4463_inactive();
}



//SI4463 get received packet
uint8_t si4463_get_rx_packet(void)
{
    uint8_t ph_pending = 0;
    
    //send get PH status command
    si4463_wait_cts();
    cs_si4463_active();
    spi1_trx(SI4463_CMD_GET_PH_STATUS);
    spi1_trx(SI4463_BYTE_DUMMY);                //also clear all pending interrupts
    cs_si4463_inactive();
    
    //retrieve response
    si4463_wait_cts();
    cs_si4463_active();
    spi1_trx(SI4463_CMD_READ_CMD_BUF);
    spi1_trx(SI4463_BYTE_DUMMY);                //skip CTS byte
    ph_pending = spi1_trx(SI4463_BYTE_DUMMY);
    cs_si4463_inactive();
    
    if (ph_pending & SI4463_PH_PEND_CRC_ERROR_BIT)
    {
        si4463_wait_cts();                      //clear RX FIFO content
        cs_si4463_active();
        spi1_trx(SI4463_CMD_FIFO_INFO);
        spi1_trx(SI4463_FIFO_INFO_RX_BIT);
        cs_si4463_inactive();
        
        si4463_wait_cts();                      //workaround if received packet has CRC error (see si4463 revB1 errata)
        cs_si4463_active();
        spi1_trx(SI4463_CMD_CHANGE_STATE);
        spi1_trx(SI4463_SLEEP_STATE);
        cs_si4463_inactive();
        
        return 0;
    }
    else if (ph_pending & SI4463_PH_PEND_PACKET_RX_BIT)
    {
        //read RX FIFO buffer
        si4463_wait_cts();
        cs_si4463_active();
        spi1_trx(SI4463_CMD_READ_RX_FIFO);
        for (uint8_t i = 0; i < AIR_PACKET_LEN; i++)
        {
        	air_packet_rx[i] = spi1_trx(SI4463_BYTE_DUMMY);
        }
        cs_si4463_inactive();
        
        return 1;
    }
    else
    {
        return 0;
    }
}



uint8_t *get_air_packet_tx(void)
{
	return &air_packet_tx[0];
}



uint8_t *get_air_packet_rx(void)
{
	return &air_packet_rx[0];
}
