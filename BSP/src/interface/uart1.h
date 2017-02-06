/********************************************************
 * File	       :  uart1.c
 * Description :  UART 1接口功能
 * Author      :  何超平
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
 * Creat       :  2015-11-20
 * Update      :  
 * History     :

*/
#ifndef __UART1_H__
#define __UART1_H__

extern u32_t uart1_send(u8_t *pDat, u32_t len);

extern u32_t uart1_recv_callback(const u8_t *pBuf, u32_t len);

/*
** 接收验证
*/

extern   BOOL uart1_check(u8_t *pDat, u32_t len);

/*
** 初始化
*/
void uart1_init(void);

#endif
