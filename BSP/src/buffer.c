/*********************************************************
Project :
Version  : 
File name: "Buffer.c"
Date     : 
Author   : 
Company  : 
Comments : 固定长数据缓存队列
**********************************************************/
#include "stdio.h"
#include "string.h"
#include "buffer.h"

#define true 1
#define false 0

unsigned char TagBuf[TAG_RECORD_BUF_SIZE];
unsigned char TagParamBuf[TAG_PARAM_MAX_CNT][16];	//  1 字节有效标识，4字节ID，5 字节参数
unsigned short TagParamCnt = 0;					//标签参数条数
BUF_QUEUE TagDataQueue;

void BufQueueInit(BUF_QUEUE * queue,unsigned char *buf,unsigned int size,unsigned char data_len)
{
	queue->cnt = 0;
	queue->p_buf = buf;
	queue->p_head = buf;
	queue->p_tail = buf;
	queue->buf_size = size;
	queue->uint_len = data_len;
}

 unsigned char BufQueueIsFull(BUF_QUEUE * queue)
{
	if(queue->p_head >= queue->p_tail)			//头在前，尾在后
	{
		if(queue->p_head-queue->p_tail > (queue->buf_size -256))		// 256字节安全缓存
		{
			return 1;
		}
		else
			return 0;
	}
	else										// 头在后，尾在前
	{
		if( queue->p_tail - queue->p_head < 256)
			return 1;
		else
			return 0;
	}
}

unsigned char BufQueueIsEmpty(BUF_QUEUE * queue)
{
	if (queue->cnt == 0)
		return 0x01;
	else
		return 0x00;
}

void BufQueueDelTailData(BUF_QUEUE * queue)			//删除队尾
{
	queue->p_tail += queue->uint_len;
	if(queue->p_tail > (queue->p_buf+queue->buf_size-256))		// 到达顶端
	{
		queue->p_tail = queue->p_buf;
	}
	queue->cnt--;
}
unsigned char BufQueueEnQueue(BUF_QUEUE * queue,unsigned char *data)	//入队列
{
	unsigned char i;
	unsigned char len;
	
	if(BufQueueIsFull(queue) == 1)		//满了
		BufQueueDelTailData(queue);
	
	len = queue->uint_len;
	for(i = 0;i < len;i++)
	{
		(queue->p_head)[i] = data[i];
	}
	queue->p_head+= len;
	if(queue->p_head > (queue->p_buf+queue->buf_size-256))		// 到达顶端
	{
		queue->p_head = queue->p_buf;
	}
	queue->cnt++;
	return 0;
}

unsigned char  BufQueueDeQueue(BUF_QUEUE * queue,unsigned char *dat)	//出队列
{
	unsigned char len,i;
	
	if(BufQueueIsEmpty(queue))
		return false;
	len = queue->uint_len;
	for(i = 0;i < len;i++)
		dat[i] = (queue->p_tail)[i];
	
	queue->p_tail+= len;
	if(queue->p_tail > (queue->p_buf+queue->buf_size-256))		// 到达顶端
	{
		queue->p_tail = queue->p_buf;
	}	
	queue->cnt--;
	return true;
}

unsigned char  BufQueueGetTailData(BUF_QUEUE * queue,unsigned char *dat)		//取队尾而不删除
{
	unsigned char len,i;
	
	if(BufQueueIsEmpty(queue))
		return false;
	len = queue->uint_len;
	for(i = 0;i < len;i++)
		dat[i] = (queue->p_tail)[i];

	return true;
}

// 从FLASH中拷贝参数列表到RAM，暂未实现
unsigned char BufCopyParamToRam(void)			
{
	unsigned short i;
	for(i = 0; i < TAG_PARAM_MAX_CNT; i++)
	{
		TagParamBuf[i][0] = 0;
	}
	return 0;
}

// 新进一参数列表，暂未实现
unsigned char BufEnParamList(unsigned char * param_dat)
{
	return 0;
}

// 在参数列表中查找有没有匹配的ID 的参数，如果有，就导出
unsigned char BufFindTagFromList(unsigned char *id,unsigned char *param)
{
	unsigned short i;

	for(i = 0; i < TAG_PARAM_MAX_CNT; i++)
	{
		if(TagParamBuf[i][0] == TAG_PARAM_VALID_FLAG)
		{
			if(memcmp(id,&(TagParamBuf[i][1]),4) == 0)
			{
				memcpy(param,&(TagParamBuf[i][5]),5);
				TagParamBuf[i][0] = 0;
				return 1;
			}
		}
	}
	return 0;
}

//
unsigned char BufDelParam(unsigned char *id)
{
	return 0;
}
