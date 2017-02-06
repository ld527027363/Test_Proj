/********************************************************
 * File	       :  protocol_485.h
 * Description :  485链路层协议
 * Author      :  左俊
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
 * Update      :  2015-06-26
 * History     :

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol_typedef.h"
#include "rs485.h"


static PProtocol_Layer_ST Self = 0;

extern unsigned short crc_ccitt(unsigned char *q, int len,unsigned short crc);

/********************************************************
 * function    :  Protocol_485_Init
 * Description :  485协议层初始化函数
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_485_Init(PProtocol_Layer_ST layer)
{
    if (!layer->down)
    {
        return;
    }

    layer->down->init(layer->down);
}

/********************************************************
 * function    :  Protocol_485_Pack
 * Description :  封包函数
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_485_Pack(PProtocol_Layer_ST layer)
{
    if (!layer->down)
    {
        layer->send(layer);
        return;
    }

    layer->down->pack(layer->down);
}

/********************************************************
 * function    :  Protocol_485_Unpack
 * Description :  解包函数
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_485_Unpack(PProtocol_Layer_ST layer)
{
    unsigned char* pbuffer;
    unsigned short datalen;
    if (!layer->down)
    {
        pbuffer = layer->linkrecv(&datalen);

        if (datalen == 0)
        {
            return;
        }
        layer->recv(layer,pbuffer,datalen);
        return;
    }

    layer->down->unpack(layer->down);
}

/********************************************************
 * function    :  Protocol_485_Getsize
 * Description :  获取待发送数据大小
 * param       :  layer  协议层对象
 * return      :  返回待发送数据大小
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static unsigned int Protocol_485_Getsize(PProtocol_Layer_ST layer)
{
    if (!layer->down)
    {
        return 0;
    }

    return layer->down->getsize(layer->down);
}

/********************************************************
 * function    :  Protocol_485_PutTag
 * Description :  向协议层中增加包体数据
 * param       :  layer  协议层对象
 * param       :  tag  应用数据标签
 * param       :  data  应用数据缓存
 * param       :  datalen  应用数据长度
 * return      :  0 表示成功 小于0 表示失败
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static int Protocol_485_PutTag(PProtocol_Layer_ST layer,Protocol_Tag_EM tag,unsigned char* data,unsigned short datalen)
{
    if (!layer->down)
    {
        return -1;
    }

    return layer->down->puttag(layer->down,tag,data,datalen);
}

/********************************************************
 * function    :  Protocol_485_Clear
 * Description :  清除485新协议层缓存
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_485_Clear(PProtocol_Layer_ST layer)
{

    memset(layer->ParamList,0,(int)PP_MAX * sizeof(Pchar));
    if (layer->data)
    {
        free(layer->data);
        layer->data =0;
    }

    if (!layer->down)
    {
        return;
    }

    layer->down->clear(layer->down);
}

/********************************************************
 * function    :  Protocol_485_SetParam
 * Description :  设置参数
 * param       :  layer  协议层对象
 * param       :  param  参数标签
 * param       :  value  参数值
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_485_SetParam(PProtocol_Layer_ST layer,Protocol_Param_EM param,Pchar value)
{
    if (param > PP_MAX || value == 0)
    {
        return;
    }

    layer->ParamList[param] = value;

    if (!layer->down)
    {
        return;
    }
    layer->down->setparam(layer->down,param,value);
}

/********************************************************
 * function    :  Protocol_485_GetParam
 * Description :  读取参数
 * param       :  layer  协议层对象
 * param       :  param  参数标签
 * return      :  参数值
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static Pchar Protocol_485_GetParam(PProtocol_Layer_ST layer,Protocol_Param_EM param)
{
    if (!layer->down)
    {
        return 0;
    }
    return layer->down->getparam(layer->down,param);
}

/********************************************************
 * function    :  Protocol_485_Send
 * Description :  发送数据
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/

void Protocol_485_Send_p1(PProtocol_Layer_ST layer)
{
	unsigned short bodysize = 0;
     unsigned char  head[3] = {0};
    ///执行发送处理
    if (layer->up)
    {
        bodysize = layer->up->getsize(layer->up);
    }
    bodysize += 3;
    head[0] = 0x02;
    head[1] = (unsigned char)((bodysize >> 8) & 0xFF);
    head[2] = (unsigned char)(bodysize & 0xFF);
    if (layer->linksend)
    {
        layer->linksend(head,3);
    }
    layer->crc = 0xFFFF;
    if (layer->up)
    {
        layer->up->setcrc(layer->up,layer->crc);
        layer->up->send(layer->up);
    }

    bodysize = layer->up->getcrc(layer->up);
    head[0] = (unsigned char)((bodysize >> 8) & 0xFF);
    head[1] = (unsigned char)(bodysize & 0xFF);
    head[2] = 0x03;
    if (layer->linksend)
    {
        layer->linksend(head,3);
    }
}

extern unsigned char  CheckSumEx(unsigned char *uBuff, unsigned int uBuffLen);
extern unsigned char g_arrRecvBuffer[];
extern unsigned char g_arrSendBuffer[];
extern unsigned short g_iRevStartPos;
extern unsigned short g_iSendLength ;
void Protocol_485_Send_p2(PProtocol_Layer_ST layer)
{
	  unsigned short bodysize = 0;
    unsigned char  head[6] = {0};
    ///执行发送处理
    if (layer->up)
    {
        bodysize = layer->up->getsize(layer->up);
    }
    //bodysize += 3;
		
		
//    head[0] = 0x02;
//    head[1] = (unsigned char)((bodysize >> 8) & 0xFF);
//    head[2] = (unsigned char)(bodysize & 0xFF);
		
		rs485_pack_head(head, RS485_CMD_SEND, NULL, bodysize);
		
    if (layer->linksend)
    {
        layer->linksend(head,(RS485_LEN_HEAD+RS485_LEN_OTHER));
    }
    layer->crc = 0xFFFF;
    if (layer->up)
    {
        layer->up->setcrc(layer->up,layer->crc);
        layer->up->send(layer->up);
    }

//    bodysize = layer->up->getcrc(layer->up);
//    head[0] = (unsigned char)((bodysize >> 8) & 0xFF);
//    head[1] = (unsigned char)(bodysize & 0xFF);
//    head[2] = 0x03;
		
		head[0] = CheckSumEx(g_arrSendBuffer, g_iSendLength);
    if (layer->linksend)
    {
        layer->linksend(head,1);
    }
	
}



static void Protocol_485_Send(PProtocol_Layer_ST layer)
{
     if (rs485_get_pro() == RS485_PRO_P1)
	 {
		 Protocol_485_Send_p1(layer);
	 }
	 else
	 {
		 Protocol_485_Send_p2(layer);
	 }
		 
}

#define PACK_HEAD 0x02
#define PACK_FOOT 0x03
/********************************************************
 * function    :  Protocol_485_Recv
 * Description :  接收服务器应答
 * param       :  layer  协议层对象
 * param       :  data  服务器应答数据缓存
 * param       :  datalen 服务器应答数据长度
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/

static void Protocol_485_Recv_p1(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
	    ///
    unsigned short packlen = 0;
    unsigned short crc = 0;
    unsigned short packcrc = 0;
    ///校验是否收到一个完整的包
    if  (data[0] != PACK_HEAD)  ///先判断包头是不是0x02
    {
        layer->linkfree();
        return;
    }

    packlen = (data[1] << 8) | data[2];
    if (datalen !=  (packlen +3) )  ///判断包的长度是否合法，包的长度是数据体+校验和+包尾0x03
    {
        return;
    }

    if (data[packlen + 2] != PACK_FOOT)  //判断包尾是不是0x03
    {
        layer->linkfree();
        return;
    }
//      printf("-8  %s",data);   //hcp123
    crc = crc_ccitt(&data[3],packlen-3,0);
    packcrc =  data[packlen] << 8 | data[packlen+1];

    if (crc != packcrc) ///判断校验和是否相等，校验和为包体数据，不包括 PACK_HEAD 、PACK_FOOT、长度以及校验和
    {
        layer->linkfree();
        return;
    }


    if (layer->up)
    {
        layer->up->recv(layer->up,&data[3],packlen-3);
    }
		
}
	

static void Protocol_485_Recv_p2(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
	    unsigned short packlen = 0;
    unsigned short crc = 0;
    unsigned short packcrc = 0;
    ///校验是否收到一个完整的包
    if  (data[0] != PACK_HEAD)  ///先判断包头是不是0x02
    {
        layer->linkfree();
        return;
    }

    packlen = (data[1] << 8) | data[2];
    if (datalen !=  (packlen +RS485_LEN_HEAD+RS485_LEN_CRC) )  ///判断包的长度是否合法，包的长度是数据体+校验和+包尾0x03
    {
        return;
    }

//    if (data[packlen + 2] != PACK_FOOT)  //判断包尾是不是0x03
//    {
//        layer->linkfree();
//        return;
//    }
		
//      printf("-8  %s",data);   //hcp123
   // crc = crc_ccitt(&data[3],packlen-3,0);
		crc = CheckSumEx(data, packlen +RS485_LEN_HEAD);
		
		
    //packcrc =  data[packlen] << 8 | data[packlen+1];
		packcrc = data[datalen-1];

    if (crc != packcrc) ///判断校验和是否相等，校验和为包体数据，不包括 PACK_HEAD 、PACK_FOOT、长度以及校验和
    {
        layer->linkfree();
        return;
    }


    if (layer->up)
    {
        layer->up->recv(layer->up,&data[RS485_IDX_DAT], packlen - RS485_LEN_OTHER);
    }
		
}

static void Protocol_485_Recv(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
     if (rs485_get_pro() == RS485_PRO_P1)
		 {
			 Protocol_485_Recv_p1(layer, data, datalen);
		 }
		 else
		 {
			 Protocol_485_Recv_p2(layer, data, datalen);
		 }
}

/********************************************************
 * function    :  Protocol_485_GetCrc
 * Description :  获取crc校验值
 * param       :  layer  协议层对象
 * return      :  crc校验值
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static unsigned short Protocol_485_GetCrc(PProtocol_Layer_ST layer)
{
    if (layer->up)
    {
        return layer->up->getcrc(layer->up);
    }

    return layer->crc;
}

/********************************************************
 * function    :  Protocol_485_SetCrc
 * Description :  设置crc校验值
 * param       :  layer  协议层对象
 * param       :  crc  crc校验值
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_485_SetCrc(PProtocol_Layer_ST layer,unsigned short crc)
{
    layer->crc = ~crc;
}

/********************************************************
 * function    :  Protocol_485_Initialize
 * Description :  创建485链路层协议对象
 * param       :  无
 * return      :  协议层对象
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
PProtocol_Layer_ST Protocol_485_Initialize(void)
{
    if (!Self)
    {
        Self = malloc(sizeof(*Self));
        memset(Self,0,sizeof(*Self));

        Self->init     = Protocol_485_Init;
        Self->pack     = Protocol_485_Pack;
        Self->unpack   = Protocol_485_Unpack;
        Self->getsize  = Protocol_485_Getsize;
        Self->puttag   = Protocol_485_PutTag;
        Self->clear    = Protocol_485_Clear;
        Self->setparam = Protocol_485_SetParam;
        Self->getparam = Protocol_485_GetParam;
        Self->send     = Protocol_485_Send;
        Self->recv     = Protocol_485_Recv;
        Self->getcrc   = Protocol_485_GetCrc;
        Self->setcrc   = Protocol_485_SetCrc;

        Self->ParamList = malloc((int)PP_MAX * sizeof(Pchar));
        memset(Self->ParamList,0,(int)PP_MAX * sizeof(Pchar));

    }
    return Self;
}


