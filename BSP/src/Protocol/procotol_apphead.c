/********************************************************
 * File	       :  protocol_apphead.h
 * Description :  Ӧ��Э��㣬��ͷ����
 * Author      :  ��
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾
 * Update      :  2015-06-26
 * History     :

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol_typedef.h"





#define HTTP_HEAD_MAX (1024)

static PProtocol_Layer_ST Self = 0;

const char* APPLICATION_HEAD_BEGIN = "{\"magic\":\"%s\",\"type\":\"%s\",\"account\":\"%s\",\"session_id\":\"%s\",\"create_time\":\"%s\"," \
                                     "\"create_date\":\"%s\",\"action\":\"%s\",\"sequence_id\":\"%s\"}";


extern unsigned short crc_ccitt(unsigned char *q, int len,unsigned short crc);

/********************************************************
 * function    :  Protocol_AppHead_Init
 * Description :  Ӧ��Э���ͷЭ����ʼ������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_AppHead_Init(PProtocol_Layer_ST layer)
{
    if (!layer->down)
    {
        return;
    }

    layer->down->init(layer->down);
}

/********************************************************
 * function    :  Protocol_AppHead_CreateParam
 * Description :  ����Ӧ�ð�ͷ������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_AppHead_CreateParam(PProtocol_Layer_ST layer)
{

     sprintf(layer->data,APPLICATION_HEAD_BEGIN, layer->ParamList[PP_MAGIC],layer->ParamList[PP_TYPE]
                                               ,layer->ParamList[PP_ACCOUNT],layer->ParamList[PP_SESSIONID]
                                               ,layer->ParamList[PP_CREATETIME],layer->ParamList[PP_CREATEDATE]
                                               ,layer->ParamList[PP_ACTION],layer->ParamList[PP_SEQUENCEID]);
     layer->datalen = (unsigned short)strlen(layer->data);
}


/********************************************************
 * function    :  Protocol_AppHead_Pack
 * Description :  �������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_AppHead_Pack(PProtocol_Layer_ST layer)
{
    ///����Ҫ���͵�������֯��

    assert_null(layer->ParamList[PP_ACCOUNT]);
    assert_null(layer->ParamList[PP_MAGIC]);
    assert_null(layer->ParamList[PP_TYPE]);
    assert_null(layer->ParamList[PP_SESSIONID]);
    assert_null(layer->ParamList[PP_CREATEDATE]);
    assert_null(layer->ParamList[PP_CREATETIME]);
    assert_null(layer->ParamList[PP_SEQUENCEID]);
    assert_null(layer->ParamList[PP_WEBTYPE]);
    assert_null(layer->ParamList[PP_ACTION]);
        

    if (layer->data)
    {
        free(layer->data);
        layer->data =0;
    }

    layer->data = (Pchar)malloc(HTTP_HEAD_MAX);
    memset(layer->data,0,HTTP_HEAD_MAX);

    ///���������б�
    Protocol_AppHead_CreateParam(layer);

    if (!layer->down)
    {
        layer->send(layer);
        return;
    }


    if (layer->down)
    {
        layer->down->pack(layer->down);
    }
}

/********************************************************
 * function    :  Protocol_AppHead_Unpack
 * Description :  �������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_AppHead_Unpack(PProtocol_Layer_ST layer)
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
 * function    :  Protocol_AppHead_Getsize
 * Description :  ��ȡ���ͻ������ݰ���С
 * param       :  layer  Э������
 * return      :  ���ͻ������ݴ�С
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned int Protocol_AppHead_Getsize(PProtocol_Layer_ST layer)
{
    unsigned int ret = 0;

    if (layer->data)
    {
        ret = layer->datalen;
    }

    if (layer->up)
    {
        ret += layer->up->getsize(layer->up);
    }
    return   ret;
}

/********************************************************
 * function    :  Protocol_AppHead_PutTag
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
static int Protocol_AppHead_PutTag(PProtocol_Layer_ST layer,Protocol_Tag_EM tag,unsigned char* data,unsigned short datalen)
{
    if (!layer->down)
    {
        return -1;
    }
    return layer->down->puttag(layer->down,tag,data,datalen);
}

/********************************************************
 * function    :  Protocol_AppHead_Clear
 * Description :  ���Ӧ��Э���ͷЭ��㻺��
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_AppHead_Clear(PProtocol_Layer_ST layer)
{
    memset((void*)layer->ParamList,0,(int)PP_MAX * sizeof(Pchar));
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
 * function    :  Protocol_AppHead_SetParam
 * Description :  ���ò���
 * param       :  layer  Э������
 * param       :  param  ������ǩ
 * param       :  value  ����ֵ
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_AppHead_SetParam(PProtocol_Layer_ST layer,Protocol_Param_EM param,Pchar value)
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
 * function    :  Protocol_AppHead_GetParam
 * Description :  ��ȡ����
 * param       :  layer  Э������
 * param       :  param  ������ǩ
 * return      :  ���ز���ֵָ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static Pchar Protocol_AppHead_GetParam(PProtocol_Layer_ST layer,Protocol_Param_EM param)
{
    if (param > PP_MAX)
    {
        return 0;
    }

    return layer->ParamList[param];
}

/********************************************************
 * function    :  Protocol_AppHead_Send
 * Description :  ��������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_AppHead_Send(PProtocol_Layer_ST layer)
{
    ///ִ�з��ʹ���
    if (layer->linksend)
    {
        ///send֮ǰ�ȼ���crc16
        layer->crc = crc_ccitt((unsigned char*)layer->data,strlen(layer->data),layer->crc);
        layer->linksend((unsigned char*)layer->data,strlen(layer->data));
    }



    if (layer->up)
    {
        layer->up->setcrc(layer->up,layer->crc);
        layer->up->send(layer->up);
    }
}

/********************************************************
 * function    :  protocol_AppHead_Section
 * Description :  ��ȡһ����Ч���ݶ�
 * param       :  data  �������ݻ���
 * param       :  datalen  Э������
 * param       :  line  ���ݶε���ʼָ��
 * return      :  ���ջ��棬ָ����һ�����ݶε�ָ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned char* protocol_AppHead_Section(unsigned char* data,unsigned short datalen,unsigned char** line)
{
    *line = data;
    while(*data != ',' && datalen > 0)
    {
        data++;
        datalen--;
    }
    if (!datalen)
    {
        return data;
    }
    return ++data;
}

struct paramtag_st
{
    char* param;
    Protocol_Param_EM tag;
};

static struct paramtag_st paramlist[3] = {
                                      {"\"create_date\":",PP_CREATEDATE},
                                      {"\"create_time\":",PP_CREATETIME},
                                      {"\"errcode\":",PP_ERRORCODE}
                                      };


/********************************************************
 * function    :  Protocol_AppHead_GetParamValue
 * Description :  ��ȡ������Ӧ���еĲ�����ֵ
 * param       :  param  ��������
 * param       :  datalen  �������泤��
 * param       :  value  ��ȡ���Ĳ�����ֵ
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_AppHead_GetParamValue(unsigned char* param,unsigned short datalen,unsigned char* value)
{



    while(*param == '"')
    {
        param++;
    }

    while(*param != '"')
    {
        *value++ = *param++;
    }

}

/********************************************************
 * function    :  Protocol_AppHead_ParamParse
 * Description :  �Խ��ջ�����в�������
 * param       :  layer  Э������
 * param       :  param  ��������
 * param       :  datalen  �������泤��
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_AppHead_ParamParse(PProtocol_Layer_ST layer,unsigned char* param,unsigned short datalen)
{
    int index;
    int size = sizeof(paramlist) / sizeof(struct paramtag_st);

    unsigned char value[64] = {0};

    for (index = 0;index<size;index++)
    {
        if (!memcmp(paramlist[index].param,param,strlen(paramlist[index].param)))
        {

            param += strlen(paramlist[index].param);
            datalen -= strlen(paramlist[index].param);
            Protocol_AppHead_GetParamValue(param,datalen,(unsigned char*)value);
            layer->protocol->setparam(layer->protocol,paramlist[index].tag,(Pchar)value);
            return;
        }
    }

}


/********************************************************
 * function    :  Protocol_AppHead_Parse
 * Description :  ������Ӧ�����ݽ���
 * param       :  layer  Э������
 * param       :  data  ������Ӧ�����ݻ���
 * param       :  datalen  ������Ӧ�����ݻ��泤��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_AppHead_Parse(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
    unsigned char* line;
    unsigned short sectionsize = 0;

    data =  protocol_AppHead_Section(data,datalen,&line);

    while(datalen)
    {
        sectionsize = data - line;
        Protocol_AppHead_ParamParse(layer,line,sectionsize);
        datalen -= sectionsize;
        data =  protocol_AppHead_Section(data,datalen,&line);
    }
}

/********************************************************
 * function    :  Protocol_AppHead_Recv
 * Description :  ���շ�����Ӧ��
 * param       :  layer  Э������
 * param       :  data  ������Ӧ�����ݻ���
 * param       :  datalen  ������Ӧ�����ݻ��泤��
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_AppHead_Recv(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
    ///��������
    unsigned char* pbuffer = data;

    if(datalen == 0)
    {
        return;
    }

    while(*pbuffer != '}')
    {
        pbuffer++;
    }
		

		
    ++data;
    Protocol_AppHead_Parse(layer,(unsigned char*)data,pbuffer-data-1);
    pbuffer++;
		
    if((pbuffer-data) > (datalen+1))  //hcp 2015-11-3  ���� �쳣��
		{
			printf("len:%d,%d", datalen,pbuffer-data);  //hcp123
			
			if (layer->data)
			{
        free(layer->data);
        layer->data =0;
			}
			return;
		}
		
    datalen -=  pbuffer-data +1;

    if (layer->data)
    {
        free(layer->data);
        layer->data =0;
    }

    if (layer->up)
    {
        layer->up->recv(layer->up,pbuffer,datalen);
    }

}

/********************************************************
 * function    :  Protocol_AppHead_GetCrc
 * Description :  ��ȡcrcУ��ֵ
 * param       :  layer  Э������
 * return      :  crcУ��ֵ
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned short Protocol_AppHead_GetCrc(PProtocol_Layer_ST layer)
{
    if (layer->up)
    {
        return layer->up->getcrc(layer->up);
    }

    return layer->crc;
}

/********************************************************
 * function    :  Protocol_AppHead_SetCrc
 * Description :  ����crcУ��ֵ
 * param       :  layer  Э������
 * param       :  crc  crcУ��ֵ
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_AppHead_SetCrc(PProtocol_Layer_ST layer,unsigned short crc)
{
    layer->crc = ~crc;
}

/********************************************************
 * function    :  Protocol_AppHead_Initialize
 * Description :  ����Ӧ��Э����ͷ����
 * param       :  ��
 * return      :  Ӧ��Э����ͷ����
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
PProtocol_Layer_ST Protocol_AppHead_Initialize(void)
{
    if (!Self)
    {
        Self = malloc(sizeof(*Self));
        memset((void*)Self,0,sizeof(*Self));

        Self->init     = Protocol_AppHead_Init;
        Self->pack     = Protocol_AppHead_Pack;
        Self->unpack   = Protocol_AppHead_Unpack;
        Self->getsize  = Protocol_AppHead_Getsize;
        Self->puttag   = Protocol_AppHead_PutTag;
        Self->clear    = Protocol_AppHead_Clear;
        Self->setparam = Protocol_AppHead_SetParam;
        Self->getparam = Protocol_AppHead_GetParam;
        Self->send     = Protocol_AppHead_Send;
        Self->recv     = Protocol_AppHead_Recv;
        Self->getcrc   = Protocol_AppHead_GetCrc;
        Self->setcrc   = Protocol_AppHead_SetCrc;

        Self->ParamList = malloc((int)PP_MAX * sizeof(Pchar));
        memset(Self->ParamList,0,(int)PP_MAX * sizeof(Pchar));

    }
    return Self;
}



