/********************************************************
 * File	       :  protocol_encrypt.h
 * Description :  协议加密解密层，负责对应用协议进行加密和解密处理
 * Author      :  左俊
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
 * Update      :  2015-06-26
 * History     :

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol_typedef.h"


#define ENCRYPT (1)
#define DECRYPT (0)

static PProtocol_Layer_ST Self = 0;

extern int tripledes(unsigned char *key,unsigned char *src,unsigned char *dest,int m);
extern unsigned short crc_ccitt(unsigned char *q, int len,unsigned short crc);

/********************************************************
 * function    :  Protocol_Encrypt_Init
 * Description :  协议层初始化函数
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Encrypt_Init(PProtocol_Layer_ST layer)
{
    if (!layer->down)
    {
        return;
    }
    layer->down->init(layer->down);
}

/********************************************************
 * function    :  Protocol_Encrypt_Pack
 * Description :  封包函数
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Encrypt_Pack(PProtocol_Layer_ST layer)
{
    if (!layer->down)
    {
        layer->send(layer);
        return;
    }

    layer->down->pack(layer->down);
}

/********************************************************
 * function    :  Protocol_Encrypt_Unpack
 * Description :  解包函数
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Encrypt_Unpack(PProtocol_Layer_ST layer)
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
 * function    :  Protocol_Encrypt_Getsize
 * Description :  获取待发送数据长度
 * param       :  layer  协议层对象
 * return      :  待发送数据长度
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static unsigned int Protocol_Encrypt_Getsize(PProtocol_Layer_ST layer)
{
    unsigned int ret = 0;

    if (layer->up)
    {
        ret += layer->up->getsize(layer->up);
    }

    if (layer->ParamList[PP_TYPE])
    {
        if (!strcmp((Pchar)layer->ParamList[PP_TYPE],"3"))
        {
            ////需要把数据扩充到8的倍数
            ret = (ret + 7) / 8 *8;
        }
    }
    return   ret;
}

/********************************************************
 * function    :  Protocol_Encrypt_PutTag
 * Description :  添加数据到应用协议包体中
  * param       :  layer  协议层对象
 * param       :  tag  应用数据标签
 * param       :  data  数据
 * param       :  datalen  数据长度
 * return      :  0 表示成功 小于0 表示失败
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static int Protocol_Encrypt_PutTag(PProtocol_Layer_ST layer,Protocol_Tag_EM tag,unsigned char* data,unsigned short datalen)
{
    if (!layer->down)
    {
        return -1;
    }
    return layer->down->puttag(layer->down,tag,data,datalen);
}

/********************************************************
 * function    :  Protocol_Encrypt_Clear
 * Description :  清除加解密协议层缓存
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Encrypt_Clear(PProtocol_Layer_ST layer)
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
 * function    :  Protocol_Encrypt_SetParam
 * Description :  设置参数
 * param       :  layer  协议层对象
 * param       :  param  参数标签
 * param       :  value  参数值
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Encrypt_SetParam(PProtocol_Layer_ST layer,Protocol_Param_EM param,Pchar value)
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
 * function    :  Protocol_Encrypt_GetParam
 * Description :  应用协议包头协议层初始化函数
 * param       :  layer  协议层对象
 * param       :  param  参数标签
 * return      :  返回参数值指针
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static Pchar Protocol_Encrypt_GetParam(PProtocol_Layer_ST layer,Protocol_Param_EM param)
{
    if (!layer->down)
    {
        return 0;
    }
    return layer->down->getparam(layer->down,param);
}

/********************************************************
 * function    :  Protocol_Encrypt
 * Description :  加密待发送数据
 * param       :  layer  协议层对象
 * param       :  data  待加密数据
 * param       :  datalen  待加密数据长度
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Encrypt(PProtocol_Layer_ST layer,Pchar data,unsigned short datalen)
{

    if (!(Pchar)layer->ParamList[PP_SESSIONKEY])
    {
        return;
    }
    while(datalen)
    {
        tripledes((unsigned char*)layer->ParamList[PP_SESSIONKEY],(unsigned char*)data,(unsigned char*)data,ENCRYPT);
        data += 8;
        datalen -= 8;
    }
}

/********************************************************
 * function    :  Protocol_Decrypt
 * Description :  解密待发送数据
 * param       :  layer  协议层对象
 * param       :  data  待解密数据
 * param       :  datalen  待解密数据长度
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Decrypt(PProtocol_Layer_ST layer,Pchar data,unsigned short datalen)
{
    unsigned short bufferlen = 0;
    Pchar buffer = data;
    if (!(Pchar)layer->ParamList[PP_SESSIONKEY])
    {
        return;
    }
    bufferlen = (datalen +7) /8 * 8;
   // printf("datalen:%d,DECRYPT_Data:%s\r\n",datalen,data);
    //printf("PP_SESSIONKEY_Encrypt:%s\r\n",layer->ParamList[PP_SESSIONKEY]);
    while(bufferlen)
    {
        //tripledes((unsigned char*)layer->ParamList[PP_SESSIONKEY],(unsigned char*)data,(unsigned char*)data,DECRYPT);
        tripledes((unsigned char*)layer->ParamList[PP_SESSIONKEY],(unsigned char*)buffer,(unsigned char*)buffer,DECRYPT);
        buffer += 8;
        bufferlen -= 8;
    }
   // printf("DECRYPT_Data:%s\r\n",data);
}

/********************************************************
 * function    :  Protocol_Encrypt_Send
 * Description :  发送数据
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Encrypt_Send(PProtocol_Layer_ST layer)
{
    Pchar buff = 0;
    unsigned short bufflen = 0;
    unsigned short index = 0;
    Pchar param = layer->ParamList[PP_TYPE];
    if (param)
    {
        if (!strcmp(param,"3"))
        {
            ///需要在这里进行数据加密发送
            #define PACK_SIZE (1024)
            buff = malloc(PACK_SIZE);
            if (!buff)
            {
                return;	
            }
            if (layer->up)
            {
                while(1)
                {
                    memset(buff,0,PACK_SIZE);
                    bufflen = layer->up->get(layer->up,index,(unsigned char*)buff,PACK_SIZE);
                    if (bufflen == 0)
                    {
                        break;
                    }
                    index += bufflen;
                    bufflen =  (bufflen + 7) / 8 * 8;
                    ///进行加密处理，然后再进行发送
                    Protocol_Encrypt(layer,buff,bufflen);
                    if (layer->linksend)
                    {
                        ///send之前先计算crc16

                        layer->crc = crc_ccitt((unsigned char*)buff,bufflen,layer->crc);
											  layer->setcrc(layer,layer->crc);
                        layer->linksend((unsigned char*)buff,bufflen);
                    }
                }
            }
            #undef PACK_SIZE
            free(buff);
						layer->setcrc(layer,layer->crc);
            //layer->up->clear(layer->up);
            return;
        }
        else
        {
            layer->setcrc(layer,layer->crc);
        }
    }

    ///如果不需要加密，则交由上层自己去发送
    if (layer->up)
    {
        layer->up->setcrc(layer->up,layer->crc);
        layer->up->send(layer->up);
    }
    #undef PACK_SIZE
}

/********************************************************
 * function    :  Protocol_Encrypt_Recv
 * Description :  接收服务器应答
 * param       :  layer  协议层对象
 * param       :  data  服务器应答数据缓存
 * param       :  datalen  服务器应答数据缓存长度
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Encrypt_Recv(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
    Pchar param = layer->ParamList[PP_TYPE];
    if (param)
    {
        if (!strcmp(param,"3"))
        {
            if (layer->up)
            {
						
                ///进行解密处理，然后再进行发送
                Protocol_Decrypt(layer,(Pchar)data,datalen);
							
            }
        }
    }

    if (layer->up)
    {
        layer->up->recv(layer->up,data,datalen);
    }
}

/********************************************************
 * function    :  Protocol_Encrypt_GetCrc
 * Description :  获取crc校验值
 * param       :  layer  协议层对象
 * return      :  crc校验值
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static unsigned short Protocol_Encrypt_GetCrc(PProtocol_Layer_ST layer)
{
    Pchar param = layer->ParamList[PP_TYPE];
    if (param)
    {

        if (!strcmp(param,"3"))
        {
            return layer->crc;
        }
    }

    if (layer->up)
    {
        return layer->up->getcrc(layer->up);
    }

    return 0;
}

/********************************************************
 * function    :  Protocol_Encrypt_SetCrc
 * Description :  设置crc校验值
 * param       :  layer  协议层对象
 * param       :  crc  crc校验值
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Encrypt_SetCrc(PProtocol_Layer_ST layer,unsigned short crc)
{
    layer->crc = ~crc;    
}

/********************************************************
 * function    :  Protocol_Encrypt_Initialize
 * Description :  创建加解密协议层对象
 * param       :  无
 * return      :  协议层对象
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
PProtocol_Layer_ST Protocol_Encrypt_Initialize(void)
{

    if (!Self)
    {
        Self = malloc(sizeof(*Self));
        memset(Self,0,sizeof(*Self));

        Self->init     = Protocol_Encrypt_Init;
        Self->pack     = Protocol_Encrypt_Pack;
        Self->unpack   = Protocol_Encrypt_Unpack;
        Self->getsize  = Protocol_Encrypt_Getsize;
        Self->puttag   = Protocol_Encrypt_PutTag;
        Self->clear    = Protocol_Encrypt_Clear;
        Self->setparam = Protocol_Encrypt_SetParam;
        Self->getparam = Protocol_Encrypt_GetParam;
        Self->send     = Protocol_Encrypt_Send;
        Self->recv     = Protocol_Encrypt_Recv;
        Self->getcrc   = Protocol_Encrypt_GetCrc;
        Self->setcrc   = Protocol_Encrypt_SetCrc;

        Self->ParamList = malloc((int)PP_MAX * sizeof(Pchar));
        memset(Self->ParamList,0,(int)PP_MAX * sizeof(Pchar));
    }


    return Self;
}




