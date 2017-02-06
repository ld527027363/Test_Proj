/*!
 * File:
 *  radio_comm.h
 *
 * Description:
 *  This file contains the RADIO communication layer.
 *
 * Silicon Laboratories Confidential
 * Copyright 2012 Silicon Laboratories, Inc.
 */

#include "si446x_bsp.h"
#include "main.h"

U8 ctsWentHigh = 0;


/*!
 * Gets a command response from the radio chip
 *
 * @param byteCount     Number of bytes to get from the radio chip
 * @param pData         Pointer to where to put the data
 *
 * @return CTS value
 */
U8 radio_comm_GetResp(U8 byteCount, U8* pData)
{

	U8 ctsVal=0;
	U16 errCnt = RADIO_CTS_TIMEOUT;
	while (errCnt >0 )      //wait until radio IC is ready with the data
	{
		radio_hal_ClearNsel();
//		Delay_us(1);
        
		radio_hal_SpiWriteByte(0x44);    //read CMD buffer
		ctsVal = radio_hal_SpiReadByte();
		if (ctsVal == 0xFF)
		{
			if (byteCount)
			{
				radio_hal_SpiReadData(byteCount, pData);
			}
	//		Delay_us(1);
			radio_hal_SetNsel();
			break;
		}
	//	Delay_us(1);
		radio_hal_SetNsel();
		errCnt--;
	}
	if (errCnt == 0)
	{
		while(1)
		{
			return 9;
      /* ERROR!!!!  CTS should never take this long. */
		}
	}

	if (ctsVal == 0xFF)
	{
		ctsWentHigh = 1;
	}

	return ctsVal;
}


/*!
 * Sends a command to the radio chip
 *
 * @param byteCount     Number of bytes in the command to send to the radio device
 * @param pData         Pointer to the command to send.
 */
void radio_comm_SendCmd(U8 byteCount, U8* pData)
{
    /* There was a bug in A1 hardware that will not handle 1 byte commands. 
       It was supposedly fixed in B0 but the fix didn't make it at the last minute, so here we go again */
	if (byteCount == 1)
		byteCount++;
	while (!ctsWentHigh)
	{
		radio_comm_PollCTS();
	}
	radio_hal_ClearNsel();
	radio_hal_SpiWriteData(byteCount, pData);
	radio_hal_SetNsel();
	ctsWentHigh = 0;
}

/*!
 * Gets a command response from the radio chip
 *
 * @param cmd           Command ID
 * @param pollCts       Set to poll CTS
 * @param byteCount     Number of bytes to get from the radio chip.
 * @param pData         Pointer to where to put the data.
 */
void radio_comm_ReadData(U8 cmd, bit pollCts, U8 byteCount, U8* pData)
{
	if(pollCts)
	{
		while(!ctsWentHigh)
		{
			radio_comm_PollCTS();
		}
	}
	radio_hal_ClearNsel();
	radio_hal_SpiWriteByte(cmd);
	radio_hal_SpiReadData(byteCount, pData);
	radio_hal_SetNsel();
	ctsWentHigh = 0;
}


/*!
 * Gets a command response from the radio chip
 *
 * @param cmd           Command ID
 * @param pollCts       Set to poll CTS
 * @param byteCount     Number of bytes to get from the radio chip
 * @param pData         Pointer to where to put the data
 */
void radio_comm_WriteData(U8 cmd, bit pollCts, U8 byteCount, U8* pData)
{
	if(pollCts)
	{
		while(!ctsWentHigh)
		{
			radio_comm_PollCTS();
		}
	}
	radio_hal_ClearNsel();
	radio_hal_SpiWriteByte(cmd);
	radio_hal_SpiWriteData(byteCount, pData);
	radio_hal_SetNsel();
	ctsWentHigh = 0;
}

/*!
 * Waits for CTS to be high
 *
 * @return CTS value
 */
U8 radio_comm_PollCTS(void)
{
#ifdef RADIO_USER_CFG_USE_GPIO1_FOR_CTS
	while(!radio_hal_Gpio1Level())
	{
        /* Wait...*/
	}
	ctsWentHigh = 1;
	return 0xFF;
#else
    //printf("5555");
	return radio_comm_GetResp(0, 0);
#endif
}

/**
 * Clears the CTS state variable.
 */
void radio_comm_ClearCTS()
{
	ctsWentHigh = 0;
}

/*!
 * Sends a command to the radio chip and gets a response
 *
 * @param cmdByteCount  Number of bytes in the command to send to the radio device
 * @param pCmdData      Pointer to the command data
 * @param respByteCount Number of bytes in the response to fetch
 * @param pRespData     Pointer to where to put the response data
 *
 * @return CTS value
 */
U8 radio_comm_SendCmdGetResp(U8 cmdByteCount, U8* pCmdData, U8 respByteCount, U8* pRespData)
{
	radio_comm_SendCmd(cmdByteCount, pCmdData);
	return radio_comm_GetResp(respByteCount, pRespData);
}

int  radio_get_rssi(void)
{
	unsigned char bTemp = 0;
	int iRet = 0;
	radio_hal_ClearNsel();
	radio_hal_SpiWriteByte(0x50);
	radio_hal_SpiReadData(1, &bTemp);
	radio_hal_SetNsel();
	iRet = bTemp/2 - 116;
	return iRet;
}

unsigned char  radio_get_rssi_byte(void)
{
	unsigned char temp = 0;
	radio_hal_ClearNsel();
	radio_hal_SpiWriteByte(0x50);
	radio_hal_SpiReadData(1, &temp);
	radio_hal_SetNsel();
	return temp;
}
