/*
    ELEPH - Local Relative Navigation System <https://github.com/FeruzTopalov/eleph/>

	Copyright (C) 2021 Feruz Topalov
	Released under the GNU General Public License v3.0 <https://www.gnu.org/licenses/>
    
    file: rfm98.c
*/

#include "stm32f10x.h"
#include "rfm98.h"
#include "rfm98_config.h"
#include "gpio.h"
#include "spi.h"
#include "service.h"
#include "main.h"
#include "settings.h"



#define BASE_CHANNEL_FREQUENCY 			(433050000)
#define CHANNEL_FREQUENCY_STEP			(25000)
#define RFM98_CRYSTAL					(32000000)
#define POWER_2_TO_19					(524288)



#define AIR_PACKET_LEN      (17)        //bytes amount to tx/rx over air; does not include two bytes of CRC



union
{
    uint32_t as_uint32;
    uint8_t as_array[4];
} freq_reg_value;
float freq_reg_value_float;

uint8_t pa_conf_reg_value;
uint8_t pa_dac_reg_value;

uint8_t radio_irq_status = 0;
uint8_t air_packet_tx[AIR_PACKET_LEN];
uint8_t air_packet_rx[AIR_PACKET_LEN];
struct settings_struct *p_settings;



//RFM98 Init
void rfm98_init(void)
{
	spi1_clock_enable();

    cs_rfm98_inactive();       	//set pins initial state
    res_rfm98_inactive();
    delay_cyc(100000);
    
    res_rfm98_active();        	//reset the chip
    delay_cyc(100000);
    res_rfm98_inactive();
    delay_cyc(100000);

    uint8_t init_arr[] = RFM_CONF_ARRAY;    	//array with init data
    
    for (uint8_t i = 0; i < sizeof(init_arr); i += 2)
    {
        cs_rfm98_active();
        spi1_trx(init_arr[i]);      	//send command byte
        spi1_trx(init_arr[i + 1]);		//send value
        cs_rfm98_inactive();
    }

    //Get current settings
    p_settings = get_settings();

    //Set the packet len
    cs_rfm98_active();
	spi1_trx(REG_PAYLOADLENGTH | RFM_WRITE);
	spi1_trx(AIR_PACKET_LEN);
	cs_rfm98_inactive();

	//Set frequency
	freq_reg_value_float = ((float)BASE_CHANNEL_FREQUENCY + p_settings->freq_channel * CHANNEL_FREQUENCY_STEP) / ((float)RFM98_CRYSTAL / POWER_2_TO_19);
	freq_reg_value.as_uint32 = freq_reg_value_float;
    cs_rfm98_active();
	spi1_trx(REG_FRFMSB | RFM_WRITE);
	spi1_trx(freq_reg_value.as_array[2]);
	spi1_trx(freq_reg_value.as_array[1]);
	spi1_trx(freq_reg_value.as_array[0]);
	cs_rfm98_inactive();

	//Set TX power
	switch (p_settings->tx_power_opt)
	{
		case TX_POWER_1MILLIW_SETTING:
			pa_conf_reg_value = (RF_PACONFIG_PASELECT_PABOOST | RF_PACONFIG_MAX_POWER_7 | RF_PACONFIG_OUTPUTPOWER_0);
			pa_dac_reg_value = RF_PADAC_20DBM_OFF;
			break;

		case TX_POWER_10MILLIW_SETTING:
			pa_conf_reg_value = (RF_PACONFIG_PASELECT_PABOOST | RF_PACONFIG_MAX_POWER_7 | RF_PACONFIG_OUTPUTPOWER_8);
			pa_dac_reg_value = RF_PADAC_20DBM_OFF;
			break;

		case TX_POWER_50MILLIW_SETTING:
			pa_conf_reg_value = (RF_PACONFIG_PASELECT_PABOOST | RF_PACONFIG_MAX_POWER_7 | RF_PACONFIG_OUTPUTPOWER_15);
			pa_dac_reg_value = RF_PADAC_20DBM_OFF;
			break;

		case TX_POWER_100MILLIW_SETTING:
			pa_conf_reg_value = (RF_PACONFIG_PASELECT_PABOOST | RF_PACONFIG_MAX_POWER_7 | RF_PACONFIG_OUTPUTPOWER_15);
			pa_dac_reg_value = RF_PADAC_20DBM_ON;
			break;

		default:
			pa_conf_reg_value = (RF_PACONFIG_PASELECT_PABOOST | RF_PACONFIG_MAX_POWER_7 | RF_PACONFIG_OUTPUTPOWER_8);
			pa_dac_reg_value = RF_PADAC_20DBM_OFF;
			break;
	}

    cs_rfm98_active();
	spi1_trx(REG_PACONFIG | RFM_WRITE);
	spi1_trx(pa_conf_reg_value);
	cs_rfm98_inactive();

	cs_rfm98_active();
	spi1_trx(REG_PADAC | RFM_WRITE);
	spi1_trx(pa_dac_reg_value);
	cs_rfm98_inactive();

	//Calibrate IQ
	cs_rfm98_active();
	spi1_trx(REG_OPMODE | RFM_WRITE);
	spi1_trx(RF_OPMODE_LONGRANGEMODE_OFF | RF_OPMODE_MODULATIONTYPE_FSK | RF_OPMODE_STANDBY);
	cs_rfm98_inactive();

	cs_rfm98_active();
	spi1_trx(REG_IMAGECAL | RFM_WRITE);
	spi1_trx(RF_IMAGECAL_AUTOIMAGECAL_OFF | RF_IMAGECAL_IMAGECAL_START | RF_IMAGECAL_TEMPMONITOR_OFF);
	cs_rfm98_inactive();

	uint8_t cal_iq_running;
	do
	{
		delay_cyc(100);
		cs_rfm98_active();
		spi1_trx(REG_IMAGECAL | RFM_READ);
		cal_iq_running = spi1_trx(0) & RF_IMAGECAL_IMAGECAL_RUNNING;
		cs_rfm98_inactive();
	}
	while (cal_iq_running == RF_IMAGECAL_IMAGECAL_RUNNING);

	cs_rfm98_active();
	spi1_trx(REG_OPMODE | RFM_WRITE);
	spi1_trx(RF_OPMODE_LONGRANGEMODE_OFF | RF_OPMODE_MODULATIONTYPE_FSK | RF_OPMODE_SLEEP);
	cs_rfm98_inactive();

    spi1_clock_disable();
}



//RFM98 TX packet
uint8_t rfm98_tx_packet(void)
{
	spi1_clock_enable();

	//check current mode
	uint8_t mode_before_tx = 0;
	cs_rfm98_active();
	spi1_trx(REG_OPMODE | RFM_READ);
	mode_before_tx = spi1_trx(0);
	cs_rfm98_inactive();

	if (!((mode_before_tx & (uint8_t)(~RF_OPMODE_MASK)) & 0x06))	//Start TX only in SLEEP or STANDBY mode
	{
		//fill TX FIFO buffer
		cs_rfm98_active();
		spi1_trx(REG_FIFO | RFM_WRITE);
		for(uint8_t i = 0; i < AIR_PACKET_LEN; i++)
		{
			spi1_trx(air_packet_tx[i]);
		}
		cs_rfm98_inactive();

		//start TX
		cs_rfm98_active();
		spi1_trx(REG_SEQCONFIG1 | RFM_WRITE);
		spi1_trx(RF_SEQCONFIG1_SEQUENCER_START | RF_SEQCONFIG1_IDLEMODE_SLEEP | RF_SEQCONFIG1_FROMSTART_TOTX | RF_SEQCONFIG1_LPS_SEQUENCER_OFF | RF_SEQCONFIG1_FROMTX_TOLPS);
		cs_rfm98_inactive();

		spi1_clock_disable();
		return 1;				//successful TX start
	}
	else
	{
		spi1_clock_disable();
		return 0;
	}

    
}



//RFM98 start packet RX
uint8_t rfm98_start_rx(void)
{
	spi1_clock_enable();

	//check current mode
	uint8_t mode_before_rx = 0;
	cs_rfm98_active();
	spi1_trx(REG_OPMODE | RFM_READ);
	mode_before_rx = spi1_trx(0);
	cs_rfm98_inactive();

	if (!((mode_before_rx & (uint8_t)(~RF_OPMODE_MASK)) & 0x06))	//Start RX only in SLEEP or STANDBY mode
	{
		cs_rfm98_active();
		spi1_trx(REG_SEQCONFIG1 | RFM_WRITE);
		spi1_trx(RF_SEQCONFIG1_SEQUENCER_START | RF_SEQCONFIG1_IDLEMODE_SLEEP | RF_SEQCONFIG1_FROMSTART_TORX | RF_SEQCONFIG1_LPS_SEQUENCER_OFF);
		cs_rfm98_inactive();

		spi1_clock_disable();
		return 1;				//successful RX start
	}
	else
	{
		spi1_clock_disable();
		return 0;
	}

}



//RFM98 get interrupt status
uint8_t rfm98_get_irq_status(void)
{
	spi1_clock_enable();

	//check both flag bytes
	cs_rfm98_active();
	spi1_trx(REG_IRQFLAGS1 | RFM_READ);
	spi1_trx(0);
	radio_irq_status = spi1_trx(0);		//take only RegIrqFlags2
	cs_rfm98_inactive();

	spi1_clock_disable();

	return radio_irq_status;
}



//RFM98 get received packet
void rfm98_get_rx_packet(void)
{
	spi1_clock_enable();

	cs_rfm98_active();
	spi1_trx(REG_FIFO | RFM_READ);
	for (uint8_t i = 0; i < AIR_PACKET_LEN; i++)
	{
		air_packet_rx[i] = spi1_trx(0);
	}
	cs_rfm98_inactive();

	spi1_clock_disable();
}



void rfm98_flush_fifo(void)
{
	spi1_clock_enable();

	cs_rfm98_active();
	spi1_trx(REG_IRQFLAGS2 | RFM_WRITE);
	spi1_trx(RF_IRQFLAGS2_FIFOOVERRUN);		//Clear FIFO by writing overrun flag
	cs_rfm98_inactive();

	spi1_clock_disable();
}



uint8_t *get_air_packet_tx(void)
{
	return &air_packet_tx[0];
}



uint8_t *get_air_packet_rx(void)
{
	return &air_packet_rx[0];
}
