/********************************************************
 * File	       :  uart1.c
 * Description :  UART 1�ӿڹ���
 * Author      :  �γ�ƽ
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾
 * Creat       :  2015-11-20
 * Update      :  
 * History     :

*/
#ifndef __UART1_H__
#define __UART1_H__

extern u32_t uart1_send(u8_t *pDat, u32_t len);

extern u32_t uart1_recv_callback(const u8_t *pBuf, u32_t len);

/*
** ������֤
*/

extern   BOOL uart1_check(u8_t *pDat, u32_t len);

/*
** ��ʼ��
*/
void uart1_init(void);

#endif
