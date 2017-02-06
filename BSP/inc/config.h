#ifndef __CONFIG_H__
#define __CONFIG_H__


//#define USE_PARAM_ENABLE  //参数下发功能打开
//#define USE_SEND433_TEST_ENABLE   //测试功能  


/******** 系统关键信息设置 ***********/

//固件版本号
#define MYVERSION  						34 // hcp 2016-1-21

//硬件版本号
#define BOARDVersion 					"0405"

//调试版本 自动开启调试信息输出、新功能调试  (屏蔽即为发布版本)
#define DEBUG_MODE_VERSION

/******** 系统关键配置设置 ***********/

//接收与发送缓存
#define TCP_MAX_RCV_LEN  			(6*1024)
#define TCP_MAX_SEND_LEN  		(9*1024)


///与服务器交互设置
#define UPLOADDATA_TIME   		15        //n秒上传一次
#define UPLOADHEART_TIME   		(60*5)   	//n秒后一次心跳 

#define SERVER_COMM_TIMEOUT	 	20     		//每次交互超时



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






#endif