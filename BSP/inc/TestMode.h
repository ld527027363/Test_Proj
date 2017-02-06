
#ifndef __TESTMODE_H_
#define __TESTMODE_H_

#include <stdint.h>

extern uint8_t TestMode_Flag;

extern uint32_t Sys_Tick_10ms;

extern uint8_t TestMode_WiFi_Flag;

void TestMode_Init(void);

void TestMode_LED(void);

void TestMode_Flash(uint8_t *test_flag);

void TestMode_Beep(void);

void TestMode_RF433(uint8_t *id);

void TestMode_WiFi(uint8_t *test_flag);

void TestMode_W5500(uint8_t *test_flag);

void TestMode_Clear_Record(void);

void TestMode_RF433(uint8_t *id);

uint8_t TestMode_RF433_Recv(uint8_t *rx_data);


void Sys_TestMode_Task(void);
void Sys_TestMode_Recv(void);
void Sys_TestMode_RecvData(uint8_t recv_data);

#endif













