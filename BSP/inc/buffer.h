/*****************************************************
Project : EPC&ISO18000 Series interrogator 
Version  : 2.0   
File name: "buffer.h"
Date     : 
Author   : GaoJun
Company  : 
Comments : 
*****************************************************/
#ifndef __BUFFER_H
#define __BUFFER_H

#define TAG_RECORD_BUF_SIZE		0x8000			// 32K 数据缓存
#define TAG_RECORD_LEN		    16	

extern unsigned char TagBuf[];

#define TAG_PARAM_MAX_CNT			1000			// 最大1000 个手环参数存储区

#define TAG_PARAM_VALID_FLAG		0xAA
//extern unsigned char TagParamBuf[TAG_PARAM_MAX_CNT][10];	//  1 字节有效标识，4字节ID，5 字节参数
extern unsigned short TagParamCnt;					//标签参数条数

typedef struct
{
	unsigned char * p_buf;
	unsigned char *p_head;			//队首
	unsigned char *p_tail;			//队尾
	unsigned short cnt;				//数据组数
	unsigned short buf_size;
	unsigned char  uint_len;
}BUF_QUEUE;						//循环队列，先进先出

extern BUF_QUEUE TagDataQueue;

void BufQueueInit(BUF_QUEUE * queue,unsigned char *buf,unsigned int size,unsigned char data_len);
 unsigned char BufQueueIsFull(BUF_QUEUE * queue);
 unsigned char BufQueueIsEmpty(BUF_QUEUE * queue);
 void BufQueueDelTailData(BUF_QUEUE * queue);
 unsigned char BufQueueEnQueue(BUF_QUEUE * queue,unsigned char *data);
 unsigned char  BufQueueDeQueue(BUF_QUEUE * queue,unsigned char *dat);
 unsigned char  BufQueueGetTailData(BUF_QUEUE * queue,unsigned char *dat);

 unsigned char BufFindTagFromList(unsigned char *id,unsigned char *param);
 
#endif

