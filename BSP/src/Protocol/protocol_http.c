
/********************************************************
 * File	       :  protocol_http.h
 * Description :  http协议层
 * Author      :  左俊
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
 * Update      :  2015-06-26
 * History     :

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "protocol_typedef.h"





#define HTTP_HEAD_MAX (1024)

static PProtocol_Layer_ST Self = 0;

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


static unsigned int Protoccol_Http_IsDigit(unsigned char value)
{
    unsigned int ret = 0;

    if ((value >= 0x30) && (value <= 0x39))
    {
        ret = 1;
    }

    return ret;
}


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
 * Description :  应用协议包体协议层初始化函数
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
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
 * Description :  封包函数
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Http_Pack(PProtocol_Layer_ST layer)
{
    ///将需要发送的数据组织好
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
        ///采用POST协议
        if (layer->up)
        {
            bodysize = layer->up->getsize(layer->up);
        }
        ///填充报文内容
        sprintf(layer->data,HTTP_POST_HEAD,layer->ParamList[PP_HOST],layer->ParamList[PP_PORT],bodysize);
    }
    else
    {
        ///采用GET协议
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
 * Description :  解包函数
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
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
 * Description :  获取待发送数据长度
 * param       :  layer  协议层对象
 * return      :  待发送数据长度，如果没有数据需要发送，则为0
 * Author      :  左俊
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
 * Description :  清除协议层数据
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Http_Clear(PProtocol_Layer_ST layer)
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
 * function    :  Protocol_Http_SetParam
 * Description :  设置参数
 * param       :  layer  协议层对象
 * param       :  param  参数标签
 * param       :  value  参数值
 * return      :  无
 * Author      :  左俊
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
 * Description :  读取参数
 * param       :  layer  协议层对象
 * param       :  param  参数标签
 * return      :  参数的数据缓存指针
 * Author      :  左俊
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
 * Description :  数据发送
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Http_Send(PProtocol_Layer_ST layer)
{
    ///执行发送处理
    if (layer->linksend)
    {
        ///send之前先计算crc16
        layer->crc = crc_ccitt((unsigned char*)layer->data,strlen(layer->data),layer->crc);

        layer->linksend((unsigned char*)layer->data,strlen(layer->data));
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
 * Description :  从接收缓存中读取一行http数据
 * param       :  data  接收数据缓存
 * param       :  datalen  接收缓存数据大小
 * param       :  line  获取的一个新行的起始指针
 * return      :  接收缓存指向下一行的指针
 * Author      :  左俊
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
   
    if (!memcmp((void*)line,(void*)CONTENT_RANGE,strlen(CONTENT_RANGE)))
    {
        line += strlen(CONTENT_RANGE);
        
        while(!Protoccol_Http_IsDigit(*line))
        {
            line++;
           
        }
        ///先解析出起始偏移
        while(Protoccol_Http_IsDigit(*line))
        {
             *prangestart++ = *line++;
        }
        Self->protocol->setparam(Self->protocol,PP_RANGSTART,(Pchar)rangestart);
        while(!Protoccol_Http_IsDigit(*line))
        {
            line++;
        }
        ///再解析出结束偏移
        while(Protoccol_Http_IsDigit(*line))
        {
             *prangeend++ = *line++;
        }
        Self->protocol->setparam(Self->protocol,PP_RANGENDEND,(Pchar)rangeend);
        while(!Protoccol_Http_IsDigit(*line))
        {
            line++;
        }
        ///再解析出文件大小
        while(Protoccol_Http_IsDigit(*line))
        {
             *pfilesize++ = *line++;
        }
        Self->protocol->setparam(Self->protocol,PP_FILESIZE,(Pchar)filesize);
        
    }

    return ;
}

static unsigned short Protocol_Http_GetHttp_Response(unsigned char* line)
{
    unsigned char tmpbuffer[6] = {0};
    unsigned char* pbuffer = tmpbuffer;

    if (memcmp((void*)line,(void*)"HTTP",strlen("HTTP")))
    {
        return 0;
    }
    line +=  4;
    while(*line != ' ')
    {
        if (*line == 0x0A)
        {
            return 0;
        }
        line++;
    }

    while(!Protoccol_Http_IsDigit(*line))
    {
        line++;
    }

    while(Protoccol_Http_IsDigit(*line))
    {
         *pbuffer++ = *line++;
    }


    return (unsigned short)atoi((Pchar)tmpbuffer);

}


static unsigned short Protocol_Http_GetContent_Length(unsigned char* line)
{
    unsigned char tmpbuffer[6] = {0};
    unsigned char* pbuffer = tmpbuffer;
    if (!memcmp((void*)line,(void*)"Content-Length:",strlen("Content-Length:")))
    {
        line += strlen("Content-Length:");
        while(!Protoccol_Http_IsDigit(*line))
        {
            line++;
        }

        while(Protoccol_Http_IsDigit(*line))
        {
             *pbuffer++ = *line++;
        }


        return (unsigned short)atoi((Pchar)tmpbuffer);
    }

    return 0;
}


/********************************************************
 * function    :  Protocol_Http_HexToByte
 * Description :  将16进制表达转换为字节
 * param       :  hex  16进制表示字符串
 * return      :  转换后的字节
 * Author      :  左俊
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
 * Description :  将16进制转换为短整型
 * param       :  hex  16进制表示字符串
 * return      :  转换后的短整型
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static unsigned short Protocol_Http_HexToShort(unsigned char* hex)
{
    unsigned short ret = 0;
    unsigned char count = strlen((Pchar)hex) / 2;
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
 * Description :  获取服务器应答数据的应用层协议大小
 * param       :  line  包含应用数据大小的协议行
 * return      :  应用层数据大小
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static unsigned short Protocol_Http_GetPackSize(unsigned char* line)
{
    ///从头读取到尾，
    unsigned char tmpbuffer[10] = {0};
    unsigned char* pbuffer = tmpbuffer;

    while (*line != '\r' && *line != '\n' )
    {
        *pbuffer++ = *line++;
    }
    ///由于长度是按照16进制表示，如果长度的表示是奇数，则需要后移一位
    if (strlen((Pchar)tmpbuffer) % 2)
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
 * Description :  接收服务器应答
 * param       :  layer  协议层对象
 * param       :  data  服务器应答数据缓存
 * param       :  datalen  服务器应答数据大小
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :  2015-07-06 增加对HTTP头进行判断

*/
static void Protocol_Http_Recv(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
    ///将缓存中的内容，读取出来
    unsigned char* line = 0;
    unsigned short packsize = 0;
    unsigned short __packsize = 0;
    unsigned short __resp = 0;
    unsigned short resp = 0;

    if (!data)
    {
        return;
    }
    
    data = protocol_Http_Readline(data,datalen,&line);
    datalen -= (unsigned short)(data-line);
  
    while((*line != '\r') && (*line != '\n') && (datalen > 0))
    {
        ////对应答的内容进行分解处理，主要是判断HTTP 200 返回，和获取数据长度
        __resp = Protocol_Http_GetHttp_Response(line);
        if (__resp)
        {
            resp = __resp;

        }
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
   
    ///按照最新的协议处理，如果没有出现过content-length,那么这个部分会出现报文大小
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

    if (resp != 200 && resp != 206)
    {
        if (layer->linkfree)
        {
            layer->linkfree();
        }
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
 * Description :  获取crc校验值
 * param       :  layer  协议层对象
 * return      :  crc校验值
 * Author      :  左俊
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
 * Description :  设置crc校验值
 * param       :  layer  协议层对象
 * param       :  crc  crc校验值
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Http_SetCrc(PProtocol_Layer_ST layer,unsigned short crc)
{
    layer->crc = ~crc;
}

/********************************************************
 * function    :  Protocol_Http_Initialize
 * Description :  创建http协议层对象
 * param       :  无
 * return      :  http协议层对象
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
PProtocol_Layer_ST Protocol_Http_Initialize(void)
{
    if (!Self)
    {
        Self = malloc(sizeof(*Self));
        memset(Self,0,sizeof(*Self));

        Self->init     = Protocol_Http_Init;
        Self->pack     = Protocol_Http_Pack;
        Self->unpack   = Protocol_Http_Unpack;
        Self->getsize  = Protocol_Http_Getsize;
        Self->puttag   = Protocol_Http_PutTag;
        Self->clear    = Protocol_Http_Clear;
        Self->setparam = Protocol_Http_SetParam;
        Self->getparam = Protocol_Http_GetParam;
        Self->send     = Protocol_Http_Send;
        Self->recv     = Protocol_Http_Recv;
        Self->getcrc   = Protocol_Http_GetCrc;
        Self->setcrc   = Protocol_Http_SetCrc;

        Self->ParamList = malloc((int)PP_MAX * sizeof(Pchar));
        memset(Self->ParamList,0,(int)PP_MAX * sizeof(Pchar));

    }
    return Self;
}






