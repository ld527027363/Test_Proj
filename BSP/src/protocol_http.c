
/********************************************************
 * File	       :  protocol_http.h
 * Description :  httpЭ���
 * Author      :  ��
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾
 * Update      :  2015-06-26
 * History     :

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "protocol_typedef.h"



static Pchar ParamNameList[PP_MAX] = {
    "account=",
    "magic=",
    "type=",
    "session_id=",
    "create_date=",
    "create_time=",
    "sequence_id=",
    "webtype=",
    "action=",
    0,
    0,
    0,
    0,
};

#define HTTP_HEAD_MAX (1024)

static PProtocol_Layer_ST this = 0;

const char* HTTP_POST_HEAD = "POST /sgi_bin/device_entry/entry.aspx?pversion=11 HTTP/1.1\r\n" \
                             "Host: %s:%s\r\n" \
                             "Content-Length: %d\r\n" \
                             "Cache-Control: max-age=0\r\n" \
                             "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n" \
                             "Origin: null\r\n" \
                             "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36\r\n" \
                             "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryXRyP7i1gwAAdCt5v\r\n" \
                             "Accept-Encoding: deflate,sdch\r\n" \
                             "Accept-Language: zh-CN,zh;q=0.8\r\n\r\n";

const char* HTTP_GET_HEAD = "GET %s HTTP/1.1\r\n" \
                            "Accept: */*\r\n" \
                            "Cache-Control: no-cache\r\n" \
                            "Connection: Keep-Alive\r\n" \
                            "Host: %s:%s\r\n" \
                            "User-Agent: Mozilla/4.0\r\n" \
                            "Range:bytes=%s-%s\r\n\r\n";

extern unsigned short crc_ccitt(unsigned char *q, int len,unsigned short crc);


static void Protocol_Http_NotifyUpgrade(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
    int __start = 0;
    int __end = 0;
    int __filesize = 0;

    __start = atoi(layer->protocol->getparam(layer->protocol,PP_RANGSTART));
    __end =   atoi(layer->protocol->getparam(layer->protocol,PP_RANGENDEND));
    __filesize =   atoi(layer->protocol->getparam(layer->protocol,PP_FILESIZE));

    layer->upgrade(__start,__end,__filesize,data,datalen);

    layer->protocol->setparam(layer->protocol,PP_ERRORCODE,"00");
}

/********************************************************
 * function    :  Protocol_Http_Init
 * Description :  Ӧ��Э�����Э����ʼ������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Http_Init(PProtocol_Layer_ST layer)
{
    if (!layer->down)
    {
        return;
    }

    layer->down->init(layer->down);
}

/********************************************************
 * function    :  Protocol_Http_Pack
 * Description :  �������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Http_Pack(PProtocol_Layer_ST layer)
{
    ///����Ҫ���͵�������֯��
    unsigned bodysize = 0;

    assert_null(layer->ParamList[PP_HOST]);
    assert_null(layer->ParamList[PP_PORT]);

    if (layer->data)
    {
        free(layer->data);
        layer->data =0;
    }

    layer->data = (Pchar)malloc(HTTP_HEAD_MAX);
    memset(layer->data,0,HTTP_HEAD_MAX);


    if (!strcmp(layer->ParamList[PP_HTTPTYPE],"POST"))
    {
        ///����POSTЭ��
        if (layer->up)
        {
            bodysize = layer->up->getsize(layer->up);
        }
        ///��䱨������
        sprintf(layer->data,HTTP_POST_HEAD,layer->ParamList[PP_HOST],layer->ParamList[PP_PORT],bodysize);
    }
    else
    {
        ///����GETЭ��
        sprintf(layer->data,HTTP_GET_HEAD, layer->ParamList[PP_URL],
                                           layer->ParamList[PP_HOST],
                                           layer->ParamList[PP_PORT],
                                           layer->ParamList[PP_RANGSTART],
                                           layer->ParamList[PP_RANGENDEND]
                                           );

    }

    if (!layer->down)
    {
        layer->send(layer);
        return;
    }

    layer->down->pack(layer->down);


}

/********************************************************
 * function    :  Protocol_Http_Unpack
 * Description :  �������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Http_Unpack(PProtocol_Layer_ST layer)
{
    unsigned char* pbuffer;
    unsigned short datalen;
    if (!layer->down)
    {
        pbuffer = layer->linkrecv(&datalen);
        if (!pbuffer)
        {
            return;
        }
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
 * function    :  Protocol_Http_Getsize
 * Description :  ��ȡ���������ݳ���
 * param       :  layer  Э������
 * return      :  ���������ݳ��ȣ����û��������Ҫ���ͣ���Ϊ0
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned int Protocol_Http_Getsize(PProtocol_Layer_ST layer)
{
    unsigned int ret = 0;

    if (layer->data)
    {
        ret = strlen(layer->data);
    }

    if (layer->up)
    {
        ret += layer->up->getsize(layer->up);
    }
    return   ret;
}

/********************************************************
 * function    :  Protocol_Http_PutTag
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
static int Protocol_Http_PutTag(PProtocol_Layer_ST layer,Protocol_Tag_EM tag,unsigned char* data,unsigned short datalen)
{
    if (!layer->down)
    {
        return -1;
    }
    return layer->down->puttag(layer->down,tag,data,datalen);
}

/********************************************************
 * function    :  Protocol_Http_Clear
 * Description :  ���Э�������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Http_Clear(PProtocol_Layer_ST layer)
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
 * function    :  Protocol_Http_SetParam
 * Description :  ���ò���
 * param       :  layer  Э������
 * param       :  param  ������ǩ
 * param       :  value  ����ֵ
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Http_SetParam(PProtocol_Layer_ST layer,Protocol_Param_EM param,Pchar value)
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
 * function    :  Protocol_Http_GetParam
 * Description :  ��ȡ����
 * param       :  layer  Э������
 * param       :  param  ������ǩ
 * return      :  ���������ݻ���ָ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static Pchar Protocol_Http_GetParam(PProtocol_Layer_ST layer,Protocol_Param_EM param)
{
    if (param > PP_MAX)
    {
        return 0;
    }

    return layer->ParamList[param];
}

/********************************************************
 * function    :  Protocol_Http_Send
 * Description :  ���ݷ���
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Http_Send(PProtocol_Layer_ST layer)
{
    ///ִ�з��ʹ���
    if (layer->linksend)
    {
        ///send֮ǰ�ȼ���crc16
        layer->crc = crc_ccitt(layer->data,strlen(layer->data),layer->crc);

        layer->linksend(layer->data,strlen(layer->data));
    }
    if (!strcmp(layer->ParamList[PP_HTTPTYPE],"POST"))
    {
        if (layer->up)
        {
            layer->up->setcrc(layer->up,layer->crc);
            layer->up->send(layer->up);
        }
    }
}

/********************************************************
 * function    :  protocol_Http_Readline
 * Description :  �ӽ��ջ����ж�ȡһ��http����
 * param       :  data  �������ݻ���
 * param       :  datalen  ���ջ������ݴ�С
 * param       :  line  ��ȡ��һ�����е���ʼָ��
 * return      :  ���ջ���ָ����һ�е�ָ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned char* protocol_Http_Readline(unsigned char* data,unsigned short datalen,unsigned char** line)
{
    if (!data)
    {
        return 0;
    }
    *line = data;
    while(datalen)
    {

        if (*data == '\n')
        {
            return ++data;
        }
        data++;
        datalen--;
    }

    return data;
}
#define CONTENT_RANGE "Content-Range:"
static void Protocol_Http_GetContent_Range(unsigned char* line)
{
    unsigned char rangestart[8] = {0};
    unsigned char rangeend[8] = {0};
    unsigned char filesize[8] = {0};
    unsigned char* prangestart = rangestart;
    unsigned char* prangeend   = rangeend;
    unsigned char* pfilesize   = filesize;
    if (!memcmp(line,CONTENT_RANGE,strlen(CONTENT_RANGE)))
    {
        line += strlen(CONTENT_RANGE);
        while(!isdigit(*line))
        {
            line++;
        }
        ///�Ƚ�������ʼƫ��
        while(isdigit(*line))
        {
             *prangestart++ = *line++;
        }
        this->protocol->setparam(this->protocol,PP_RANGSTART,rangestart);
        while(!isdigit(*line))
        {
            line++;
        }
        ///�ٽ���������ƫ��
        while(isdigit(*line))
        {
             *prangeend++ = *line++;
        }
        this->protocol->setparam(this->protocol,PP_RANGENDEND,rangeend);
        while(!isdigit(*line))
        {
            line++;
        }
        ///�ٽ������ļ���С
        while(isdigit(*line))
        {
             *pfilesize++ = *line++;
        }
        this->protocol->setparam(this->protocol,PP_FILESIZE,filesize);
        
    }

    return ;
}

static unsigned short Protocol_Http_GetContent_Length(unsigned char* line)
{
    unsigned char tmpbuffer[6] = {0};
    unsigned char* pbuffer = tmpbuffer;
    if (!memcmp(line,"Content-Length:",strlen("Content-Length:")))
    {
        line += strlen("Content-Length:");
        while(!isdigit(*line))
        {
            line++;
        }

        while(isdigit(*line))
        {
             *pbuffer++ = *line++;
        }


        return (unsigned short)atoi(tmpbuffer);
    }

    return 0;
}


/********************************************************
 * function    :  Protocol_Http_HexToByte
 * Description :  ��16���Ʊ��ת��Ϊ�ֽ�
 * param       :  hex  16���Ʊ�ʾ�ַ���
 * return      :  ת������ֽ�
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned char  Protocol_Http_HexToByte(unsigned char* hex)
{
    unsigned char ret =0;
    unsigned char count = 2;
    while(count--)
    {
        ret <<= 4;
        if (*hex < 0x41)
        {
            ret |= (*hex - 0x30);
        }
        else
        {
            ret |= (*hex - 0x37);
        }
        hex++;
    }
    return ret;
}

/********************************************************
 * function    :  Protocol_Http_HexToShort
 * Description :  ��16����ת��Ϊ������
 * param       :  hex  16���Ʊ�ʾ�ַ���
 * return      :  ת����Ķ�����
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned short Protocol_Http_HexToShort(unsigned char* hex)
{
    unsigned short ret = 0;
    unsigned char count = strlen(hex) / 2;
    while(count--)
    {
        ret <<= 8;
        ret |= Protocol_Http_HexToByte(hex);
        hex += 2;
    }

    return ret;
}

/********************************************************
 * function    :  Protocol_Http_GetPackSize
 * Description :  ��ȡ������Ӧ�����ݵ�Ӧ�ò�Э���С
 * param       :  line  ����Ӧ�����ݴ�С��Э����
 * return      :  Ӧ�ò����ݴ�С
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned short Protocol_Http_GetPackSize(unsigned char* line)
{
    ///��ͷ��ȡ��β��
    unsigned char tmpbuffer[10] = {0};
    unsigned char* pbuffer = tmpbuffer;

    while (*line != '\r' && *line != '\n' )
    {
        *pbuffer++ = *line++;
    }
    ///���ڳ����ǰ���16���Ʊ�ʾ��������ȵı�ʾ������������Ҫ����һλ
    if (strlen(tmpbuffer) % 2)
    {
        while(tmpbuffer != pbuffer)
        {
            *pbuffer = *(pbuffer -1);
            pbuffer--;
        }

        *pbuffer = '0';
    }
    pbuffer = tmpbuffer;

    while(*pbuffer)
    {
        *pbuffer = toupper(*pbuffer);
        pbuffer++;
    }

    pbuffer = tmpbuffer;
    return Protocol_Http_HexToShort(pbuffer);
}


/********************************************************
 * function    :  Protocol_Http_Recv
 * Description :  ���շ�����Ӧ��
 * param       :  layer  Э������
 * param       :  data  ������Ӧ�����ݻ���
 * param       :  datalen  ������Ӧ�����ݴ�С
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Http_Recv(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
    ///�������е����ݣ���ȡ����
    unsigned char* line = 0;
    unsigned short packsize = 0;
    unsigned short __packsize = 0;

    if (!data)
    {
        return;
    }
    data = protocol_Http_Readline(data,datalen,&line);
    datalen -= (unsigned short)(data-line);

    while(*line != '\r' && *line != '\n' && datalen > 0)
    {
        ////��Ӧ������ݽ��зֽ⴦����Ҫ���ж�HTTP 200 ���أ��ͻ�ȡ���ݳ���

        __packsize = Protocol_Http_GetContent_Length(line);
        if (__packsize > 0)
        {
            packsize = __packsize;

        }
        else
        {
            Protocol_Http_GetContent_Range(line);
        }

        data = protocol_Http_Readline(data,datalen,&line);
        datalen -= (unsigned short)(data-line);
    }

    ///�������µ�Э�鴦�����û�г��ֹ�content-length,��ô������ֻ���ֱ��Ĵ�С
    if(packsize == 0)
    {
        data = protocol_Http_Readline(data,datalen,&line);
        datalen -= (unsigned short)(data-line);

        if (datalen == 0)
        {
            return;
        }
        __packsize = Protocol_Http_GetPackSize(line);
        if (__packsize > 0)
        {
            packsize = __packsize;

        }
    }
    


    if (packsize == 0)
    {
        return;
    }

    if (packsize > datalen)
    {
        return;
    }

    if (layer->data)
    {
        free(layer->data);
        layer->data =0;
    }

    if (layer->upgrade)
    {
        Protocol_Http_NotifyUpgrade(layer,data,datalen);

        if (layer->linkfree)
        {
            layer->linkfree();
        }
        return;
    }

    if (layer->up)
    {
        layer->up->recv(layer->up,data,datalen);
    }
}

/********************************************************
 * function    :  Protocol_Http_GetCrc
 * Description :  ��ȡcrcУ��ֵ
 * param       :  layer  Э������
 * return      :  crcУ��ֵ
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned short Protocol_Http_GetCrc(PProtocol_Layer_ST layer)
{
    if (!strcmp(layer->ParamList[PP_HTTPTYPE],"POST"))
    {
        if (layer->up)
        {
            return layer->up->getcrc(layer->up);
        }
    }

    return layer->crc;
}

/********************************************************
 * function    :  Protocol_Http_SetCrc
 * Description :  ����crcУ��ֵ
 * param       :  layer  Э������
 * param       :  crc  crcУ��ֵ
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Http_SetCrc(PProtocol_Layer_ST layer,unsigned short crc)
{
    layer->crc = ~crc;
}

/********************************************************
 * function    :  Protocol_Http_Initialize
 * Description :  ����httpЭ������
 * param       :  ��
 * return      :  httpЭ������
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
PProtocol_Layer_ST Protocol_Http_Initialize(void)
{
    if (!this)
    {
        this = malloc(sizeof(*this));
        memset(this,0,sizeof(*this));

        this->init     = Protocol_Http_Init;
        this->pack     = Protocol_Http_Pack;
        this->unpack   = Protocol_Http_Unpack;
        this->getsize  = Protocol_Http_Getsize;
        this->puttag   = Protocol_Http_PutTag;
        this->clear    = Protocol_Http_Clear;
        this->setparam = Protocol_Http_SetParam;
        this->getparam = Protocol_Http_GetParam;
        this->send     = Protocol_Http_Send;
        this->recv     = Protocol_Http_Recv;
        this->getcrc   = Protocol_Http_GetCrc;
        this->setcrc   = Protocol_Http_SetCrc;

        this->ParamList = malloc(PP_MAX * sizeof(Pchar));
        memset(this->ParamList,0,PP_MAX * sizeof(Pchar));

    }
    return this;
}




