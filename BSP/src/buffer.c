/*********************************************************
Project :
Version  : 
File name: "Buffer.c"
Date     : 
Author   : 
Company  : 
Comments : �̶������ݻ������
**********************************************************/
#include "stdio.h"
#include "string.h"
#include "buffer.h"

#define true 1
#define false 0

unsigned char TagBuf[TAG_RECORD_BUF_SIZE];
unsigned char TagParamBuf[TAG_PARAM_MAX_CNT][16];	//  1 �ֽ���Ч��ʶ��4�ֽ�ID��5 �ֽڲ���
unsigned short TagParamCnt = 0;					//��ǩ��������
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
	if(queue->p_head >= queue->p_tail)			//ͷ��ǰ��β�ں�
	{
		if(queue->p_head-queue->p_tail > (queue->buf_size -256))		// 256�ֽڰ�ȫ����
		{
			return 1;
		}
		else
			return 0;
	}
	else										// ͷ�ں�β��ǰ
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

void BufQueueDelTailData(BUF_QUEUE * queue)			//ɾ����β
{
	queue->p_tail += queue->uint_len;
	if(queue->p_tail > (queue->p_buf+queue->buf_size-256))		// ���ﶥ��
	{
		queue->p_tail = queue->p_buf;
	}
	queue->cnt--;
}
unsigned char BufQueueEnQueue(BUF_QUEUE * queue,unsigned char *data)	//�����
{
	unsigned char i;
	unsigned char len;
	
	if(BufQueueIsFull(queue) == 1)		//����
		BufQueueDelTailData(queue);
	
	len = queue->uint_len;
	for(i = 0;i < len;i++)
	{
		(queue->p_head)[i] = data[i];
	}
	queue->p_head+= len;
	if(queue->p_head > (queue->p_buf+queue->buf_size-256))		// ���ﶥ��
	{
		queue->p_head = queue->p_buf;
	}
	queue->cnt++;
	return 0;
}

unsigned char  BufQueueDeQueue(BUF_QUEUE * queue,unsigned char *dat)	//������
{
	unsigned char len,i;
	
	if(BufQueueIsEmpty(queue))
		return false;
	len = queue->uint_len;
	for(i = 0;i < len;i++)
		dat[i] = (queue->p_tail)[i];
	
	queue->p_tail+= len;
	if(queue->p_tail > (queue->p_buf+queue->buf_size-256))		// ���ﶥ��
	{
		queue->p_tail = queue->p_buf;
	}	
	queue->cnt--;
	return true;
}

unsigned char  BufQueueGetTailData(BUF_QUEUE * queue,unsigned char *dat)		//ȡ��β����ɾ��
{
	unsigned char len,i;
	
	if(BufQueueIsEmpty(queue))
		return false;
	len = queue->uint_len;
	for(i = 0;i < len;i++)
		dat[i] = (queue->p_tail)[i];

	return true;
}

// ��FLASH�п��������б�RAM����δʵ��
unsigned char BufCopyParamToRam(void)			
{
	unsigned short i;
	for(i = 0; i < TAG_PARAM_MAX_CNT; i++)
	{
		TagParamBuf[i][0] = 0;
	}
	return 0;
}

// �½�һ�����б���δʵ��
unsigned char BufEnParamList(unsigned char * param_dat)
{
	return 0;
}

// �ڲ����б��в�����û��ƥ���ID �Ĳ���������У��͵���
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
