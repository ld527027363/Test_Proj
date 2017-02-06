/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#include "si446x_bsp.h"
#include "main.h"
#include "utility.h"

/*****************************************************************************
 *  Local Macros & Definitions
 *****************************************************************************/

/*****************************************************************************
 *  Global Variables
 *****************************************************************************/
//SEGMENT_VARIABLE(Radio_Configuration_Data_Array[], U8, SEG_CODE) = \
//              RADIO_CONFIGURATION_DATA_ARRAY;

U8 Radio_Configuration_Data_Array[] = RADIO_CONFIGURATION_DATA_ARRAY;
//const SEGMENT_VARIABLE(RadioConfiguration, tRadioConfiguration, SEG_CODE) = \
//                        RADIO_CONFIGURATION_DATA;	
const tRadioConfiguration RadioConfiguration = RADIO_CONFIGURATION_DATA;

//const SEGMENT_VARIABLE_SEGMENT_POINTER(pRadioConfiguration, tRadioConfiguration, SEG_CODE, SEG_CODE) = \
 //                       &RadioConfiguration;
const tRadioConfiguration * pRadioConfiguration =  &RadioConfiguration;
int g_iRssiValue = 0;
unsigned char g_uRssiByte;

/*****************************************************************************
 *  Local Function Declarations
 *****************************************************************************/
void vRadio_PowerUp(void);

/*!
 *  Power up the Radio.
 *
 *  @note
 *
 */
void vRadio_PowerUp(void)
{
	//SEGMENT_VARIABLE(wDelay,  U16, SEG_XDATA) = 0u;
	U16 wDelay= 0;
  /* Hardware reset the chip */
	si446x_reset();

  /* Wait until reset timeout or Reset IT signal */
	for (; wDelay < pRadioConfiguration->Radio_Delay_Cnt_After_Reset; wDelay++);
}

/*!
 *  Radio Initialization.
 *
 *  @author Sz. Papp
 *
 *  @note
 *
 */
void vRadio_Init(void)
{
	/* Power Up the radio chip */
    vRadio_PowerUp(); 
	/* Load radio configuration */
	while (SI446X_SUCCESS != si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
	{
		Delay_ms(10); 
		vRadio_PowerUp();
	}
	// Read ITs, clear pending ones  
	si446x_get_int_status(0u, 0u, 0u);
}

/*!
 *  Check if Packet sent IT flag is pending.
 *
 *  @return   TRUE / FALSE
 *
 *  @note
 *
 */
U8 gRadio_CheckTransmitted(void)
{
	U8 TEMP_data = 0;
	TEMP_data = RF433_SPI_IRQ();
	if (TEMP_data == FALSE)
	{
    /* Read ITs, clear pending ones */
		si446x_get_int_status(0u, 0u, 0u);

    /* check the reason for the IT */
		if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*!
 *  Set Radio to TX mode, fixed packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent
 *
 *  @note
 *
 */
void  vRadio_StartTx(U8 channel, U8 len,U8 *pioFixRadioPacket)
{
  /* Reset TX FIFO */
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT);

  // Read ITs, clear pending ones
	si446x_get_int_status(0u, 0u, 0u);

  /* Fill the TX fifo with datas */
	si446x_write_tx_fifo(len, pioFixRadioPacket);

  /* Start sending packet, channel 0, START immediately, Packet length according to PH, go READY when done */
	si446x_start_tx(channel, 0x30,  0x00);
	//si446x_start_tx(channel, 0x30,  len);
}



/*!
 *  Check if Packet received IT flag is pending.
 *
 *  @return   TRUE - Packet successfully received / FALSE - No packet pending.
 *
 *  @note
 *
 */
BIT gRadio_CheckReceived(U8 *rx_buf,U8 rcv_len)
{

	if (RF433_SPI_IRQ() == 0)
	{
        
		

		/* check the reason for the IT */
		if (Si446xCmd.GET_INT_STATUS.MODEM_PEND & SI446X_CMD_GET_INT_STATUS_REP_SYNC_DETECT_BIT)
		{
			/* Blink once LED2 to show Sync Word detected */
			g_iRssiValue = radio_get_rssi();
		}
		si446x_get_int_status(0u, 0u, 0u);
		if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT)
		{

			/* Packet RX */
			si446x_read_rx_fifo(rcv_len, rx_buf);

			return TRUE;
		}

		if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_CRC_ERROR_BIT)
		{
			/* Reset FIFO */
			si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
		}
	}

	return FALSE;
}

/************************************************************************************************
?D??¡ä|¨¤¨ª
¡¤¦Ì??0 : ?T¨º??t
¡¤¦Ì?? 1: ¡¤¡é?¨ª¨ª¨º3¨¦
¡¤¦Ì??2: ?¨®¨º?¨ª¨º3¨¦
************************************************************************************************/
unsigned char gRadio_IntHandle(U8 *rx_buf,U8 rcv_len)
{
	static unsigned char rssi;
	if (RF433_SPI_IRQ() == 0)
	{
	//	Delay_10us(5);
					
		/* check the reason for the IT */
		if (Si446xCmd.GET_INT_STATUS.MODEM_PEND & SI446X_CMD_GET_INT_STATUS_REP_SYNC_DETECT_BIT)
		{
			/* Blink once LED2 to show Sync Word detected */
			rssi = radio_get_rssi_byte();
		}
		si446x_get_int_status(0u, 0u, 0u);	
		if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_BIT)
		{
			return 1;
		}
		if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT)
		{

			/* Packet RX */
			si446x_read_rx_fifo(rcv_len, rx_buf);
			rx_buf[rcv_len] = rssi;
			return 2;
		}

		if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_CRC_ERROR_BIT)
		{
			/* Reset FIFO */
			si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
			return 0;
		}		
	}
	return 0;
}


/*!
 *  Set Radio to RX mode, fixed packet length.
 *
 *  @param channel Freq. Channel
 *
 *  @note
 *
 */
void vRadio_StartRX(U8 channel,U8 rcv_len)
{
  // Read ITs, clear pending ones
	si446x_get_int_status(0u, 0u, 0u);
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT); 
  /* Start Receiving packet, channel 0, START immediately, Packet length according to PH */
	si446x_start_rx(channel, 0u, rcv_len,
                  SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_RX,
                  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX );
}
