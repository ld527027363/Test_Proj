/************************************************************************************************ 
* ��Ȩ����(C) 	�����а��ĿۿۿƼ����޹�˾									
* �ļ���		: Rf433Protocal.c  																	
* �汾			: 1.0                                           							
* ����			:�߾�							                                	  
* ��д����	:	2015-07-14                                               											 
* ���뻷��	:	Keil MDK 4.7.2													                   	 
* ������		:	STM32F4XX		                                            				
* ����			:      433M��д���ͱ�ǩ���нӿ�Э��ʵ�� 							
*----------------------------------------    �޶���¼    --------------------------------------* 
* �°汾		:    																				
* ����			:																				
* ����			:																				
* �޶�����	: 																				 
*																								
*************************************************************************************************/ 
#include "global.h"
#include "si446x_bsp.h"
#include "buffer.h"
#include "log.h"
#include "timer.h"
#include "rtc.h"
#include "SystemParameter.h"
#include "memory.h"
#include "debug.h"
#include "linklayer.h"
#include "Rf433Protocal.h"
#include "testmode.h"

unsigned char TestSlot;
unsigned char TestCons;
unsigned char TestType;

unsigned short Rf433CurrentSlot;	//��ǰʱ϶��������������λ�ĸ�ʱ϶���
unsigned short Rf433ChNum;		// Ƶ����

unsigned char Rf433SlotBusy;	//��ǰʱ϶æ��־��ֻ�ᱣ��һ��ʱ϶����λʱ��
unsigned char Rf433SlotBusyId[4];

STRUCT_RF433_TX_FRAME	RF433TxBuf;
unsigned char Rf433RxData[20];
unsigned char Rf433PrePacketIndex = 0xFF;
unsigned char Rf433SendTimeoutCnt;
unsigned char Rf433SendTimeoutFlag;

volatile unsigned char g_rf433IntRecvFlag = FALSE;


#define CHECK_SLOT_OK			0				//ƥ��
#define CHECK_SLOT_EMPTY		0x01			//����
#define CHECK_SLOT_COLL		0x02			//��ͻ
#define CHECK_SLOT_ERR			0x10			//����ָû��ָ����ʱ϶����

#define SOF_BIT_DEPRIVE		0x40				//��ռʱ϶

#define RF_COMM_CONTINUE		0
#define RF_COMM_STOP			1
typedef union
{
    unsigned int  iTime;
    unsigned char bTime[4];
}MY_UNION_TIME;

MY_UNION_TIME *p_Time;

typedef struct
{
	uint8_t UseFlag;				//ʹ�ñ�־,����λ��ʾ�Ƿ���ʹ��,
								//������λ��ʾ����ʹ�õ�����, 0 ~ F,��ʾ1~16
	uint8_t TagId[4];				//��ǩID
	uint32_t LastCommTime;		//���һ��ͨ��ʱ�䣬��λ��
}SYNC_SLOT_TAB_STRU;

SYNC_SLOT_TAB_STRU Rf433SyncSlotTab[1024];
u32_t Rf433SlotCap;					//ʱ϶����
u32_t Rf433EmptySlotCnt;				//��ʱ϶����

/************************************************************************************************* 
* ��������	:  ʱ϶�б��ʼ��                                                         
* ����			:                                                                
* ����			:                                                                                                  
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/ 
void Rf433_SlotTabInit(void)
{
	u32_t i;

	for(i = 0;i < 1024;i++)
	{
		Rf433SyncSlotTab[i].UseFlag = 0x00;
	}
	Rf433SlotCap = 512;
	Rf433ChNum = 0;
	Rf433EmptySlotCnt = Rf433SlotCap;
}

/************************************************************************************************* 
* ��������	:  �Ա�ǩ���лظ�                                                          
* ����			:                                                                
*        			tag_data: ��ǩ������������                            
*        			cmd: �����·�������
				prarm:�����·�����
				stop_flag: ֪ͨ��ǩ�����ٴ�
* ����			:                                                                 

* ��ע:                                                                  
*                                                                        
**************************************************************************************************/  
uint8_t Rf433_RspTag(unsigned char *tag_data,unsigned char cmd,unsigned char*param,unsigned char stop_flag)
{
	u32_t crc;
	
	RF433TxBuf.sof = 0x80|(tag_data[0]&0x0F);	
	if(stop_flag)								
		RF433TxBuf.sof |= 0x40;				

	crc = CRC16(&(tag_data[1]),4);	//���յ��ı�ǩID����У��
	
	RF433TxBuf.tag_crc[0] = crc>>8;
	RF433TxBuf.tag_crc[1] = crc;
//	RF433TxBuf.tag_crc[0] = crc;
//	RF433TxBuf.tag_crc[1] = crc;
	
	RF433TxBuf.reader_id = strSystemParameter.ProductSn[15];//��д��ID
	RF433TxBuf.rsv = 0;

	RF433TxBuf.cfg[0]=cmd;//��������
	RF433TxBuf.cfg[1]=param[0];
	RF433TxBuf.cfg[2]=param[1];
	RF433TxBuf.cfg[3]=param[2];
	RF433TxBuf.cfg[4]=param[3];
	if(TestMode_Flag)
		vRadio_StartTx(strSystemParameter.RFTxChannel, RF_TX_LEN,(uint8_t*)(&RF433TxBuf));
	else
		vRadio_StartTx(RF_FREQ_TX_CHANNEL, RF_TX_LEN,(uint8_t*)(&RF433TxBuf));
	Rf433SendTimeoutCnt = 5;			// 50ms
	return RF_ERROR_OK;
}


/************************************************************************************************* 
* ��������	: ����ǩ��ʱ϶                                                    
* ����			:                                                                
*        			tag_id: ��ǩID                            
*        			correct_slot: ��ȷʱ϶�ţ��ɺ������
* ����			:                                                                 
				CHECK_SLOT_OK			0	//ƥ��
				CHECK_SLOT_EMPTY		0x01//����
				CHECK_SLOT_COLL		0x02//��ͻ
				CHECK_SLOT_ERR			0x10//����ָû��ָ����ʱ϶����,������ǰ����                                  
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/  
uint8_t Rf433_CheckTagSlot(uint8_t *tag_id, u32_t *correct_slot)
{
	int i;
	uint8_t status;
	u32_t current_slot_m;

	current_slot_m = Rf433CurrentSlot>>2;
	
	if((Rf433SyncSlotTab[current_slot_m].UseFlag)&0xF0)	//��ǰʱ϶������
	{
		i = memcmp(Rf433SyncSlotTab[current_slot_m].TagId,tag_id,4);
		if(i == 0)
		{
			Rf433SyncSlotTab[current_slot_m].LastCommTime = SysTick1sCnt;//ˢ�����һ��ͨ��ʱ��
			return CHECK_SLOT_OK;
		}
		else
			status = CHECK_SLOT_COLL;
	}
	else
	{
		status =  CHECK_SLOT_EMPTY;
	}
	for(i = 0; i < Rf433SlotCap; i++)
	{
		if((Rf433SyncSlotTab[i].UseFlag)&0xF0)
		{
			if(memcmp(Rf433SyncSlotTab[i].TagId,tag_id,4) == 0)	//��ѯ���ñ�ǩ����ȷʱ϶
			{
				*correct_slot = ((i<<4)|((Rf433SyncSlotTab[i].UseFlag)&0x0F));
				status |= CHECK_SLOT_ERR;
				return status;
			}
		}
	}
	return status;
}

/************************************************************************************************* 
* ��������	: ��ȡһ���µĿ���ʱ϶                                                  
* ����			:                                                                
*        			tag_id: ��ǩID                            
* ����			:  ��ȡ��ʱ϶, ��10 λʱ϶��ź�4λ������ռ����                                                               
                              
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/  
u32_t Rf433_ObtainNewSlot(uint8_t *tag_id)
{
	u32_t slot;
	uint8_t current_slot_m;
	u32_t i;//j;

	if(Rf433EmptySlotCnt == 0)	//ʱ϶��
		return 0xFFFF;
	current_slot_m = Rf433CurrentSlot>>2;

	for(i = 0;i < Rf433SlotCap;i+= 4)		//��һ�֣����ȿ���4 ��������ʱ϶
	{
		slot = (current_slot_m&0xFFFC)+i;
		if(current_slot_m&0x03)
			slot += 4;
		if(slot >= Rf433SlotCap)
			slot &= (Rf433SlotCap-1);
		if(Rf433SyncSlotTab[slot].UseFlag == 0)
		{
			Rf433SyncSlotTab[slot].UseFlag = 0x10;
			memcpy(Rf433SyncSlotTab[slot].TagId,tag_id,4);
			Rf433EmptySlotCnt--;
			return (slot<<4);
		}
	}
	
	for(i = 0;i < Rf433SlotCap;i+= 4)		//�ڶ��֣�����2 ��������(��4)ʱ϶
	{
		slot = (current_slot_m&0xFFFC) + i + 2;
		if(current_slot_m&0x03)
			slot += 4;
		if(slot >= Rf433SlotCap)
			slot &= (Rf433SlotCap-1);
		if(Rf433SyncSlotTab[slot].UseFlag == 0)
		{
			Rf433SyncSlotTab[slot].UseFlag = 0x10;
			memcpy(Rf433SyncSlotTab[slot].TagId,tag_id,4);
			Rf433EmptySlotCnt--;
			return (slot<<4);
		}
	}
	
	for(i = 0;i < Rf433SlotCap;i+= 2)		//�����֣�����ʱ϶
	{
		slot = (current_slot_m&0xFFFE) + i + 1;
		if(current_slot_m & 0x01)
			slot += 2;
		if(slot >= Rf433SlotCap)
			slot &= (Rf433SlotCap-1);
		if(Rf433SyncSlotTab[slot].UseFlag == 0)
		{
			Rf433SyncSlotTab[slot].UseFlag = 0x10;
			memcpy(Rf433SyncSlotTab[slot].TagId,tag_id,4);
			Rf433EmptySlotCnt--;
			return (slot<<4);
		}
	}
	return 0xFFFF;
}

/************************************************************************************************* 
* ��������	: ��ȡ��ǰʱ϶����                                         
* ����			:                                                                
*        			tag_id: ��ǩID                            
* ����			:   ��ȡ��ʱ϶, ��10 λʱ϶��ź�4λ������ռ����                                                                    
                              
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/  
u32_t Rf433_ObtainCurrentSlot(uint8_t *tag_id)
{
	uint8_t continue_num;
	u32_t empty_div,current_slot_m;
	u32_t i;

	current_slot_m = Rf433CurrentSlot>>2;
	
	if((Rf433SyncSlotTab[current_slot_m].UseFlag)&0xF0)
		return 0xFFFF;

	empty_div = Rf433EmptySlotCnt/(Rf433SlotCap>>2);	//��������ʱ϶��= ����ʱ϶��/��ʱ϶1/4			
	
	for(i = 0; i < empty_div; i++)					//����ʵ�ʿ��е�ʱ϶��
	{
		if((Rf433SyncSlotTab[current_slot_m+i].UseFlag)&0xF0)
		break;
	}
	continue_num = i;
	for(i = 0; i < continue_num+1; i++)			//�ѿ��е�ʱ϶���ϱ�ǩID��
	{
		Rf433SyncSlotTab[current_slot_m+i].UseFlag = (0x10|continue_num);	
		memcpy(Rf433SyncSlotTab[current_slot_m+i].TagId,tag_id,4);
	}
	Rf433EmptySlotCnt -= (continue_num+1);		//ʣ�����ʱ϶

	return ((current_slot_m<<4)+continue_num);
}


/************************************************************************************************* 
* ��������	: ����ǩ�ظ�ͬ��֡                                                         
* ����			:                                                                
*        			tag_data: ��ǩ������������                            
* ����			:  ��                                                               
*        			                                   
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/ 
uint8_t Rf433_SendSyncPkt(uint8_t * tag_data,u32_t send_slot,uint8_t stop_flag)
{
	uint8_t param_buf[5];
	u32_t temp;
	
	temp = Rf433ChNum<<14;
	temp += ((Rf433SlotCap >> 8)<<12);
	temp += Rf433CurrentSlot;

	param_buf[0] = (temp>>8);
	param_buf[1] = temp;
	
	param_buf[2] = (send_slot>>8);
	param_buf[3] = send_slot;

	Rf433_RspTag(tag_data,RF_RESP_SYNC,param_buf,stop_flag);
	return 0;
	
}

/************************************************************************************************* 
* ��������	: ����ǩ�ظ�ʵʱʱ��                                                       
* ����			:                                                                
*        			tag_data: ��ǩ������������                            
* ����			:  ��                                                               
*        			                                   
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/ 
uint8_t Rf433_SendRtcPkt(uint8_t * tag_data)
{
	uint8_t param_buf[5];
//	u32_t temp;
	
	param_buf[0] = SystemTimeSimple>>24;
	param_buf[1] = SystemTimeSimple>>16;
	param_buf[2] = SystemTimeSimple>>8;
	param_buf[3] = SystemTimeSimple;


	Rf433_RspTag(tag_data,RF_RESP_SET_TIME,param_buf,RF_COMM_CONTINUE);
	return 0;
}

/************************************************************************************************* 
* ��������	: ����ǩ�ظ��հ�                                                    
* ����			:                                                                
*        			tag_data: ��ǩ������������                            
* ����			:  ��                                                               
*        			                                   
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/ 
uint8_t Rf433_SendNULL(uint8_t * tag_data)
{
	uint8_t param_buf[5]= {0};
//	u32_t temp;

	Rf433_RspTag(tag_data,RF_RESP_NULL,param_buf,RF_COMM_CONTINUE);
	return 0;
}

#define DEF_TAG_LEN   4
#define DEF_DAT_LEN   4
BOOL  GetTagMask(u8_t * tag)
{
	BOOL bRet = FALSE;
	u8_t tag_mask_set[DEF_TAG_LEN][DEF_DAT_LEN] = {
	{0xA9,0x7B,0xC2,0xFC},
	{0xA9,0x7A,0x88,0x6C},
  {0xA9, 0x7A, 0xF6, 0xDC},
	{0xA9, 0x7A, 0xFA, 0xCC}};
  
	int i,j;
	
	for (i=0; i<DEF_TAG_LEN;i++)
	{
		
		for (j=0; j<DEF_DAT_LEN;j++)
		{
			if (tag[j] != tag_mask_set[i][j])
			{
				break;
			}
		}
		
		//same
		if (j >= DEF_DAT_LEN)
		{
			bRet = TRUE;
			break;
		}
		
	}
	
	return bRet;
}

/************************************************************************************************* 
* ��������	: ��Ƶ���ݽ��մ���                                                         
* ����			:                                                                
*        			rf_data: ��ǩ������������                            
* ����			:  ��                                                               
*        			                                   
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/ 
void Rf433_RcvDataHandle(unsigned char *rf_data)
{
	STRUCT_RF433_RX_FRAME *p_stTagData;
	unsigned char ret,stop;
	unsigned char param[5] = {0};
	u32_t slot;
	static unsigned char pre_data[7];
	static BOOL bLed = FALSE;

	TestType = 0;
	
	TestMode_RF433_Recv(rf_data);
	if(TestMode_Flag)
		return;
	p_stTagData=(STRUCT_RF433_RX_FRAME *)rf_data;
	
	if (bLed)
	{
		SET_LED2();
	}
	else
	{
		CLR_LED2();
	}
	bLed = !bLed;
	g_rf433IntRecvFlag  = TRUE;
	
//	if (!GetTagMask(p_stTagData->id))
//	{
//		return;
//	}
	
//		if(TestReq & TEST_REQ_PRINT_RSSI)
//		{
//			rssi = rf_data[10]/2 - 116;
//			DEBUG_PRINTF("Tag:%02X %02X %02X %02X,D:%02X %02X %02X ,Tim:%02X %02X,Rssi:%d.\r\n",rf_data[1],rf_data[2],rf_data[3],rf_data[4],rf_data[5],rf_data[6],rf_data[7],rf_data[8],rf_data[9],rssi);
//			//if(TestType)
//			//	printf(">>Slot:%d--%d,%d.\r\n",TestType,TestSlot,(Rf433CurrentSlot/4));
//		}	
//	//20160317 hcp û��ʱ϶������ͬ��ʱ��
	if(p_stTagData->tag == RF_RESP_SET_TIME)			//������������費��Ҫ��ʱ
	{
		p_Time = (MY_UNION_TIME *)p_stTagData->value;
		if((SystemTimeSimple !=0 )&&(abs(p_Time->iTime - SystemTimeSimple) > 120))		//������
		{
			Rf433_SendRtcPkt(rf_data);
		}
		else
		{
			//Rf433_SendNULL(rf_data);
		}
		return;
	}
	
	g_rf433IntRecvFlag  = TRUE;
	if(BufQueueIsFull(&TagDataQueue) || ((!g_bServerLoginSucFlag)&& (TestReq == 0)))		//������������������,��¼����ܲɼ�
		return;
	
	if(rf_data[0] & SOF_BIT_DEPRIVE)	//��ռʱ϶����Ϊʱ϶��Ч
	{
		ret = CHECK_SLOT_OK;
	}
	else
	{
		ret = Rf433_CheckTagSlot(rf_data+1,&slot);
	}
	if(ret == CHECK_SLOT_OK)		//ʱ϶����ȷ, ����������
	{
		p_stTagData=(STRUCT_RF433_RX_FRAME *)rf_data;
		ret = BufFindTagFromList(p_stTagData->id,param);		//�鿴�Ƿ��в�����Ҫ�·�
		if(ret != 0)						
		{
			Rf433_RspTag(rf_data,param[0],param+1,RF_COMM_CONTINUE);	//�·�����
		}
		else
		{
			if(p_stTagData->tag == RF_RESP_SET_TIME)			//������������費��Ҫ��ʱ
			{
				p_Time = (MY_UNION_TIME *)p_stTagData->value;
				if((SystemTimeSimple !=0 )&&(abs(p_Time->iTime - SystemTimeSimple) > 120))		//������
				{
					Rf433_SendRtcPkt(rf_data);
				}
				else
				{
					//Rf433_SendNULL(rf_data);
				}
			}
			else
				Rf433_SendRtcPkt(rf_data);
		}		
	}
	else
	{
		if(( ret & CHECK_SLOT_ERR) == 0)		//û��ʱ϶��������ʱ϶
		{
			 slot = Rf433_ObtainNewSlot(rf_data+1);
			 TestType |= CHECK_SLOT_ERR;
		}
		
		if(ret & CHECK_SLOT_COLL)	 		//�г�ͻ��ضϣ�û����������ǰ����
		{
			stop = RF_COMM_STOP;
			TestType |= CHECK_SLOT_COLL;
		}
		else if(Rf433SlotBusy)		//��ʱ϶�����Ǳ���ռ��
		{
			if(memcmp(Rf433SlotBusyId,rf_data +1,4) == 0)			//���Լ���ռ��
			{
				stop = RF_COMM_CONTINUE;
			}
			else
				stop = RF_COMM_STOP;
		}
		else
		{
			stop = RF_COMM_CONTINUE;
			Rf433SlotBusy = 1;								//�����˲������Լ��Ŀ�ʱ϶������ռ��־
			memcpy(Rf433SlotBusyId,rf_data+1,4);
		}
		
		if(slot != 0xFFFF)
		{
			Rf433_SendSyncPkt(rf_data,slot,stop);
		}
		else
		{
			Rf433_SendRtcPkt(rf_data);
		}
		
		TestSlot = slot>>4;		
		
	}
	
	
		if((rf_data[5] &0xF0) == 0x50 || (rf_data[5] &0xF0) == 0x60|| (rf_data[5] &0xF0) == 0x70|| (rf_data[5] &0xF0) == 0x80) 
    {
			if(memcmp(pre_data,rf_data,6) == 0)				//�ظ����ݣ����洢
			{
			//	printf("**%02X,%02X\r\n",rf_data[0],pre_data[0]);
			}
			else					//�������ݲ�ͬ�������п������ظ��������࿨����ʱ��
			{
				BufQueueEnQueue(&TagDataQueue,rf_data);
				
		//printf("--2--%02x%02x%02x%02x%02x%02x\r\n",rf_data[0],rf_data[1],rf_data[2],rf_data[3],rf_data[4],rf_data[5]);
			}
			memcpy(pre_data,rf_data,6);
		}  
		
}

/************************************************************************************************* 
* ��������	: SI4463���жϴ�����                                                         
* ����			:  ��                                                            
*                               
* ����			:  ��                                                               
*        			                                   
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/ 
void Rf433_IntHandle(void)
{
	unsigned char rx_buf[16]={0};
	unsigned char status;	
	int rssi;
	status = gRadio_IntHandle(rx_buf,RF_RX_LEN);
	if(status == 1)
	{
		Rf433SendTimeoutCnt = 0;	//ֹͣ���ͳ�ʱ����
		vRadio_StartRX(RF_FREQ_RX_CHANNEL,RF_RX_LEN);
	}
	else if(status == 2)
	{	
		Rf433_RcvDataHandle(rx_buf);	

		if(TestReq & TEST_REQ_PRINT_RSSI)
		{
			rssi = rx_buf[10]/2 - 116;
			DEBUG_PRINTF("Tag:%02X %02X %02X %02X,D:%02X %02X %02X ,Tim:%02X %02X,Rssi:%d.\r\n",rx_buf[1],rx_buf[2],rx_buf[3],rx_buf[4],rx_buf[5],rx_buf[6],rx_buf[7],rx_buf[8],rx_buf[9],rssi);
			if(TestType)
				printf(">>Slot:%d--%d,%d.\r\n",TestType,TestSlot,(Rf433CurrentSlot/4));
		}	

	//	StartBeep(10,strSystemParameter.BeepFlag);		
	}
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line9) != RESET)
	{	
		Rf433_IntHandle();
		//printf("1\r\n");
		EXTI_ClearITPendingBit(EXTI_Line9);

	}
} 

/************************************************************************************************* 
* ��������	: ���ݺϷ�����֤                                                         
* ����			:  ��                                                            
*                               
* ����			:                                                       
*        			                                   
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/ 
unsigned char Rf433_CheckRFRevData(unsigned char *pBuf)
{
//	unsigned short d;
//	unsigned char t;
	
//	printf("------%02x%02x%02x%02x\r\n",pBuf[5],pBuf[6],pBuf[7],pBuf[8]);
	if (pBuf[5] == 0x70)
	{
		pBuf[5] = 0x73;
	}
	
	
	if(((pBuf[5] &0xF0) != 0x50) && ((pBuf[5] &0xF0) != 0x60) && ((pBuf[5] &0xF0) != 0x70) && ((pBuf[5] &0xF0) != 0x80)) 
	{
		return 0;  
	}  
/*
    d = pBuf[8]*256+pBuf[9];					//�����Э������˷Ǳ�׼��ʽ��ʱ�䣬����������
	
	t = d&0x003F;		//��
	if(t >= 60)
		return 0;
	t = (d>>6)&0x001F;	//ʱ
	if(t > 24)			
         return 0;
	t = (d>>11);		//��
	if(t >31)
		return 0;*/
	return 1;

}


/************************************************************************************************* 
* ��������	: RF433�Ĵ���������                                                   
* ����			:  ��                                                            
*                               
* ����			:  ��                                                               
*        			                                   
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/ 
void RF433_Task(void)
{
	unsigned short tim;
	if(Rf433SendTimeoutFlag)
	{
		Rf433SendTimeoutFlag = 0;
		si446x_get_int_status(0,0,0);
		vRadio_StartRX(strSystemParameter.RFRxChannel,RF_RX_LEN);
	}
	if(RF433_SPI_IRQ() == 0)			
	{
		Delay_us(10);
		if(RF433_SPI_IRQ() == 0)		
		{
			Rf433_IntHandle();
		}
	}	
	
	if(TagDataQueue.cnt)
	{
		FeedDog();
		DisableIrq();  //hcp 2015-11-20 �����жϱ���
		
		BufQueueDeQueue(&TagDataQueue,Rf433RxData);
		
		//printf("**%02X %02X %02X \r\n",Rf433RxData[1],Rf433RxData[5],Rf433RxData[6]);
		if(Rf433_CheckRFRevData(Rf433RxData))
		{
			Rf433RxData[0] = Rf433RxData[10];		//��10���ֽڵ�RSSI�Ƶ���0�ֽ�
			//Rf433RxData[10] =  SystemDate.RTC_Year;
			Rf433RxData[10] =  SystemDate.RTC_Month;
			Rf433RxData[11] =  SystemDate.RTC_Date;
			Rf433RxData[12] =  SystemTime.RTC_Hours;
			Rf433RxData[13] =  SystemTime.RTC_Minutes;
			Rf433RxData[14] =  SystemTime.RTC_Seconds;
			Rf433RxData[15] = CheckSum(Rf433RxData,15);  //����flash �洢����У�� 2016-01-20 hcp
			FeedDog();		
			
			FlashQueue_EnQueue(&TagRecordFlashQueue, Rf433RxData); 
			
			EnableIrq();//hcp 2015-11-20 �����жϱ���
			
	#ifdef EN_USING_LOG
			tim = Rf433RxData[8]*256 + Rf433RxData[9];
 			sprintf(LogStrBuf,"Tag:%02X%02X%02X%02X ",Rf433RxData[1],Rf433RxData[2],Rf433RxData[3],Rf433RxData[4]);
			sprintf(LogStrBuf+13,"D:%02X%02X%02X ",Rf433RxData[5],Rf433RxData[6],Rf433RxData[7]);
			sprintf(LogStrBuf+22,"R:%d Tim:%02d,%02d:%02d\r\n\0",Rf433RxData[0]/2 - 116,(tim>>11),((tim>>6)&0x1F),(tim&0x3F));
			AddLog(LogStrBuf);
			
			//д����־��Ч
			//memset(LogStrBuf,0,64); 
			//sprintf(LogStrBuf,"cnt:%d tail:%08X,head:%08X\r\n\0",   //������־������� hcp 2015-10-12
			//TagRecordFlashQueue.cnt,TagRecordFlashQueue.tail_addr,TagRecordFlashQueue.head_addr);		
			//AddLog(LogStrBuf);	
			
	#endif
			
		}
	}
}



unsigned char BCD2HEX(unsigned char bcd_data)    //BCD??HEX???     
{   
    unsigned char temp;   
    temp=(bcd_data/16*10 + (bcd_data%16));   
    return temp;   
}

unsigned char HEX2BCD(unsigned char hex_data)    //HEX??BCD???      
{   
    unsigned char temp;   
    temp=(hex_data/10*16 + (hex_data%10));
//temp = 	hex_data;
    return temp;   
}  

extern void ChangeDataFormat(unsigned char *pDest, unsigned char *pSrc);

///����ת�� ��ʽ
typedef struct rf433_dat_stu
{
	u8_t  id[4];
	u8_t  date[2];
	u8_t  tag;
	u8_t  time[2];
	u8_t  dat[2];
}* rf433_dat_p;

typedef struct rf433_send_stu
{
	u8_t  id[5];
	u8_t  temp[2];
	u8_t  weig[2];
	u8_t  high[2];
	u8_t  rsv[2];
	u8_t  time[6];
}* rf433_send_p;

#define RF433_REC_LEN  19
static u32_t rf433_rcd_cnt = 0;
void rf433_fmt_dat(u8_t *buf,  u8_t* dat)
{
	rf433_dat_p pDat;
	rf433_send_p pSend;
	int dec = 0;
	u32_t time = 0;
	
	pDat = (rf433_dat_p)dat;
	pSend = (rf433_send_p)buf;
	
	memcpy(&pSend->id[1], pDat->id, 4);
	
	dec = (pDat->dat[0]<<8)|(pDat->dat[1]<<0);
	switch (pDat->tag)
	{
		case 0x71://���
			dec = dec/10;
			pSend->high[0] = HEX2BCD(dec/100);
			pSend->high[1] = HEX2BCD(dec%100);
			break;
		
		case 0x72://����
			dec = dec/10;
			pSend->weig[0] = HEX2BCD(dec/100);
			pSend->weig[1] = HEX2BCD(dec%100);
			break; 
		
		case 0x73://����
			pSend->temp[0] = HEX2BCD(dec/10);
			pSend->temp[1] = HEX2BCD(dec%10);
			break;
	}
	
	//time format
	time = (pDat->date[0]<<8)|pDat->date[1];
	pSend->time[0] = ((time>>10)&((1<<5)-1));
	pSend->time[1] = ((time>>5)&((1<<5)-1));
	pSend->time[2] = ((time>>0)&((1<<5)-1));
	
	pSend->time[3] = (pDat->time[0]);
	pSend->time[4] = (pDat->time[1]);
	pSend->time[5] = 0;
	 
}



u32_t rf433_get_rec(u8_t * buf, u32_t maxLen)
{
	u32_t tmp = 0;
	u8_t strSrc[16];
	u8_t strDst[16];
	rf433_dat_p pDat = 0;
	
	int i=0;
	u32_t nRet = 0;
	
	rf433_rcd_cnt = 0;
	for(i=0; i< maxLen/RF433_REC_LEN-1; )
	{
		tmp = FlashQueue_DeQueue((FLASH_QUEUE *)&TagRecordFlashQueue,strSrc);		
		if(tmp == 0) 
		{								
			break;
		}
		else
		{
			
			ChangeDataFormat(strDst, &strSrc[1]);
			
			pDat = (rf433_dat_p)strDst;
			
			if ((pDat->tag &0X70)!= 0X70)
			{
				continue;
			}
			//���ݴ洢
	    	rf433_fmt_dat(&buf[1+nRet],strDst);
			
			rf433_rcd_cnt++;
			buf[0] = rf433_rcd_cnt;
			nRet += RF433_REC_LEN;
			i++;
			//FlashQueue_FinTagGet(&TagRecordFlashQueue);
		}
	}
	
	if (nRet > 0) nRet++;
	return nRet;
}


/************************************************************************************************* 
* ��������	: ʱ϶����                                                 
* ����			:  ��                                                            
*                               
* ����			:  ��                                                               
*        			                                   
* ��ע:            ����ÿ��������ִ��һ��                                     
*                                                                        
**************************************************************************************************/ 
void Rf433_SlotRecycle(void)
{
	u32_t i;//j;
	uint8_t cnt;

	for(i = 0; i < Rf433SlotCap; i++)
	{
		if(Rf433SyncSlotTab[i].UseFlag & 0x10)
		{
			cnt = (Rf433SyncSlotTab[i].UseFlag & 0x0F)+1;
			if((SysTick1sCnt - Rf433SyncSlotTab[i].LastCommTime) > 3600)		//һ��Сʱû�г���
			{
				while(cnt--)
				{
					Rf433SyncSlotTab[i].UseFlag = 0;
					i++;
					Rf433EmptySlotCnt++;
				}
			}
			else if((cnt > 1)&&(Rf433EmptySlotCnt < (Rf433SlotCap/10)))		//��ʣ��ʱ϶��С����ʱ϶��1/10
			{
				cnt	-=	1;			//����һ���������Ķ��ͷ�
				i++;
				while(cnt--)
				{
					Rf433SyncSlotTab[i].UseFlag = 0;
					i++;
					Rf433EmptySlotCnt++;
				}				
			}
		}
	}
}



