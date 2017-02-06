/********************************************************
 * File	       :  protocol_485.h
 * Description :  485��·��Э��
 * Author      :  ��
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾
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
 * Description :  485Э����ʼ������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
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
 * Description :  �������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
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
 * Description :  �������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
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
 * Description :  ��ȡ���������ݴ�С
 * param       :  layer  Э������
 * return      :  ���ش��������ݴ�С
 * Author      :  ��
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
 * Description :  ��Э��������Ӱ�������
 * param       :  layer  Э������
 * param       :  tag  Ӧ�����ݱ�ǩ
 * param       :  data  Ӧ�����ݻ���
 * param       :  datalen  Ӧ�����ݳ���
 * return      :  0 ��ʾ�ɹ� С��0 ��ʾʧ��
 * Author      :  ��
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
 * Description :  ���485��Э��㻺��
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
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
 * Description :  ���ò���
 * param       :  layer  Э������
 * param       :  param  ������ǩ
 * param       :  value  ����ֵ
 * return      :  ��
 * Author      :  ��
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
 * Description :  ��ȡ����
 * param       :  layer  Э������
 * param       :  param  ������ǩ
 * return      :  ����ֵ
 * Author      :  ��
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
 * Description :  ��������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/

void Protocol_485_Send_p1(PProtocol_Layer_ST layer)
{
	unsigned short bodysize = 0;
     unsigned char  head[3] = {0};
    ///ִ�з��ʹ���
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
    ///ִ�з��ʹ���
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
 * Description :  ���շ�����Ӧ��
 * param       :  layer  Э������
 * param       :  data  ������Ӧ�����ݻ���
 * param       :  datalen ������Ӧ�����ݳ���
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/

static void Protocol_485_Recv_p1(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
	    ///
    unsigned short packlen = 0;
    unsigned short crc = 0;
    unsigned short packcrc = 0;
    ///У���Ƿ��յ�һ�������İ�
    if  (data[0] != PACK_HEAD)  ///���жϰ�ͷ�ǲ���0x02
    {
        layer->linkfree();
        return;
    }

    packlen = (data[1] << 8) | data[2];
    if (datalen !=  (packlen +3) )  ///�жϰ��ĳ����Ƿ�Ϸ������ĳ�����������+У���+��β0x03
    {
        return;
    }

    if (data[packlen + 2] != PACK_FOOT)  //�жϰ�β�ǲ���0x03
    {
        layer->linkfree();
        return;
    }
//      printf("-8  %s",data);   //hcp123
    crc = crc_ccitt(&data[3],packlen-3,0);
    packcrc =  data[packlen] << 8 | data[packlen+1];

    if (crc != packcrc) ///�ж�У����Ƿ���ȣ�У���Ϊ�������ݣ������� PACK_HEAD ��PACK_FOOT�������Լ�У���
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
    ///У���Ƿ��յ�һ�������İ�
    if  (data[0] != PACK_HEAD)  ///���жϰ�ͷ�ǲ���0x02
    {
        layer->linkfree();
        return;
    }

    packlen = (data[1] << 8) | data[2];
    if (datalen !=  (packlen +RS485_LEN_HEAD+RS485_LEN_CRC) )  ///�жϰ��ĳ����Ƿ�Ϸ������ĳ�����������+У���+��β0x03
    {
        return;
    }

//    if (data[packlen + 2] != PACK_FOOT)  //�жϰ�β�ǲ���0x03
//    {
//        layer->linkfree();
//        return;
//    }
		
//      printf("-8  %s",data);   //hcp123
   // crc = crc_ccitt(&data[3],packlen-3,0);
		crc = CheckSumEx(data, packlen +RS485_LEN_HEAD);
		
		
    //packcrc =  data[packlen] << 8 | data[packlen+1];
		packcrc = data[datalen-1];

    if (crc != packcrc) ///�ж�У����Ƿ���ȣ�У���Ϊ�������ݣ������� PACK_HEAD ��PACK_FOOT�������Լ�У���
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
 * Description :  ��ȡcrcУ��ֵ
 * param       :  layer  Э������
 * return      :  crcУ��ֵ
 * Author      :  ��
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
 * Description :  ����crcУ��ֵ
 * param       :  layer  Э������
 * param       :  crc  crcУ��ֵ
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_485_SetCrc(PProtocol_Layer_ST layer,unsigned short crc)
{
    layer->crc = ~crc;
}

/********************************************************
 * function    :  Protocol_485_Initialize
 * Description :  ����485��·��Э�����
 * param       :  ��
 * return      :  Э������
 * Author      :  ��
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


