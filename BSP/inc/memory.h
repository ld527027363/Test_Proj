#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>
#include <stdbool.h>

#define FLASH_ADDR_RECORD_CNT			(1024*1024)				// �ܼ�¼�����洢��ַ
#define FLASH_ADDR_RECORD_DATA			0
#define FLASH_RECORD_MAX_SIZE			(1024*1024)//(1024*1024)	  

#define FLASH_ADDR_LOG_DATA			(1024*1024*2)	
#define FLASH_ADDR_LOG_CNT			((1024+16)*1024)
#define FLASH_LOG_MAX_SIZE				(1024*1024*2)

#define RECORD_UNIT_LEN					16
#define LOG_UNIT_LEN						64

typedef struct
{	
	uint32_t flash_addr_start;		//�洢��ʼ��ַ����������������
	uint32_t flash_addr_end;		//�洢��ֹ��ַ����������������
	uint32_t flash_addr_param;		//��Ŷ��в����ĵ�ַ
	uint8_t data_len;				//��λ���ݳ���
	uint32_t max_cnt;				//���Ԫ�����ܿռ�/ ���ݳ���- ��ȫ������
	uint32_t cnt;					//��ǰԪ�ظ���
	uint32_t tail_addr;			//��β��ַ
	uint32_t head_addr;			//��ͷ��ַ
	uint32_t param_addr_index;		//�����洢��������ÿ��һ�Σ���1�����1023
	
}FLASH_QUEUE;


typedef struct
{	
	uint32_t flash_addr_start;		//�洢��ʼ��ַ����������������
	uint32_t flash_addr_end;		//�洢��ֹ��ַ����������������
	uint32_t flash_addr_param;		//��Ŷ��в����ĵ�ַ
	uint8_t data_len;				//��λ���ݳ���
	uint32_t max_cnt;				//���Ԫ�����ܿռ�/ ���ݳ���- ��ȫ������
	uint32_t cnt;					//��ǰԪ�ظ���
	uint32_t tail_addr;			//��β��ַ
	uint32_t head_addr;			//��ͷ��ַ
	uint32_t param_addr_index;		//�����洢��������ÿ��һ�Σ���1�����1023
	uint32_t bk_tail_addr;			//���ݶ�β��ַ
	uint32_t bk_cnt;					//���ݵ�ǰԪ�ظ���
}TEMP_FLASH_QUEUE;


void Flash_Init(void);
//void Flash_SectorErase(uint32_t SectorAddr);
void Flash_ChipErase(void);
void Flash_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

void Flash_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void Flash_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

void Flash_SectorErase(unsigned int dst_addr);

uint8_t Flash_SpiRead(void);
uint8_t Flash_SpiWrite(uint8_t byte);
uint16_t Flash_SendHalfWord(uint16_t HalfWord);
void Flash_WriteEnable(void);
void Flash_WaitForWriteEnd(void);

void FlashQueue_SaveParam(FLASH_QUEUE *p_FlashQueue);
void FlashQueue_GetParam(FLASH_QUEUE *p_FlashQueue);
unsigned char FlashQueue_EnQueue(FLASH_QUEUE *p_FlashQueue,unsigned char *dat);
unsigned char FlashQueue_GetTail(FLASH_QUEUE *p_FlashQueue,unsigned char *dat);
unsigned char FlashQueue_DelTail(FLASH_QUEUE *p_FlashQueue);
void Flash_TagRecordQueueInit(void);
void Flash_PrintTagRecordQueueInit(FLASH_QUEUE *p_FlashQueue);
unsigned char FlashQueue_GetTagData(FLASH_QUEUE *p_FlashQueue,unsigned char *dat);
void Flash_EraseData(uint32_t start_addr, uint32_t size);
unsigned char FlashQueue_DeQueue(FLASH_QUEUE *p_FlashQueue,unsigned char *dat);
void FlashQueue_ClearQueue(FLASH_QUEUE *p_FlashQueue);

void FlashQueue_Copy(TEMP_FLASH_QUEUE * p_FlashQueueDis,FLASH_QUEUE *p_FlashQueueSrc);
extern FLASH_QUEUE TagRecordFlashQueue;
extern TEMP_FLASH_QUEUE FlashTempQueue;

void FlashQueue_FinTagGet(FLASH_QUEUE *p_FlashQueue);

 unsigned char FlashQueue_DeQueue_NoSave(FLASH_QUEUE *p_FlashQueue,unsigned char *dat);
 
 //У��flash���� ʧ�ܶ�ξ��ض� 
extern bool FlashQueue_CheckTagData(FLASH_QUEUE *p_FlashQueue, uint8_t *dat, int nTryTime);

void Flash_Read(uint8_t* pBuffer);
void Flash_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void Flash_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
	
#endif 


