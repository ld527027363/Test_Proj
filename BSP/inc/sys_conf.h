#ifndef __SYS_CONFIG_H__
#define __SYS_CONFIG_H__


//#define USE_PARAM_ENABLE  //参数下发功能打开
//#define USE_SEND433_TEST_ENABLE   //测试功能  

/*************************************************/

#define ERRO_LOSE_POWER			0x0001



/*************************************************/


/******** 系统关键信息设置 ***********/

//固件版本号
#define MYVERSION  						55
#define VERSION_STR           "0.1.55 Bulid-161208"

//硬件版本号
#define BOARDVersion 					"0405"

//调试版本 自动开启调试信息输出、新功能调试  (屏蔽即为发布版本)
#define DEBUG_MODE_VERSION

/******** 系统关键配置设置 ***********/

//接收与发送缓存
#define TCP_MAX_RCV_LEN  					(6*1024)
#define TCP_MAX_SEND_LEN  				(9*1024)


///与服务器交互设置
#define UPLOADDATA_TIME   				15        //n秒上传一次
#define UPLOADHEART_TIME   				(60*5)   	//n秒后一次心跳 
#define UPLOADDATA_RECORD_CNT			(100)					//超过100条数据上传一次

#define SERVER_COMM_TIMEOUT	 			20     		//每次交互超时


//test
#define RESTRAT_TEST_TIMEOUT      (6)   //维护模式下 系统重启 是平时的8倍 约1小时

//rf433
#define DEASOM_RF433_TIMER_L1	    (300)    //5分钟 接收超时 复位芯片
#define DEASOM_RF433_TIMER_L2     (600)    //10分钟 接收超时 复位系统


//ble参数设置
#define ble_timeout_sec 					(60)
#define conn_timeout_sec 					(20)

/******** 系统功能开关设置 ***********/

//开启日志
#define EN_USING_LOG   				1

//开启BLE功能
#define EN_USING_BLE					1


//开启打印调试信息
#define EN_DEBUG_MSG					1

//开启新功能调试
#define EN_FUN_TEST						1



/******** 辅助部分(一般不用修改)***********/

//版本： 调试 or 正式
#ifdef DEBUG_MODE_VERSION
	#ifndef EN_DEBUG_MSG
		#define EN_DEBUG_MSG 					1
	#endif
	
	#ifndef EN_FUN_TEST
		#define EN_FUN_TEST 					1
	#endif
#else
	#define REALEASE_MODE_VERSION
#endif



#define  IF_FUN_TEST       	#if (EN_FUN_TEST)
#define  END_FUN_TEST				#endif


#endif //__SYS_CONFIG_H__

