/********************************************************
 * File	       :  protocol_encrypt.h
 * Description :  Э����ܽ��ܲ㣬�����Ӧ��Э����м��ܺͽ��ܴ���
 * Author      :  ��
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾
 * Update      :  2015-06-26
 * History     :

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol_typedef.h"


#define ENCRYPT (1)
#define DECRYPT (0)

static PProtocol_Layer_ST this = 0;

extern int tripledes(unsigned char *key,unsigned char *src,unsigned char *dest,int m);
extern unsigned short crc_ccitt(unsigned char *q, int len,unsigned short crc);

/********************************************************
 * function    :  Protocol_Encrypt_Init
 * Description :  Э����ʼ������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
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
 * Description :  �������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
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
 * Description :  �������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
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
 * Description :  ��ȡ���������ݳ���
 * param       :  layer  Э������
 * return      :  ���������ݳ���
 * Author      :  ��
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
            ////��Ҫ���������䵽8�ı���
            ret = (ret + 7) / 8 *8;
        }
    }
    return   ret;
}

/********************************************************
 * function    :  Protocol_Encrypt_PutTag
 * Description :  ������ݵ�Ӧ��Э�������
  * param       :  layer  Э������
 * param       :  tag  Ӧ�����ݱ�ǩ
 * param       :  data  ����
 * param       :  datalen  ���ݳ���
 * return      :  0 ��ʾ�ɹ� С��0 ��ʾʧ��
 * Author      :  ��
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
 * Description :  ����ӽ���Э��㻺��
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Encrypt_Clear(PProtocol_Layer_ST layer)
{
    memset(layer->ParamList,0,PP_MAX * sizeof(Pchar));

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
 * Description :  ���ò���
 * param       :  layer  Э������
 * param       :  param  ������ǩ
 * param       :  value  ����ֵ
 * return      :  ��
 * Author      :  ��
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
 * Description :  Ӧ��Э���ͷЭ����ʼ������
 * param       :  layer  Э������
 * param       :  param  ������ǩ
 * return      :  ���ز���ֵָ��
 * Author      :  ��
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
 * Description :  ���ܴ���������
 * param       :  layer  Э������
 * param       :  data  ����������
 * param       :  datalen  ���������ݳ���
 * return      :  ��
 * Author      :  ��
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
 * Description :  ���ܴ���������
 * param       :  layer  Э������
 * param       :  data  ����������
 * param       :  datalen  ���������ݳ���
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Decrypt(PProtocol_Layer_ST layer,Pchar data,unsigned short datalen)
{
    unsigned short bufferlen = 0;
    if (!(Pchar)layer->ParamList[PP_SESSIONKEY])
    {
        return;
    }
    bufferlen = (datalen +7) /8 * 8;
    while(bufferlen)
    {
        tripledes((unsigned char*)layer->ParamList[PP_SESSIONKEY],(unsigned char*)data,(unsigned char*)data,DECRYPT);
        data += 8;
        bufferlen -= 8;
    }
}

/********************************************************
 * function    :  Protocol_Encrypt_Send
 * Description :  ��������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
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
            ///��Ҫ������������ݼ��ܷ���
            #define PACK_SIZE (1024)
            buff = malloc(PACK_SIZE);
            if (layer->up)
            {
                while(1)
                {
                    memset(buff,0,PACK_SIZE);
                    bufflen = layer->up->get(layer->up,index,buff,PACK_SIZE);
                    if (bufflen == 0)
                    {
                        break;
                    }
                    index += bufflen;
                    bufflen =  (bufflen + 7) / 8 * 8;
                    ///���м��ܴ���Ȼ���ٽ��з���
                    Protocol_Encrypt(layer,buff,bufflen);
                    if (layer->linksend)
                    {
                        ///send֮ǰ�ȼ���crc16

                        layer->crc = crc_ccitt(buff,bufflen,layer->crc);
                        layer->linksend(buff,bufflen);
                    }
                }
            }
            #undef PACK_SIZE
            free(buff);
            //layer->up->clear(layer->up);
            return;
        }
        else
        {
            layer->setcrc(layer,layer->crc);
        }
    }

    ///�������Ҫ���ܣ������ϲ��Լ�ȥ����
    if (layer->up)
    {
        layer->up->setcrc(layer->up,layer->crc);
        layer->up->send(layer->up);
    }
    #undef PACK_SIZE
}

/********************************************************
 * function    :  Protocol_Encrypt_Recv
 * Description :  ���շ�����Ӧ��
 * param       :  layer  Э������
 * param       :  data  ������Ӧ�����ݻ���
 * param       :  datalen  ������Ӧ�����ݻ��泤��
 * return      :  ��
 * Author      :  ��
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
                ///���н��ܴ���Ȼ���ٽ��з���
                Protocol_Decrypt(layer,data,datalen);
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
 * Description :  ��ȡcrcУ��ֵ
 * param       :  layer  Э������
 * return      :  crcУ��ֵ
 * Author      :  ��
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
 * Description :  ����crcУ��ֵ
 * param       :  layer  Э������
 * param       :  crc  crcУ��ֵ
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Encrypt_SetCrc(PProtocol_Layer_ST layer,unsigned short crc)
{
    layer->crc = ~crc;    
}

/********************************************************
 * function    :  Protocol_Encrypt_Initialize
 * Description :  �����ӽ���Э������
 * param       :  ��
 * return      :  Э������
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
PProtocol_Layer_ST Protocol_Encrypt_Initialize(void)
{

    if (!this)
    {
        this = malloc(sizeof(*this));
        memset(this,0,sizeof(*this));

        this->init     = Protocol_Encrypt_Init;
        this->pack     = Protocol_Encrypt_Pack;
        this->unpack   = Protocol_Encrypt_Unpack;
        this->getsize  = Protocol_Encrypt_Getsize;
        this->puttag   = Protocol_Encrypt_PutTag;
        this->clear    = Protocol_Encrypt_Clear;
        this->setparam = Protocol_Encrypt_SetParam;
        this->getparam = Protocol_Encrypt_GetParam;
        this->send     = Protocol_Encrypt_Send;
        this->recv     = Protocol_Encrypt_Recv;
        this->getcrc   = Protocol_Encrypt_GetCrc;
        this->setcrc   = Protocol_Encrypt_SetCrc;

        this->ParamList = malloc(PP_MAX * sizeof(Pchar));
        memset(this->ParamList,0,PP_MAX * sizeof(Pchar));
    }


    return this;
}


