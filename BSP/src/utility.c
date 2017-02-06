#include "global.h"
#include "utility.h"
#include "time.h"

const unsigned short crc16_table[]=   //* CRC???
{ 
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

void Delay(unsigned int i)
{
	while(i--);
}
void Delay_us(unsigned short iTime)
{	
	unsigned int i,j;
	for(i=0;i<iTime;i++)
	{
		for (j=0;j<5;j++);
	}
}

void Delay_10us(unsigned int iTime)
{	
	unsigned int i=0;
	for(i=0;i<iTime;i++)
	{
		Delay_us(30);
	}
}

void Delay_100us(unsigned int  iTime)
{
	unsigned int i=0;
	for(i=0;i<iTime;i++)
	{
		Delay_us(300);
	}
}

void DelayMs(unsigned int iTime)
{
    Delay_100us(10*iTime); 
}

void Delay_ms(unsigned int iTime)
{
    Delay_100us(10*iTime); 

}

unsigned char Byte_Parity_Cal(unsigned char Data)
{
	Data = (Data & 0x0F) ^ (Data >> 4);
	Data = (Data & 0x03) ^ (Data >> 2);
	Data = (Data & 0x01) ^ (Data >> 1);
	return Data;		
}

unsigned char  CheckSum(unsigned char *uBuff, unsigned int uBuffLen) //reentrant
{
	unsigned int i;
	unsigned char uSum=0;
	for(i=0;i<uBuffLen;i++)
	{
		uSum = uSum + uBuff[i];
	}
	uSum = (~uSum) + 1;
	return uSum;
}

unsigned char  CheckSumEx(unsigned char *uBuff, unsigned int uBuffLen) //reentrant
{
	unsigned int i;
	unsigned char uSum=0;
	
	for(i=0; i<uBuffLen; i++)
	{
		uSum = uSum ^ uBuff[i];
	}
	
	return uSum;
}


unsigned short CalBinCRC16(unsigned short iCrcValue,unsigned char * Data,unsigned short Length)
{
    unsigned char da;
    while(Length--!=0)
	{
        da=(unsigned char) (iCrcValue/256);
        iCrcValue <<= 8;
        iCrcValue ^= crc16_table[da^*Data];
        Data++;
    }
	return iCrcValue;
}

void crc_ccitt_old(unsigned char * CrcValue,unsigned char * Data,unsigned short Length)
{
    unsigned short crc = 0;
    unsigned char da = 0;
    while (Length-- > 0)
    {
        da=(unsigned char) (crc/256);
        crc <<= 8;
        crc ^= crc16_table[da^*Data];
        Data++;
    }
    crc =  ~crc;
	CrcValue[0] = (unsigned char)(crc >>8);
	CrcValue[1] = (unsigned char)(crc & 0x00FF);
}

void crc_ccitt_upgrade(unsigned char * CrcValue,unsigned char * Data,unsigned short Length)
{
    unsigned short crc = 0;
    unsigned char da = 0;
    while (Length-- > 0)
    {
        da=(unsigned char) (crc/256);
        crc <<= 8;
        crc ^= crc16_table[da^*Data];
        Data++;
    }
    crc =  ~crc;
	CrcValue[0] = (unsigned char)(crc >>8);
	CrcValue[1] = (unsigned char)(crc & 0x00FF);
}

unsigned short crc_ccitt(unsigned char *q, int len,unsigned short crc)
{
    unsigned short __crc = crc;
    while (len-- > 0)
    __crc = crc16_table[(__crc >> 8 ^ *q++) & 0xff] ^ (__crc << 8);
    return ~__crc;
}
unsigned short CRC16(unsigned char * Data,unsigned short Length)
{
	unsigned short crc;
    	unsigned char da;
		
    	crc = 0xFFFF;
    	while(Length--!=0)
		{
        	da=(unsigned char) (crc/256);
        	crc <<= 8;
        	crc ^= crc16_table[da^*Data];
        	Data++;
    	}

	return crc;
}

/******************* queue *****************************/

/*
* 队列初始化
*/
void queue_init(QUEUE_T * pqueue,QUEUE_INFO_T *info, u8_t *buf,u32_t size,u32_t dat_len)
{
	pqueue->cnt 		= 0;
	pqueue->pBuf 	= buf;
	pqueue->pHead 	= buf;
	pqueue->pTail 	= buf;
	pqueue->size 	= size;
	pqueue->uint 	= dat_len;
	pqueue->pInfo  = info;
}

/*
* 队列满？
*/
BOOL queue_chk_full(QUEUE_T * pqueue, u32_t len)
{
	BOOL bRet = FALSE;
	
	if(pqueue->pHead >= pqueue->pTail)			//头在前，尾在后
	{
		if(pqueue->pHead - pqueue->pTail < len)		// 256字节安全缓存
		{
			bRet = TRUE;
		}
	}
	else										// 头在后，尾在前
	{
		if( pqueue->pTail - pqueue->pHead < len)
		{
			bRet = TRUE;
		}
	
	}
	
	return bRet;
}


/*
* 队列空？
*/
BOOL queue_chk_empty(QUEUE_T * pqueue)
{
	BOOL bRet = FALSE;
	
	if (pqueue->cnt == 0)
	{
		bRet = TRUE;
	}
	
	return bRet;
}


/*
* 队列 尾部元素长度
*/
u32_t queue_get_len(QUEUE_T * pqueue)
{
	u32_t len = 0;
	
	if (pqueue->uint == 0)
	{
		len = pqueue->pInfo[pqueue->cnt].len;
	}
	else
	{
		len  = pqueue->uint;
	}

	return len;
}

/*
* 队列删除最后一个元素
*/
void queue_del_tail(QUEUE_T * pqueue)			//删除队尾
{
	u32_t len = 0;
	
	if (pqueue->cnt > 0)
	{
		//等长与不等长队列分别处理
		len= queue_get_len(pqueue);
		
		pqueue->pTail += len;
		
		
		if(pqueue->pTail > pqueue->pBuf )		// 到达顶端
		{
			pqueue->pTail = (u8_t *)((u32_t)pqueue->pBuf - (u32_t)pqueue->pTail);
		}
		pqueue->cnt--;
	}
}

/*
* 队列增加一个元素
*/
BOOL queue_enqueue(QUEUE_T * pQueue, u8_t *pDat, u32_t dat_len)	//入队列
{
//	u8_t i;
	u32_t len  = 0;
	u32_t tmpLen = 0;
	
	if(queue_chk_full(pQueue, dat_len))		//满了删尾
	{
		return FALSE;
		//queue_del_tail(pQueue, );
	}
	
	len= queue_get_len(pQueue);
	tmpLen = ((pQueue->pBuf + pQueue->size) - pQueue->pHead)/sizeof(char);
	
	if (tmpLen > len)
	{
		memcpy(pQueue->pHead, pDat, len);
	}
	else
	{
		memcpy(pQueue->pHead, pDat, tmpLen);
		
		pQueue->pHead = pQueue->pBuf;
		
		memcpy(pQueue->pHead, pDat, len - tmpLen);
	}
	
	pQueue->cnt++;
	
	return 0;
}


//unsigned char  BufQueueDeQueue(BUF_QUEUE * queue,unsigned char *dat)	//出队列
//{
//	unsigned char len,i;
//	
//	if(BufQueueIsEmpty(queue))
//		return false;
//	len = queue->uint_len;
//	for(i = 0;i < len;i++)
//		dat[i] = (queue->p_tail)[i];
//	
//	queue->p_tail+= len;
//	if(queue->p_tail > (queue->p_buf+queue->buf_size-256))		// 到达顶端
//	{
//		queue->p_tail = queue->p_buf;
//	}	
//	queue->cnt--;
//	return true;
//}

//unsigned char  BufQueueGetTailData(BUF_QUEUE * queue,unsigned char *dat)		//取队尾而不删除
//{
//	unsigned char len,i;
//	
//	if(BufQueueIsEmpty(queue))
//		return false;
//	len = queue->uint_len;
//	for(i = 0;i < len;i++)
//		dat[i] = (queue->p_tail)[i];

//	return true;
//}
