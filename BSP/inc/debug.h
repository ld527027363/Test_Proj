/********************************************************
 * File	       :  debug.c
 * Description :  ���Թ��ܽӿ�
 * Author      :  �γ�ƽ
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾
 * Creat       :  2015-11-20
 * Update      :  
 * History     :

*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "types.h"

/************** ���Խӿ� ���� **************/

#define TEST_REQ_RF_TX			(1<<0)
#define TEST_REQ_PRINT_RSSI	(1<<1)
#define TEST_REQ_PRINT_COMM	(1<<2)
#define TEST_MEM_PRINT_COMM	(1<<3)
#define TEST_MEM_PRINT_TAGS	(1<<4)



typedef u32_t  (*__debug_send)(u8_t *pDat, u32_t len);
typedef BOOL   (*__debug_unpack)(const u8_t *pBuf, u8_t **pDat);
typedef u32_t  (*__debug_pack)(u8_t *pPack, u8_t cmd, u8_t *pDat, u32_t datLen, u8_t opt);

#define DEBUG_PKT_HEAD_NRF          0XAA
#define DEBUG_PKT_HEAD_BLE          0XAA
#define DEBUG_PKT_HEAD_UP           0XAA
#define DEBUG_PKT_HEAD_AP           0XBB
#define DEBUG_PKT_MIN_SIZE          4
#define DEBUG_PKT_MAX_SIZE          168


/*********һ��ͳһ����ָ����**************/
#define DEBUG_RET_CMD_ERROR         0x00
#define DEBUG_RET_CMD_MASK          0xA0
#define DEBUG_RET_CMD_OK            0xA0

#define DEBUG_CMD_GET_PRAM					0x01 //��ȡ���� 
#define DEBUG_CMD_SET_PRAM					0x02 //���ò���

#define DEBUG_CMD_GET_TIME				  0x03 //��ȡ��λ��ʱ��
#define DEBUG_CMD_GET_LOG						0x04 //��ȡ��־
#define DEBUG_CMD_GET_FLASH_CNT		  0x05 //��ȡflash�л�����������
#define DEBUG_CMD_GET_TAG		        0x06 //��ȡflash�л�������  16�ֽ�ÿ��
#define DEBUG_CMD_GET_STATE					0x07 //��ȡAP״̬ �ӿ�״̬+����״̬
#define	DEBUG_CMD_GET_SUM           0x08 //��ȡͳ������

#define DEBUG_CMD_GET_VERSION				0x09 //��ȡ�汾
#define DEBUG_CMD_GET_WIFI					0x0B //��ȡAP�źŷ�Χ�ڵ��б�

#define DEBUG_CMD_ENTER_MODE				0x10 //�������ģʽ
#define DEBUG_CMD_EXIT_MODE					0x11 //�˳�����ģʽ
#define DEBUG_CMD_GET_RUNING        0x12 //��ȡ���в���
//#define DEBUG_CMD_GET_SUM						0x13 //��ȡͳ������


#define DEBUG_CMD_UPGRADE_START			0x40 //��ʼ��������
#define DEBUG_CMD_UPGRADE_ING       0X41 //����������
#define DEBUG_CMD_UPGRADE_END       0X42 //������� ׼������

#define DEBUG_CMD_RESTART						0xE0 //����AP
#define DEBUG_CMD_ERASE_ALL					0xE1 //�����ⲿflash
#define DEBUG_CMD_ERASE_LOG					0xE2 //ɾ��������־
#define DEBUG_CMD_ERASE_TAG					0xE3 //ɾ����������

#define DEBUG_CMD_WRITE_1KTAG       0x0c //д��1000����¼��flash����
#define DEBUG_CMD_RF_CHANN          0x0D //�ֶ��޸�433Ƶ����

#define DEBUG_MASK_433RF						(1<<0)//433��Ƶ������Ϣ
#define DEBUG_MASK_TAG              (1<<1)//�ֻ�������Ϣ
#define DEBUG_MASK_SERVER           (1<<2)//������������Ϣ
#define DEBUG_MASK_INTERFACE        (1<<3)//�ӿڵ�����Ϣ


#define PKT_CHECKSUM_LEN						(1)



/*****************NRF �궨�� ********************/
#define NRF_TAG_BLE_DOWN            0x51

#define NRF_TAG_BLE                 0xA1
#define NRF_TAG_INIT								0xA0
#define NRF_TAG_STATE								0xA5
#define NRF_CMD_STATE_RESTART       0xFE

/*****************BLE �궨�� ********************/
#define BLE_CMD_PUSH								0x70
#define BLE_HEAD_LEN								(3)
#define NRF_HEAD_LEN								(4)


#define DEBUG_HEAD_LEN							(3)


#define DEBUG_INTF_CNT							(INTF_TYPE_MAX)
#define DEBUG_LIST_MAX							(8)   //��󻺴�debug ����
#define debug_intf_em_t  intf_type_em_t

//#define DEBUG_INTF_CNT							(4)
//typedef enum debug_intf_em
//{
//    DEBUG_INTF_UART1 = 0,
//    DEBUG_INTF_NET,
//		DEBUG_INTF_BLE,
//		DEBUG_INTF_WIFI
//}debug_intf_em_t;


typedef struct debug_intf_s
{
	//__debug_send  	send;  
	__debug_unpack  unpack; /* ������Ϣ������ӿ� */
	__debug_pack  	pack;  	/* ������Ϣ����ӿ� */
}debug_intf_t,*debug_intf_p;

typedef struct debug_info_s 
{
	u32_t           index;
	debug_intf_t    *intf_fun;
	BOOL          	mode; /* �Ƿ���� ����ģʽ */
	u8_t         		mask; /* ����ģʽ ����  1byte  �������8��Ȩ��*/
	HANDLE          hdl;
	BOOL            bDoing;
	BOOL            bRecv;
	u8_t            *buf;
}debug_info_t,*debug_info_p;


//�����������ʽ  ��ͷ(1B 0xAA)+���ܳ���(1B)+������(1B)+����(0-nB)+У��(sum(����+����+����)������һ)
typedef  struct debug_pack_s  
{
	u8_t head;
	u8_t len;
	u8_t cmd;
	u8_t dat[DEBUG_PKT_MAX_SIZE];
}debug_pack_t ,*debug_pack_p;

typedef  struct debug_blepack_s  
{
	u8_t cmd;
	u8_t len_h;
	u8_t len_l;
	u8_t dat[DEBUG_PKT_MAX_SIZE];
}debug_blepack_t;


typedef  struct debug_nrfpack_s  
{
	u8_t head;
	u8_t cmd;
	u8_t len_h;
	u8_t len_l;
	u8_t dat[DEBUG_PKT_MAX_SIZE];
}debug_nrfpack_t;




/*
** ������뿪��
*/
#define debug_check_mask(check_mask)\
(debug_get_mask()&(check_mask))


/*
** �����԰�ͷ
*/
#define debug_check_head(check_mask)\
(DEBUG_PKT_HEAD_UP==check_mask)


extern u8_t g_printf_buf[128];
/*
** ��ӡdebug��Ϣ
*/
extern void debug_send(HANDLE hdl, u8_t *dat, u32_t len);

extern BOOL TestReq;


/*
** ���͵��Խӿ� ��Ϣ  ����Ƿ��ڴ�������
*/
extern void debug_post_msg(HANDLE hdl,u8_t * msg);
	
/*
** ���Խӿڴ���
*/
extern void debug_process(void);

/*
** debug �ӿڳ�ʼ��
*/
extern void debug_init(void);
	

#include "rtc.h"
#define DEBUG_PRINTF(msg,args...) do{\
	memset((char *)g_printf_buf,128,0);\
	sprintf((char *)g_printf_buf,"[%02d:%02d:%02d]",SystemTime.RTC_Hours,SystemTime.RTC_Minutes,SystemTime.RTC_Seconds);\
	sprintf((char *)&g_printf_buf[10],msg, ##args);\
	printf((char *)g_printf_buf);\
}while(0)

	//debug_send(g_printf_buf, 128);

#endif
