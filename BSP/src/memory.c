
#include "global.h"
#include "utility.h"
#include "string.h"
#include "spi.h"
#include "memory.h"
#include "stdlib.h"
#include "stdint.h"

/******************************************************************************************
W25Q32 FLASH 存储器特性:
1. 	总容量4MB，分64个64KB的块，每个块含16个4KB的扇区；
2. 	SPI 总线速度最高103MHz，但03号指令除外，03号指令最高50M；
3. 	主要指令包括读、快读、页面写、扇区擦除、块擦除、片擦除等；
4.	写数据第一字节20us，后续2.5us；整页写0.7ms；扇区擦除30ms，64K块擦
	除150ms；整片擦7s.

*******************************************************************************************/

/* Private typedef -----------------------------------------------------------*/
#define FLASH_PAGE_SIZE	256
#define FLASH_SECTOR_SIZE	0x1000
#define FLASH_BLOCK_SIZE	0x10000

#define FLASH_DATA_VALID_FLAG	0xAA		//写在地址最高字节

/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID 	0x90 
#define W25X_JedecDeviceID		0x9F 

#define WIP_Flag 0x01/* Write In Progress (WIP) flag */

#define DUMMY_BYTE	0xFF

#define FLASH_CS_LOW() 		GPIO_ResetBits(FLASH_CS_PORT, FLASH_CS_PIN)
#define FLASH_CS_HIGH()		GPIO_SetBits(FLASH_CS_PORT, FLASH_CS_PIN)


FLASH_QUEUE TagRecordFlashQueue;
TEMP_FLASH_QUEUE FlashTempQueue;

void Flash_PowerDown(void) 
{ 
	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_PowerDown);
	FLASH_CS_HIGH();
} 

void Flash_WAKEUP(void) 
{
	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_ReleasePowerDown);
	FLASH_CS_HIGH();
} 

void Flash_WRSR(unsigned char sesr) 
{
	FLASH_CS_LOW();
	Flash_SpiWrite(0x01);
	Flash_SpiWrite(sesr);
	FLASH_CS_HIGH(); 
} 

u8 Flash_SpiRead(void)
{
	return (FLASH_SPI(DUMMY_BYTE));
}

u8 Flash_SpiWrite(u8 byte)
{
	/*#if USING_MCU405
	if(Read_Power_DetectPin() )
	{
		printf("Power Off!\r\n");
		return 0;
	}
	#endif*/
	return FLASH_SPI(byte);
}

void Flash_Init(void)
{
	FLASH_CS_HIGH();
}

void Flash_SectorEraseAdd(u32 dst_addr)
{
	Flash_WriteEnable();
	Flash_WaitForWriteEnd();
	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_SectorErase);
	Flash_SpiWrite((dst_addr & 0xFF0000) >> 16);
	Flash_SpiWrite((dst_addr & 0xFF00) >> 8);
	Flash_SpiWrite(dst_addr & 0xFF);
	FLASH_CS_HIGH();
	Flash_WaitForWriteEnd();
}

void Flash_SectorErase(unsigned int dst_addr)
{
	Flash_WriteEnable();
	Flash_WaitForWriteEnd();
	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_SectorErase);
	Flash_SpiWrite((dst_addr & 0xFF0000) >> 16);
	Flash_SpiWrite((dst_addr & 0xFF00) >> 8);
	Flash_SpiWrite(dst_addr & 0xFF);
	FLASH_CS_HIGH();
	Flash_WaitForWriteEnd();
}

void Flash_BlockErase(unsigned int dst_addr)
{
	Flash_WriteEnable();
	Flash_WaitForWriteEnd();
	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_BlockErase);
	Flash_SpiWrite((dst_addr & 0xFF0000) >> 16);
	Flash_SpiWrite((dst_addr & 0xFF00) >> 8);
	Flash_SpiWrite(dst_addr & 0xFF);
	FLASH_CS_HIGH();
	Flash_WaitForWriteEnd();	
}

void Flash_ChipErase(void)
{
	Flash_WriteEnable();
	Flash_WaitForWriteEnd();
	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_ChipErase);
	FLASH_CS_HIGH();
	Flash_WaitForWriteEnd();
}

void Flash_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	Flash_WriteEnable();
	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_PageProgram);
	Flash_SpiWrite((WriteAddr & 0xFF0000) >> 16);
	Flash_SpiWrite((WriteAddr & 0xFF00) >> 8);
	Flash_SpiWrite(WriteAddr & 0xFF);
	if(NumByteToWrite > FLASH_PAGE_SIZE)
	{
		NumByteToWrite = FLASH_PAGE_SIZE;
	}
	while (NumByteToWrite--)
	{
		Flash_SpiWrite(*pBuffer);
		pBuffer++;
	}
	FLASH_CS_HIGH();
	Flash_WaitForWriteEnd();
}


//这个函数是有问题的，如: 250地址写10个字节就出错了!!!!   gaojun
void Flash_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)		
{
	u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = WriteAddr % FLASH_PAGE_SIZE;
	count = FLASH_PAGE_SIZE - Addr;
	NumOfPage =NumByteToWrite / FLASH_PAGE_SIZE;
	NumOfSingle = NumByteToWrite % FLASH_PAGE_SIZE;

	if (Addr == 0) 
	{
		if (NumOfPage == 0) 
		{
			Flash_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
		}
		else 
		{
			while (NumOfPage--)
			{
				Flash_PageWrite(pBuffer, WriteAddr, FLASH_PAGE_SIZE);
				WriteAddr +=FLASH_PAGE_SIZE;
				pBuffer += FLASH_PAGE_SIZE;
			}

			Flash_PageWrite(pBuffer, WriteAddr, NumOfSingle);
		}
	}
	else 
	{
		if (NumOfPage == 0) 
		{
			if (NumOfSingle > count) 
			{
				temp = NumOfSingle - count;

				Flash_PageWrite(pBuffer, WriteAddr, count);
				WriteAddr +=count;
				pBuffer += count;

				Flash_PageWrite(pBuffer, WriteAddr, temp);
			}
			else
			{
				Flash_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
			}
		}
		else 
		{
			NumByteToWrite -= count;
			NumOfPage =NumByteToWrite / FLASH_PAGE_SIZE;
			NumOfSingle = NumByteToWrite % FLASH_PAGE_SIZE;

			Flash_PageWrite(pBuffer, WriteAddr, count);
			WriteAddr +=count;
			pBuffer += count;

			while (NumOfPage--)
			{
				Flash_PageWrite(pBuffer, WriteAddr, FLASH_PAGE_SIZE);
				WriteAddr +=FLASH_PAGE_SIZE;
				pBuffer += FLASH_PAGE_SIZE;
			}

			if (NumOfSingle != 0)
			{
				Flash_PageWrite(pBuffer, WriteAddr, NumOfSingle);
			}
		}
	}
}

void Flash_Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)		//不能跨256 字节页写!!!!!!!!!  GAOJUN 
{
	unsigned char i=0;
	
	//DisableIrq();
	Flash_WriteEnable();
	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_PageProgram);
	Flash_SpiWrite((WriteAddr & 0xFF0000) >> 16);
	Flash_SpiWrite((WriteAddr & 0xFF00) >> 8);
	Flash_SpiWrite(WriteAddr & 0xFF);
	for(i=0;i<NumByteToWrite;i++)
	{
		Flash_SpiWrite(pBuffer[i]);
	}
	FLASH_CS_HIGH();

	Flash_WaitForWriteEnd(); 
	
	//EnableIrq();
}

void Flash_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	u16 i;
	
	//DisableIrq();
	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_ReadData);
	Flash_SpiWrite((ReadAddr & 0xFF0000) >> 16);
	Flash_SpiWrite((ReadAddr& 0xFF00) >> 8);
	Flash_SpiWrite(ReadAddr & 0xFF);

	for(i = 0;i < NumByteToRead;i++)
	{
		pBuffer[i] = Flash_SpiWrite(DUMMY_BYTE);
	}
	FLASH_CS_HIGH();
	
	//EnableIrq();
}


void Flash_Read(u8* pBuffer)
{
	u16 i;
	
	//DisableIrq();
	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_ReadData);

	for(i = 0;i < 12;i++)
	{
		pBuffer[i] = Flash_SpiWrite(DUMMY_BYTE);
	}
	FLASH_CS_HIGH();
	
	//EnableIrq();
}

void Flash_WriteEnable(void)
{
	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_WriteEnable);
	FLASH_CS_HIGH();
}

void Flash_WaitForWriteEnd(void)
{
	u8 FlashStatus = 0;
	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_ReadStatusReg);
	do
	{
		FlashStatus = Flash_SpiWrite(DUMMY_BYTE);	 
		FeedDog();
	}
	while ((FlashStatus & 0x01) == 0x01); 
	FLASH_CS_HIGH();
}

/************************************************************************************************
	寻找从起始地址开始，第一个是0xFF的地址
************************************************************************************************/
uint32_t Flash_FindLastData(uint32_t start_addr)
{
	uint32_t i;
	uint8_t dat;
//	uint8_t n;

	FLASH_CS_LOW();
	Flash_SpiWrite(W25X_ReadData);
	Flash_SpiWrite((start_addr & 0xFF0000) >> 16);
	Flash_SpiWrite((start_addr& 0xFF00) >> 8);
	Flash_SpiWrite(start_addr & 0xFF);

	for(i = 0;i < FLASH_SECTOR_SIZE*4;i++)
	{
		dat = Flash_SpiWrite(DUMMY_BYTE);
		if((i % 16)==0)		//四字节的最高字节如果是0xFF，就是空位
		{
			if(dat == 0xFF)
				break;
			else if(dat != FLASH_DATA_VALID_FLAG)
			{
				i = 1;	//返回无效值
				break;
			}
		}
	}
	FLASH_CS_HIGH();

	return i;
}

void Flash_EraseData(uint32_t start_addr, uint32_t size)
{
	unsigned short i = 0;
	for(i=0; i< size; i++)
	{
		FeedDog();
		Flash_SectorErase(start_addr+i*FLASH_SECTOR_SIZE);
	}
}

/***************************************************************************************
将数量、队尾存于FLASH中，并计算出队首

*******************************************************************************************/
void FlashQueue_SaveParam(FLASH_QUEUE *p_FlashQueue)
{
	unsigned int addr;
	unsigned char buf[16];
	
	if(p_FlashQueue->param_addr_index >= 1024)
	{
		Flash_SectorErase(p_FlashQueue->flash_addr_param);
		Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SECTOR_SIZE);
		Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SECTOR_SIZE*2);
		Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SECTOR_SIZE*3);
		p_FlashQueue->param_addr_index = 0;
	}	
	addr = p_FlashQueue->flash_addr_param + (p_FlashQueue->param_addr_index)*16;	//数量存储地址
	buf[0] = 0xAA;//(p_FlashQueue->cnt)>>24;
	buf[1] = (p_FlashQueue->cnt)>>16;
	buf[2] = (p_FlashQueue->cnt)>>8;
	buf[3] = (p_FlashQueue->cnt)&0xFF;

	buf[4] = 0xAA;
	buf[5] = (p_FlashQueue->tail_addr)>>16; 
	buf[6] = (p_FlashQueue->tail_addr)>>8; 
	buf[7] = (p_FlashQueue->tail_addr)&0xFF;

	buf[8] = 0xAA;
	buf[9] = (p_FlashQueue->head_addr)>>16;
	buf[10] = (p_FlashQueue->head_addr)>>8;
	buf[11] = (p_FlashQueue->head_addr)&0xFF;

	Flash_Write(buf,addr,16);
	//Flash_BufferRead(buf,addr,16);

	p_FlashQueue->param_addr_index++;	
	
//	printf(">> Queue tail:%08X,head:%08X,cnt:%04X\r\n",TagRecordFlashQueue.tail_addr,TagRecordFlashQueue.head_addr,TagRecordFlashQueue.cnt);	
}
/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////
void FlashQueue_GetParam(FLASH_QUEUE *p_FlashQueue)
{
	uint32_t i;
	uint8_t buf[16];

	i = Flash_FindLastData(p_FlashQueue->flash_addr_param);
	if((i%16)!= 0)	//不是12 的整数倍，数据错乱了
	{
		Flash_SectorErase(p_FlashQueue->flash_addr_param);
		Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SECTOR_SIZE);
		Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SECTOR_SIZE*2);
		Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SECTOR_SIZE*3);
		i = 0;		
	}
	if(i == 0)
	{
		p_FlashQueue->param_addr_index = 0;
		p_FlashQueue->cnt = 0;
		p_FlashQueue->head_addr = p_FlashQueue->flash_addr_start;
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
		Flash_SectorErase(p_FlashQueue->flash_addr_start);
		return;
	}
	
	
	p_FlashQueue->param_addr_index = i/16;

	i += p_FlashQueue->flash_addr_param;	//指向下最后一个数据的下一个空位地址
	i -= 16;							//数量存储地址
	Flash_BufferRead(buf, i, 16);
	p_FlashQueue->cnt = 0;//(buf[0]<<24);
	p_FlashQueue->cnt += (buf[1]<<16);
	p_FlashQueue->cnt += (buf[2]<<8);
	p_FlashQueue->cnt += (buf[3]);

	p_FlashQueue->tail_addr = 0;//(buf[0]<<24);
	p_FlashQueue->tail_addr += (buf[5]<<16);
	p_FlashQueue->tail_addr += (buf[6]<<8);
	p_FlashQueue->tail_addr += (buf[7]);	

	p_FlashQueue->head_addr = 0;//(buf[0]<<24);
	p_FlashQueue->head_addr += (buf[9]<<16);
	p_FlashQueue->head_addr += (buf[10]<<8);
	p_FlashQueue->head_addr += (buf[11]);	

	i = p_FlashQueue->tail_addr + (p_FlashQueue->cnt)*(p_FlashQueue->data_len);
	if(i >=  p_FlashQueue->flash_addr_end)		//超出范围，则回头
	{
		i -=  p_FlashQueue->flash_addr_end;		//超出部分
		i += p_FlashQueue->flash_addr_start;		//折回到FLASH起始部分去
	}
	if(i != p_FlashQueue->head_addr)										//验证数据的有效性
	{
		printf("!! Queue tail:%08X,head:%08X,cnt:%04X\r\n",TagRecordFlashQueue.tail_addr,TagRecordFlashQueue.head_addr,TagRecordFlashQueue.cnt);
		printf("!! Index:%08X.\r\n",TagRecordFlashQueue.param_addr_index);
		Flash_SectorErase(p_FlashQueue->flash_addr_param);
		Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SECTOR_SIZE);
		Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SECTOR_SIZE*2);
		p_FlashQueue->param_addr_index = 0;
		p_FlashQueue->cnt = 0;
		p_FlashQueue->head_addr = p_FlashQueue->flash_addr_start;
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
		Flash_SectorErase(p_FlashQueue->flash_addr_start);
		printf(">> Flash Data Error!!!\r\n");
		
	}
}

void FlashQueue_ClearQueue(FLASH_QUEUE *p_FlashQueue)
{
	Flash_SectorErase(p_FlashQueue->flash_addr_param);
	Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SECTOR_SIZE);
	Flash_SectorErase(p_FlashQueue->flash_addr_param+FLASH_SECTOR_SIZE*2);

	p_FlashQueue->param_addr_index = 0;
	p_FlashQueue->cnt = 0;
	p_FlashQueue->head_addr = p_FlashQueue->flash_addr_start;
	p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
	Flash_SectorErase(p_FlashQueue->flash_addr_start);	
}

unsigned char FlashQueue_IsFull(FLASH_QUEUE *p_FlashQueue)
{

	if(p_FlashQueue->cnt >= p_FlashQueue->max_cnt)
		return 1;
	else
		return 0;
}

unsigned char FlashQueue_IsEmpty(FLASH_QUEUE *p_FlashQueue)
{
	if(p_FlashQueue->cnt  == 0)
		return 1;
	else
		return 0;
}

void FlashQueue_EraseTail(FLASH_QUEUE *p_FlashQueue)
{
	unsigned int cnt;
	Flash_SectorErase(p_FlashQueue->tail_addr);		//擦除队尾所在的扇区

	cnt = (FLASH_SECTOR_SIZE - ((p_FlashQueue->tail_addr)&(FLASH_SECTOR_SIZE-1)))/(p_FlashQueue->data_len);	//被擦掉的数量
																	
	p_FlashQueue->tail_addr = ((p_FlashQueue->tail_addr)&(~(FLASH_SECTOR_SIZE-1)))+FLASH_SECTOR_SIZE;
													//队尾地址移到下一扇区起始地址
	if(p_FlashQueue->tail_addr >= p_FlashQueue->flash_addr_end)
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;

	p_FlashQueue->cnt -= cnt;

	FlashQueue_SaveParam(p_FlashQueue);
}

unsigned char FlashQueue_EnQueue(FLASH_QUEUE *p_FlashQueue,unsigned char *dat)
{
	if(FlashQueue_IsFull(p_FlashQueue))
	{
		FlashQueue_EraseTail(p_FlashQueue);
	}

	Flash_Write(dat,p_FlashQueue->head_addr,p_FlashQueue->data_len);
	p_FlashQueue->head_addr += p_FlashQueue->data_len;
	if(p_FlashQueue->head_addr >= p_FlashQueue->flash_addr_end)
		p_FlashQueue->head_addr = p_FlashQueue->flash_addr_start;

	if(((p_FlashQueue->head_addr) & (FLASH_SECTOR_SIZE-1)) == 0)		//处于一个新的扇区开始  0xfff
		Flash_SectorErase(p_FlashQueue->head_addr);					//擦除扇区
	p_FlashQueue->cnt++;

	FlashQueue_SaveParam(p_FlashQueue);
	return 0;
}

unsigned char FlashQueue_GetTail(FLASH_QUEUE *p_FlashQueue,unsigned char *dat)
{
	if(FlashQueue_IsEmpty(p_FlashQueue))
		return 0;
	Flash_BufferRead(dat,p_FlashQueue->tail_addr,p_FlashQueue->data_len);
	return 1;
}

unsigned char FlashQueue_DelTail(FLASH_QUEUE *p_FlashQueue)
{
	if(FlashQueue_IsEmpty(p_FlashQueue))
		return 0;
	p_FlashQueue->tail_addr+=p_FlashQueue->data_len;
	if(p_FlashQueue->tail_addr >= p_FlashQueue->flash_addr_end)
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
	p_FlashQueue->cnt--;
	FlashQueue_SaveParam(p_FlashQueue);
	return 1;
}

unsigned char FlashQueue_DeQueue(FLASH_QUEUE *p_FlashQueue,unsigned char *dat)
{
	if(FlashQueue_IsEmpty(p_FlashQueue))
		return 0;
	Flash_BufferRead(dat,p_FlashQueue->tail_addr,p_FlashQueue->data_len);
	
	p_FlashQueue->tail_addr+=p_FlashQueue->data_len;
	if(p_FlashQueue->tail_addr >= p_FlashQueue->flash_addr_end)
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
	p_FlashQueue->cnt--;
	FlashQueue_SaveParam(p_FlashQueue);
	return 1;
}

unsigned char FlashQueue_DeQueue_NoSave(FLASH_QUEUE *p_FlashQueue,unsigned char *dat)
{
	if(FlashQueue_IsEmpty(p_FlashQueue))
		return 0;
	Flash_BufferRead(dat,p_FlashQueue->tail_addr,p_FlashQueue->data_len);
	
	p_FlashQueue->tail_addr+=p_FlashQueue->data_len;
	if(p_FlashQueue->tail_addr >= p_FlashQueue->flash_addr_end)
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
	p_FlashQueue->cnt--;
	return 1;
}


#define BIT_FLASH_TAG_CHECKSUM    15
#define LEN_FLASH_TAG_CHECKSUM    15
#define CNT_FLASH_TAG_CHECKESUM   5

//校验flash数据 失败多次就重度 
bool FlashQueue_CheckTagData(FLASH_QUEUE *p_FlashQueue, u8_t *dat, int nTryTime)
{
	BOOL bRet = FALSE;
	int i = 0;
	
	nTryTime = (nTryTime > CNT_FLASH_TAG_CHECKESUM) ? CNT_FLASH_TAG_CHECKESUM : nTryTime;
	
	for (i=0 ;i<nTryTime; i++)
	{
		if (dat[BIT_FLASH_TAG_CHECKSUM]  == CheckSum(dat,15))
		{
			bRet = TRUE;
			break;
		}
		FlashQueue_GetTagData(p_FlashQueue, dat);
	}
	
	//多次读取失败
	if (i == nTryTime)
	{
		//do thing
		
	}
	
	return bRet;
}


unsigned char FlashQueue_GetTagData(FLASH_QUEUE *p_FlashQueue,unsigned char *dat)
{
	if(FlashQueue_IsEmpty(p_FlashQueue))
		return 0;
	
	Flash_BufferRead(dat,p_FlashQueue->tail_addr,p_FlashQueue->data_len);
	
	//hcp 2015-10-21
//	p_FlashQueue->tail_addr+=p_FlashQueue->data_len;
//	if(p_FlashQueue->tail_addr >= p_FlashQueue->flash_addr_end)
//		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
//	p_FlashQueue->cnt--;
	
	return 1;
}

//hcp 2015-10-21
void FlashQueue_FinTagGet(FLASH_QUEUE *p_FlashQueue)
{
	p_FlashQueue->tail_addr+=p_FlashQueue->data_len;
	if(p_FlashQueue->tail_addr >= p_FlashQueue->flash_addr_end)
		p_FlashQueue->tail_addr = p_FlashQueue->flash_addr_start;
	p_FlashQueue->cnt--;
}

//hcp 修改 copy 函数  temp flash 相关操作
void FlashQueue_Copy(TEMP_FLASH_QUEUE * p_FlashQueueDis,FLASH_QUEUE *p_FlashQueueSrc)
{
	p_FlashQueueDis->flash_addr_start 	= p_FlashQueueSrc->flash_addr_start;
	p_FlashQueueDis->flash_addr_end 		= p_FlashQueueSrc->flash_addr_end;
	p_FlashQueueDis->flash_addr_param 	= p_FlashQueueSrc->flash_addr_param;
	p_FlashQueueDis->data_len 			= p_FlashQueueSrc->data_len;
	p_FlashQueueDis->max_cnt 			= p_FlashQueueSrc->max_cnt;
	p_FlashQueueDis->cnt 				= p_FlashQueueSrc->cnt;
	p_FlashQueueDis->tail_addr 			= p_FlashQueueSrc->tail_addr;
	p_FlashQueueDis->head_addr 			= p_FlashQueueSrc->head_addr;
	p_FlashQueueDis->param_addr_index	= p_FlashQueueSrc->param_addr_index;
	
	//相关标记备份 hcp 2015-10-17
	p_FlashQueueDis->bk_cnt = p_FlashQueueSrc->cnt;
	p_FlashQueueDis->bk_tail_addr = p_FlashQueueSrc->tail_addr;
	
}

void Flash_TagRecordQueueInit(void)
{
	TagRecordFlashQueue.flash_addr_start = FLASH_ADDR_RECORD_DATA;
	TagRecordFlashQueue.flash_addr_end = FLASH_ADDR_RECORD_DATA + FLASH_RECORD_MAX_SIZE;
	TagRecordFlashQueue.flash_addr_param = FLASH_ADDR_RECORD_CNT;
	TagRecordFlashQueue.data_len = RECORD_UNIT_LEN;
	TagRecordFlashQueue.max_cnt = (FLASH_RECORD_MAX_SIZE-FLASH_SECTOR_SIZE)/RECORD_UNIT_LEN;
	FlashQueue_GetParam(&TagRecordFlashQueue);
	printf(">>Queue tail:%08X,head:%08X,cnt:%04X\r\n",TagRecordFlashQueue.tail_addr,TagRecordFlashQueue.head_addr,TagRecordFlashQueue.cnt);
}

void Flash_PrintTagRecordQueueInit(FLASH_QUEUE *p_FlashQueue)
{
	p_FlashQueue->flash_addr_start = FLASH_ADDR_RECORD_DATA;
	p_FlashQueue->flash_addr_end = FLASH_ADDR_RECORD_DATA + FLASH_RECORD_MAX_SIZE;
	p_FlashQueue->flash_addr_param = FLASH_ADDR_RECORD_CNT;
	p_FlashQueue->data_len = RECORD_UNIT_LEN;
	p_FlashQueue->max_cnt = (FLASH_RECORD_MAX_SIZE-FLASH_SECTOR_SIZE)/RECORD_UNIT_LEN;
	p_FlashQueue->cnt = TagRecordFlashQueue.max_cnt;
	p_FlashQueue->tail_addr = 0;
	p_FlashQueue->head_addr = 0;
}

