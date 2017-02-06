/********************************************************
 * File	       :  interface.c
 * Description :  ͨ�ô���ӿ� 485 LAN WIFI
 * Author      :  �γ�ƽ
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾
 * Creat       :  2015-11-20
 * Update      :  
 * History     :

*/



#ifndef __INTERFACE_H__
#define __INTERFACE_H__


#include "uart1.h"
#include "ble.h"

#ifdef __cplusplus
extern "C" {
#endif
	


typedef u32_t    (*__intf_send)(u8_t *pDat, u32_t len);
typedef u32_t    (*__intf_recv)(const u8_t *pBuf, u32_t len);	
typedef BOOL     (*__intf_check)(u8_t *pDat, u32_t len);	

//�ӿ�״̬
typedef enum intf_state_em
{
    INTF_INIT = 0,
		INTF_IDLE,    
    INTF_RECV,
		INTF_SEND,
}intf_state_em_t;

//�ӿ�����
typedef enum intf_type_em
{
	INTF_TYPE_NULL = 0,
  INTF_TYPE_UART1,
  INTF_TYPE_NET,
	INTF_TYPE_BLE,
	INTF_TYPE_WIFI,
	INTF_TYPE_SERVER,
	INTF_TYPE_MAX
}intf_type_em_t;


//�ӿ� ���� 
typedef struct intf_list_s
{
	__intf_send  	send;  		/* �ӿڷ��ͽӿ� */
	__intf_recv  	recv;  		/* �ӿڽ��սӿ� */
	__intf_check  check;    /* �ӿڽ�����֤�ӿ� */
	intf_type_em_t  type;
	
}intf_list_t,*intf_list_p;

//�ӿ� ����
typedef struct intf_profile_s
{
	intf_list_p intf;
	intf_state_em_t state;
	
}intf_profile_t,*intf_profile_p;



#define SYS_INTF_CNT					(5)

//ϵͳ�ӿڱ�ʶ
typedef enum intf_flag_em
{
	INTF_FLAG_NULL,
  INTF_FLAG_UART1,
  INTF_FLAG_NET,
	INTF_FLAG_BLE,
	INTF_FLAG_WIFI,
	INTF_FLAG_SERVER
}intf_flag_em_t;



/*
* �ӿڳ�ʼ��
*/
extern void intf_init(void);


/*
* �ӿ� ���handle
*/
BOOL check_handle(HANDLE hdl, BOOL bSetCur);

/*
* �ӿڷ���
*/
extern u32_t intf_send(HANDLE hdl, u8_t * pDat, u32_t len);

/*
* �ӿڽ���
*/
extern u32_t intf_recv(HANDLE hdl, u8_t * pBuf, u32_t len);

/*
* ���ӽӿ�  ���ؽӿ�ID
*/
HANDLE intf_add(intf_type_em_t intf_type,intf_flag_em_t intf_flag, __intf_send send, __intf_recv recv_callback, __intf_check);


extern HANDLE g_handle_list[SYS_INTF_CNT+1];
extern intf_profile_p g_intf_p;



//��ȡ��ʼ�����handle
#define intf_get_handle(flag) (g_handle_list[flag])
#define intf_get_cur()    		(g_intf_cur)
#define intf_get_profile(hdl)    (&g_intf_p[hdl])
#define intf_get_type(hdl)    (check_handle(hdl,FALSE)?intf_get_profile(hdl)->intf->type:INTF_TYPE_NULL)


	
#ifdef __cplusplus
}
#endif

	
	
#endif
