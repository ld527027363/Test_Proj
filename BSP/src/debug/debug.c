/********************************************************
 * File	       :  debug.c
 * Description :  调试功能接口
 * Author      :  何超平
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
 * Creat       :  2015-11-20
 * Update      :  
 * History     :

*/

#include "global.h"

#include "SystemParameter.h"
#include "interface.h"
#include "rtc.h"
#include "debug.h"




#define DEBUG_LOCK() s_debug_info.bDoing = TRUE;
#define DEBUG_UNLOCK() s_debug_info.bDoing = FALSE;
#define DEBUG_CHECK_LOCK() (s_debug_info.bDoing)

#define debug_clear_msg()  s_debug_cnt--;
#define debug_check_msg()  s_debug_cnt
#define debug_get_last()   &s_debug_list[s_debug_cnt-1]

static struct debug_info_s  s_debug_list[DEBUG_LIST_MAX];
static u32_t  s_debug_cnt = 0,s_debug_idx = 0;
static debug_intf_t s_debug_intf[DEBUG_INTF_CNT];

BOOL TestReq = FALSE;

u8_t g_printf_buf[128] = {0};

///*
//** get set 调试模式开关
//*/
//void debug_set_mode(BOOL debug_mode)
//{
//	s_debug_info.mode = debug_mode;
//}

//u8_t debug_get_mode(void)
//{
//	return s_debug_info.mode;
//}




///*
//** get set 调试模式掩码
//*/
//void debug_set_mask(BOOL debug_mask)
//{
//	s_debug_info.mask = debug_mask;
//}

//u8_t debug_get_mask(void)
//{
//	return s_debug_info.mask;
//}

/*
** 设置调试信息发送接口
*/
void debug_set_output(__debug_send pSend)
{
	//s_debug_info.send = pSend;
}

/*
** 打印debug信息
*/
void debug_send(HANDLE hdl, u8_t *dat, u32_t len)
{
	//s_debug_info.send(dat, len);
	
	intf_send(hdl, dat, len);
}


/*
** 发送调试接口 消息  检测是否在处理其他
*/
void debug_post_msg(HANDLE hdl,u8_t * msg)
{
	  if (s_debug_cnt >= DEBUG_LIST_MAX)
		{
			return;  // full
		}
		
		s_debug_list[s_debug_cnt].intf_fun = &s_debug_intf[hdl];
		s_debug_list[s_debug_cnt].hdl = hdl;
		s_debug_list[s_debug_cnt].bRecv = TRUE;
		s_debug_list[s_debug_cnt].buf = msg;
	
		if (++s_debug_idx >= DEBUG_LIST_MAX)
		{
			s_debug_idx = 0;
		}
		s_debug_cnt++;
	
	//if(!DEBUG_CHECK_LOCK())
//	{
//		s_debug_info.intf_fun = &s_debug_intf[hdl];
//		s_debug_info.hdl = hdl;
//		s_debug_info.bRecv = TRUE;
//		s_debug_info.buf = msg;
//	}
//	//else
//	{
//		
//	}
}

/*
** 填充上报包 
*/
u32_t debug_pack_norpack(u8_t *pPack, u8_t cmd, u8_t *pDat, u32_t datLen, u8_t opt)
{
	u32_t ret = 0;
	
	memmove( &pPack[DEBUG_HEAD_LEN] , pDat, datLen);
	
	pPack[0] = DEBUG_PKT_HEAD_AP;
	pPack[1] = (DEBUG_HEAD_LEN-PKT_CHECKSUM_LEN)+datLen;
	pPack[2] = cmd;
	
  if (opt == 1)
	{
		pPack[DEBUG_HEAD_LEN+datLen] = CheckSum( &pPack[(DEBUG_HEAD_LEN-PKT_CHECKSUM_LEN)], PKT_CHECKSUM_LEN+datLen);
		datLen += PKT_CHECKSUM_LEN;
	}
	
	ret  = DEBUG_HEAD_LEN + datLen;
	
	return ret;
}


/*
** 填充上报 nrf1822 串口包 
*/
u32_t debug_pack_nrfpack(u8_t *pPack, u8_t cmd, u8_t *pDat, u32_t datLen)
{
	u32_t ret = 0;
	
	memmove(&pPack[NRF_HEAD_LEN] , pDat, datLen);
	
	pPack[0] = DEBUG_PKT_HEAD_NRF;
	pPack[1] = cmd;
	pPack[2] = ((PKT_CHECKSUM_LEN + datLen)>>8)&0xff;
	pPack[3] = ((PKT_CHECKSUM_LEN + datLen)>>0)&0xff;
	

	pPack[NRF_HEAD_LEN+datLen] = CheckSum( &pDat[NRF_HEAD_LEN], datLen);
	
	ret  = NRF_HEAD_LEN + datLen;
	
	return ret;
}


/*
** 填充上报 BLE包 
*/
u32_t debug_pack_blepack(u8_t *pPack, u8_t cmd, u8_t *pDat, u32_t datLen)
{
	u32_t ret = 0;
	
	memmove(&pPack[BLE_HEAD_LEN] , pDat, datLen);
	
	pPack[0] = cmd;
	pPack[1] = ((PKT_CHECKSUM_LEN + datLen)>>8)&0xff;
	pPack[2] = ((PKT_CHECKSUM_LEN + datLen)>>0)&0xff;
	
	ret  = BLE_HEAD_LEN + datLen;

	return ret;
}



/*
** 填充上报 BLE 调试包 
*/
u32_t debug_pack_nrfblepack(u8_t *pPack, u8_t cmd, u8_t *pDat, u32_t datLen, u8_t opt)
{
	u32_t ret = 0;
	
	/* debug层 */
	ret  = debug_pack_norpack(pPack,cmd,pDat,datLen,opt);

	/* BLE层*/
	ret = debug_pack_blepack(pPack, BLE_CMD_PUSH, pPack,ret);
	
	/* NRF层*/
	ret = debug_pack_nrfpack(pPack, NRF_TAG_BLE, pPack,ret);
	
	return ret;
}





/*
** 检测调试包 并检测校验 去掉头部
*/
BOOL debug_uppack_nrfpack(const u8_t *pPack, u8_t **pDat)
{
	debug_nrfpack_t * pNrfPack;
	u32_t len;
	u8_t checksum = 0;
	BOOL bRet = FALSE;
	
	pNrfPack = (debug_nrfpack_t *)pPack;
	
	//检查一下校验命令
	if (NRF_TAG_BLE_DOWN == pNrfPack->cmd)
	{
		
		len = (pNrfPack->len_h << 8) + pNrfPack->len_l;
		//检查一下校验checksum
		checksum = CheckSum(pNrfPack->dat, len);
		if (pNrfPack->dat[len] == checksum)
		{
			*pDat = pNrfPack->dat;
			bRet = TRUE;
		}
	}
	
	return bRet;
}

BOOL debug_uppack_blepack(const u8_t *pPack, u8_t **pDat)
{
	debug_blepack_t * pBlePack;
//	u32_t len;
//	u8_t checksum = 0;
	BOOL bRet = FALSE;
	
	pBlePack = (debug_blepack_t *)pPack;
	
	//检查一下校验命令
	if (BLE_CMD_PUSH == pBlePack->cmd)
	{	
		*pDat = pBlePack->dat;
		bRet = TRUE;	
	}
	
	return bRet;
}

BOOL debug_uppack_nrfblepack(const u8_t *pPack, u8_t **pDat)
{
	BOOL bRet = FALSE;
//	debug_pack_t * pTmpdebugPkt;
	u8_t * pTempNrf;
	
	//校验内容包括  长度+命令+数据
	bRet = debug_uppack_nrfpack(pPack, &pTempNrf);
	if (bRet)
	{
		//ble
		bRet = debug_uppack_blepack(pTempNrf, pDat);
		if (bRet)
		{
			//normal
			bRet = TRUE ; //校验正确
		}
	
	}
	
	return bRet;
}

BOOL debug_uppack_norpack(const u8_t *pPack, u8_t **pDat)
{
	BOOL bRet = FALSE;
	u8_t sum = 0;
	debug_pack_t * pTmpdebugPkt;
	
	pTmpdebugPkt = (debug_pack_t *)pPack;
	//校验内容包括  长度+命令+数据
	sum = CheckSum(&pTmpdebugPkt->cmd, pTmpdebugPkt->len-1);
	if (sum == pTmpdebugPkt->dat[pTmpdebugPkt->len - DEBUG_PKT_MIN_SIZE + DEBUG_HEAD_LEN-PKT_CHECKSUM_LEN])
	{
		*pDat = (u8_t *)pTmpdebugPkt;//pTmpdebugPkt->dat;
		bRet = TRUE ; //校验正确
	}
	
	return bRet;
	
}


/*
** debug 接口绑定 不同的处理方法
*/
void debug_bind(HANDLE hdl, __debug_unpack uppack, __debug_pack pack)
{
	s_debug_intf[hdl].unpack = uppack;
	s_debug_intf[hdl].pack   = pack;
}



/*
** debug 接口初始化
*/
void debug_init(void)
{
	u32_t i = 0;
	
	for (i=0; i<DEBUG_INTF_CNT; i++)
	{
		s_debug_list[i].index = i;
	}
	
	debug_bind(intf_get_handle(INTF_FLAG_UART1), debug_uppack_norpack, debug_pack_norpack);
	debug_bind(intf_get_handle(INTF_FLAG_SERVER), debug_uppack_norpack, debug_pack_norpack);
	debug_bind(intf_get_handle(INTF_FLAG_WIFI), debug_uppack_norpack, debug_pack_norpack);
	
	debug_bind(intf_get_handle(INTF_FLAG_BLE), debug_uppack_nrfblepack, debug_pack_nrfblepack);
	
//	//s_debug_intf[DEBUG_INTF_UART1].send  = DEBUG_INTF_UART1_SEND;
//	s_debug_intf[DEBUG_INTF_UART1].unpack = DEBUG_INTF_UART1_CHECK;
//	s_debug_intf[DEBUG_INTF_UART1].pack  = DEBUG_INTF_UART1_MAKE;
//	
//	//s_debug_intf[DEBUG_INTF_NET].send  = DEBUG_INTF_NET_SEND;
//	s_debug_intf[DEBUG_INTF_NET].unpack = DEBUG_INTF_NET_CHECK;
//	s_debug_intf[DEBUG_INTF_NET].pack  = DEBUG_INTF_NET_MAKE;
//	
//	//s_debug_intf[DEBUG_INTF_BLE].send  = DEBUG_INTF_BLE_SEND;
//	s_debug_intf[DEBUG_INTF_BLE].unpack = DEBUG_INTF_BLE_CHECK;
//	s_debug_intf[DEBUG_INTF_BLE].pack  = DEBUG_INTF_BLE_MAKE;
//	
//	//s_debug_intf[DEBUG_INTF_WIFI].send  = DEBUG_INTF_BLE_SEND;
//	s_debug_intf[DEBUG_INTF_WIFI].unpack = DEBUG_INTF_WIFI_CHECK;
//	s_debug_intf[DEBUG_INTF_WIFI].pack  = DEBUG_INTF_WIFI_MAKE;
//	
//	s_debug_info.intf_fun = s_debug_intf;
}



/*
** 调试接口处理
*/
void debug_process(void)
{
	debug_pack_t * pTmpdebugPkt ;
	u8_t sendBuf[DEBUG_PKT_MAX_SIZE] = {0};
	u8_t sendLen = 0;
	u8_t *pDat = 0;
	u8_t retCmd = 0;
	debug_info_p pDebugInfo;
	
	if (debug_check_msg())
	{	
		/* 锁定 单次处理 */
		//DEBUG_LOCK();
		
		/* 获取最近的debug */
		pDebugInfo = debug_get_last();
		
		/* 检测命令格式 并去掉包头 */
		if (pDebugInfo->intf_fun->unpack(pDebugInfo->buf,&pDat))
		{
			pTmpdebugPkt = (debug_pack_t *)pDat;
			
			intf_recv(pDebugInfo->hdl, pTmpdebugPkt->dat, pTmpdebugPkt->len);
			
			switch(pTmpdebugPkt->cmd)
			{
				case DEBUG_CMD_GET_PRAM:   //获取参数
						sendLen = system_get_param(&pDat);
						retCmd = DEBUG_RET_CMD_MASK|DEBUG_CMD_GET_PRAM;
					break;
				
				case DEBUG_CMD_SET_PRAM:	//设置参数
					retCmd = system_set_param(pTmpdebugPkt->dat, pTmpdebugPkt->len);
				  if (retCmd)
					{
						retCmd = DEBUG_RET_CMD_OK;
					}
					else
					{
						retCmd = DEBUG_RET_CMD_ERROR;
					}
					break;
				
				case DEBUG_CMD_GET_TIME:  //time
					sendLen = rtc_get_curTime((char *)sendBuf, "YY-MM-DD hh:mm:ss");
					retCmd = DEBUG_RET_CMD_MASK|DEBUG_CMD_GET_TIME;
					break;
				
				case DEBUG_CMD_GET_LOG:
					
					break;
				
				case DEBUG_CMD_GET_FLASH_CNT:
					
					break;
				
				case DEBUG_CMD_GET_TAG:
					
					break;
				
				case DEBUG_CMD_GET_STATE: //aa 02 07 f9
					
					break;

				
				case DEBUG_CMD_RESTART:  //aa 02 09 f7
					
					break;
				case DEBUG_CMD_ENTER_MODE:
					
					break;
				
				case DEBUG_CMD_EXIT_MODE:
					
					break;
				
				case DEBUG_CMD_GET_VERSION://aa 02 22 DE
					sendLen = sizeof(VERSION_STR)/sizeof(char);
					strcpy((char *)sendBuf, VERSION_STR);
					retCmd = DEBUG_RET_CMD_MASK|DEBUG_CMD_GET_VERSION;
				
					break;
				
				
				case DEBUG_CMD_UPGRADE_START:
					
					break;
				
				case DEBUG_CMD_UPGRADE_ING:
					
					break;
				
				case DEBUG_CMD_UPGRADE_END:
					
					break;
				
				
				case DEBUG_CMD_ERASE_ALL:
					
					break;
				
				case DEBUG_CMD_ERASE_LOG:
					
					break;
				
				case DEBUG_CMD_ERASE_TAG:
					
					break;  
				
				case DEBUG_CMD_WRITE_1KTAG: //aa 02 0C f4
					
					break; 
				case DEBUG_CMD_RF_CHANN:
				
					break;
				
			}
			
			//构造应答报文  BLE 不用校验 debug层
			if (intf_get_type(pDebugInfo->hdl) == INTF_TYPE_BLE)
			{
				sendLen = pDebugInfo->intf_fun->pack(sendBuf, retCmd, sendBuf, sendLen, FALSE);
			}
			else
			{
				sendLen = pDebugInfo->intf_fun->pack(sendBuf, retCmd, sendBuf, sendLen, TRUE);
			}
			
			//发送
			intf_send(pDebugInfo->hdl, sendBuf, sendLen);
		}
		else
		{
			//回调处理失败消息 参数全为0
			intf_recv(pDebugInfo->hdl, NULL, NULL);
		}

		debug_clear_msg();
	}
	
	
	//DEBUG_UNLOCK();
}

