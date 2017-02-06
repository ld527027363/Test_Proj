#ifndef __WIFIFUNC_H
#define __WIFIFUNC_H


extern volatile unsigned char WIFITimeTic;
extern volatile unsigned char WIFIUpgradeStartFlag;
extern volatile unsigned char WifiConnectFlag;

void WIFI_Reset(void);

void WIFI_IRQHandler(void);
unsigned char WIFI_Init(void);
void WIFI_SendStart(unsigned char *pBuf, unsigned short len);
void WIFI_CommTask(void);

void WIFI_ready(void);
 extern void WIFI_SendProcess(void);

#endif

