#ifndef __UTILITY_h
#define __UTILITY_h


typedef struct
{
	unsigned char *	pStart;			//元素下标
	unsigned int 		len;				//元素长度
}QUEUE_INFO_T;						//不等长队列 需要记录 每个元素大小


typedef struct
{
	unsigned char * pBuf;
	unsigned char *	pHead;			//队首
	unsigned char *	pTail;			//队尾
	unsigned int 		cnt;				//数据组数
	unsigned int 		size;       
	unsigned char  	uint; 
	QUEUE_INFO_T   *pInfo;
}QUEUE_T;						//循环队列，先进先出


void Delay(unsigned int i);
void Delay_us(unsigned short iTime);
void Delay_10us(unsigned int iTime);
void Delay_100us(unsigned int iTime);
void Delay_ms(unsigned int iTime);
void DelayMs(unsigned int iTime);
unsigned char CheckSum(unsigned char *uBuff, unsigned int uBuffLen) ;
unsigned short crc_ccitt(unsigned char *q, int len,unsigned short crc);
void crc_ccitt_old(unsigned char * CrcValue,unsigned char * Data,unsigned short Length);
void crc_ccitt_upgrade(unsigned char * CrcValue,unsigned char * Data,unsigned short Length);
unsigned short CalBinCRC16(unsigned short iCrcValue,unsigned char * Data,unsigned short Length);
unsigned short CRC16(unsigned char * Data,unsigned short Length);

unsigned char  CheckSumEx(unsigned char *uBuff, unsigned int uBuffLen);
#endif

