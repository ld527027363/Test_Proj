/********************************************************
 * File	       :  debug.c
 * Description :  调试功能接口
 * Author      :  何超平
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
 * Creat       :  2015-11-20
 * Update      :  
 * History     :

*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "types.h"

/************** 调试接口 配置 **************/

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


/*********一般统一调试指令码**************/
#define DEBUG_RET_CMD_ERROR         0x00
#define DEBUG_RET_CMD_MASK          0xA0
#define DEBUG_RET_CMD_OK            0xA0

#define DEBUG_CMD_GET_PRAM					0x01 //获取参数 
#define DEBUG_CMD_SET_PRAM					0x02 //设置参数

#define DEBUG_CMD_GET_TIME				  0x03 //获取下位机时间
#define DEBUG_CMD_GET_LOG						0x04 //获取日志
#define DEBUG_CMD_GET_FLASH_CNT		  0x05 //获取flash中缓存数据数量
#define DEBUG_CMD_GET_TAG		        0x06 //获取flash中缓存数据  16字节每条
#define DEBUG_CMD_GET_STATE					0x07 //获取AP状态 接口状态+运行状态
#define	DEBUG_CMD_GET_SUM           0x08 //获取统计数据

#define DEBUG_CMD_GET_VERSION				0x09 //获取版本
#define DEBUG_CMD_GET_WIFI					0x0B //获取AP信号范围内的列表

#define DEBUG_CMD_ENTER_MODE				0x10 //进入调试模式
#define DEBUG_CMD_EXIT_MODE					0x11 //退出调试模式
#define DEBUG_CMD_GET_RUNING        0x12 //获取运行参数
//#define DEBUG_CMD_GET_SUM						0x13 //获取统计数据


#define DEBUG_CMD_UPGRADE_START			0x40 //开始下载升级
#define DEBUG_CMD_UPGRADE_ING       0X41 //下载升级包
#define DEBUG_CMD_UPGRADE_END       0X42 //下载完毕 准备重启

#define DEBUG_CMD_RESTART						0xE0 //重启AP
#define DEBUG_CMD_ERASE_ALL					0xE1 //重置外部flash
#define DEBUG_CMD_ERASE_LOG					0xE2 //删除所有日志
#define DEBUG_CMD_ERASE_TAG					0xE3 //删除缓存数据

#define DEBUG_CMD_WRITE_1KTAG       0x0c //写入1000条记录到flash缓存
#define DEBUG_CMD_RF_CHANN          0x0D //手动修改433频道号

#define DEBUG_MASK_433RF						(1<<0)//433射频调试信息
#define DEBUG_MASK_TAG              (1<<1)//手环交互信息
#define DEBUG_MASK_SERVER           (1<<2)//服务器交互信息
#define DEBUG_MASK_INTERFACE        (1<<3)//接口调试信息


#define PKT_CHECKSUM_LEN						(1)



/*****************NRF 宏定义 ********************/
#define NRF_TAG_BLE_DOWN            0x51

#define NRF_TAG_BLE                 0xA1
#define NRF_TAG_INIT								0xA0
#define NRF_TAG_STATE								0xA5
#define NRF_CMD_STATE_RESTART       0xFE

/*****************BLE 宏定义 ********************/
#define BLE_CMD_PUSH								0x70
#define BLE_HEAD_LEN								(3)
#define NRF_HEAD_LEN								(4)


#define DEBUG_HEAD_LEN							(3)


#define DEBUG_INTF_CNT							(INTF_TYPE_MAX)
#define DEBUG_LIST_MAX							(8)   //最大缓存debug 队列
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
	__debug_unpack  unpack; /* 调试信息处理包接口 */
	__debug_pack  	pack;  	/* 调试信息打包接口 */
}debug_intf_t,*debug_intf_p;

typedef struct debug_info_s 
{
	u32_t           index;
	debug_intf_t    *intf_fun;
	BOOL          	mode; /* 是否进入 调试模式 */
	u8_t         		mask; /* 调试模式 掩码  1byte  兼容最多8种权限*/
	HANDLE          hdl;
	BOOL            bDoing;
	BOOL            bRecv;
	u8_t            *buf;
}debug_info_t,*debug_info_p;


//调试命令包格式  包头(1B 0xAA)+后方总长度(1B)+命令码(1B)+数据(0-nB)+校验(sum(长度+命令+数据)反码后加一)
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
** 检查掩码开关
*/
#define debug_check_mask(check_mask)\
(debug_get_mask()&(check_mask))


/*
** 检查调试包头
*/
#define debug_check_head(check_mask)\
(DEBUG_PKT_HEAD_UP==check_mask)


extern u8_t g_printf_buf[128];
/*
** 打印debug信息
*/
extern void debug_send(HANDLE hdl, u8_t *dat, u32_t len);

extern BOOL TestReq;


/*
** 发送调试接口 消息  检测是否在处理其他
*/
extern void debug_post_msg(HANDLE hdl,u8_t * msg);
	
/*
** 调试接口处理
*/
extern void debug_process(void);

/*
** debug 接口初始化
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
