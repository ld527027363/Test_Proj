/********************************************************
 * File	       :  protocol_lan.h
 * Description :  以太网协议对象
 * Author      :  左俊
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
 * Update      :  2015-06-26
 * History     :

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol_typedef.h"
#include "protocol.h"

static PProtocol_ST Self = 0;
#define TAG_WAIT "55"


static int Protocol_Lan_CalcUpgradeRange(PProtocol_ST protocol)
{
    int __start = 0;
    int __end = 0;
    int __filesize = 0;
    char rangestart[8] = {0};
    char rangeend[8]   = {0};
   // char filesize[8]   = {0};


    if (!protocol->getparam(protocol,PP_RANGSTART))
    {
        protocol->setparam(protocol,PP_RANGSTART,"0");
    }

    if (!protocol->getparam(protocol,PP_RANGENDEND))
    {
        protocol->setparam(protocol,PP_RANGENDEND,"0");
    }

    if (!protocol->getparam(protocol,PP_FILESIZE))
    {
        protocol->setparam(protocol,PP_FILESIZE,"0");
    }

    __start = atoi(protocol->getparam(protocol,PP_RANGSTART));
    __end = atoi(protocol->getparam(protocol,PP_RANGENDEND));
    __filesize = atoi(protocol->getparam(protocol,PP_FILESIZE));

    if (__filesize && __end == __filesize -1)
    {
        return -1;
    }
    if (__end)
    {
        __end++;
    }
    __start = __end;
    __end =  __start + UPGRADE_REQUEST_SIZE - 1;

    if (__filesize && (__end > __filesize))
    {
        __end = __filesize;
    }

    sprintf(rangestart,"%d",__start);
    sprintf(rangeend,"%d",__end);
    protocol->setparam(protocol,PP_RANGSTART,rangestart);
    protocol->setparam(protocol,PP_RANGENDEND,rangeend);

    return 0;
}

static void Protocol_Lan_SetUpgradeCallback(PProtocol_ST protocol,__upgrade upgrade)
{
    PProtocol_Layer_ST layer = 0;
    if (protocol->layers[0]->upgrade)
    {
        return;
    }

    layer =  protocol->layers[0];

    while(layer)
    {
        layer->upgrade = upgrade;
        layer = layer->down;
    }
}

static void Protocol_Lan_ClearUpgradeCallback(PProtocol_ST protocol)
{
    PProtocol_Layer_ST layer = 0;
    if (!protocol->layers[0]->upgrade)
    {
        return;
    }

    layer =  protocol->layers[0];

    while(layer)
    {
        layer->upgrade = 0;
        layer = layer->down;
    }
    
}

/********************************************************
 * function    :  Protocol_Lan_Login
 * Description :  登录操作
 * param       :  protocol  协议栈对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static int Protocol_Lan_Login(PProtocol_ST protocol)
{
    int index = (int)PP_MAX;
    if (!protocol)
    {
        return -1;
    }

    ///1、对协议层先进行初始化

    protocol->sequence_inc(protocol);
    protocol->setparam(protocol,PP_TYPE,"2");
    protocol->setparam(protocol,PP_ERRORCODE,TAG_WAIT);
    protocol->setparam(protocol,PP_ACTION,"action_login");
    protocol->setparam(protocol,PP_HTTPTYPE,"POST");
    Protocol_Lan_ClearUpgradeCallback(protocol);
    protocol->layers[0]->clear(protocol->layers[0]);
    ///2、将参数下发到协议各层
    while(index)
    {
        protocol->layers[0]->setparam(protocol->layers[0],(Protocol_Param_EM)(index-1),protocol->ParamList[index-1]);
        index--;
    }

    ///3、将参数中需要数据打包的内容，放入数据包内
    protocol->layers[0]->puttag(protocol->layers[0],PT_ACCOUNT,(unsigned char*)protocol->ParamList[PP_ACCOUNT],strlen(protocol->ParamList[PP_ACCOUNT]));
    protocol->layers[0]->puttag(protocol->layers[0],PT_PASSWORD,(unsigned char*)protocol->ParamList[PP_PASSWWORD],strlen(protocol->ParamList[PP_PASSWWORD]));
    ///4、调用封包函数，进行封包
    protocol->layers[0]->pack(protocol->layers[0]);

    return 0;
}

/********************************************************
 * function    :  Protocol_Lan_SyncBatch
 * Description :  批上送操作
 * param       :  protocol  协议栈对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static int Protocol_Lan_SyncBatch(PProtocol_ST protocol)
{
    int index = (int)PP_MAX;
    if (!protocol)
    {
        return -1;
    }

    ///1、对协议层先进行初始化

    protocol->sequence_inc(protocol);
    protocol->setparam(protocol,PP_TYPE,"3");
    protocol->setparam(protocol,PP_ERRORCODE,TAG_WAIT);
    protocol->setparam(protocol,PP_HTTPTYPE,"POST");
    protocol->setparam(protocol,PP_ACTION,"action_syncbatch1_1");
    Protocol_Lan_ClearUpgradeCallback(protocol);
  //  protocol->layers[0]->clear(protocol->layers[0]);
    ///2、将参数下发到协议各层
    while(index)
    {
        protocol->layers[0]->setparam(protocol->layers[0],(Protocol_Param_EM)(index-1),protocol->ParamList[index-1]);
        index--;
    }
    

    ///3、调用封包函数，进行封包
    protocol->layers[0]->pack(protocol->layers[0]);

    return 0;
}

/********************************************************
 * function    :  Protocol_Lan_HeartPack
 * Description :  心跳包操作
 * param       :  protocol  协议栈对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static int   Protocol_Lan_HeartPack(PProtocol_ST protocol)
{
    int index = (int)PP_MAX;
    if (!protocol)
    {
        return -1;
    }

    ///1、对协议层先进行初始化

    protocol->sequence_inc(protocol);
    protocol->setparam(protocol,PP_TYPE,"3");
    protocol->setparam(protocol,PP_ERRORCODE,TAG_WAIT);
    protocol->setparam(protocol,PP_HTTPTYPE,"POST");
    protocol->setparam(protocol,PP_ACTION,"action_heartbeatpack");
    Protocol_Lan_ClearUpgradeCallback(protocol);
    protocol->layers[0]->clear(protocol->layers[0]);
    ///2、将参数下发到协议各层
    while(index)
    {
        protocol->layers[0]->setparam(protocol->layers[0],(Protocol_Param_EM)(index-1),protocol->ParamList[index-1]);
        index--;
    }
    
    ///3、将参数中需要数据打包的内容，放入数据包内
    protocol->layers[0]->puttag(protocol->layers[0],PT_DATE,(unsigned char*)protocol->ParamList[PP_CREATEDATE],strlen(protocol->ParamList[PP_CREATEDATE]));
    protocol->layers[0]->puttag(protocol->layers[0],PT_TIME,(unsigned char*)protocol->ParamList[PP_CREATETIME],strlen(protocol->ParamList[PP_CREATETIME]));
    protocol->layers[0]->puttag(protocol->layers[0],PT_MODEL,(unsigned char*)protocol->ParamList[PP_MODEL],strlen(protocol->ParamList[PP_MODEL]));
    protocol->layers[0]->puttag(protocol->layers[0],PT_SOFTVERSION,(unsigned char*)protocol->ParamList[PP_SOFTVERSION],strlen(protocol->ParamList[PP_SOFTVERSION]));
    protocol->layers[0]->puttag(protocol->layers[0],PT_APPARAMVER,(unsigned char*)protocol->ParamList[PP_APPARAMVER],strlen(protocol->ParamList[PP_APPARAMVER]));
    protocol->layers[0]->puttag(protocol->layers[0],PT_APWARNING,(unsigned char*)protocol->ParamList[PP_APWARNING],strlen(protocol->ParamList[PP_APWARNING])); 			//20160831 LD 增加警告信息
    protocol->layers[0]->puttag(protocol->layers[0],PT_HEARTBEAT,(unsigned char*)"lovicoco",strlen("lovicoco"));

    ///4、调用封包函数，进行封包
    protocol->layers[0]->pack(protocol->layers[0]);

    return 0;
}

/********************************************************
 * function    :  Protocol_Lan_GetResponse
 * Description :  获取服务器应答
 * param       :  protocol  协议栈对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static Protocol_RspStatus_EM   Protocol_Lan_GetResponse(PProtocol_ST protocol)
{
    char* value = 0;
    ///1、调用解包函数进行解包
    protocol->layers[0]->unpack(protocol->layers[0]);

    value = protocol->getparam(protocol,PP_ERRORCODE);

    if (!strcmp(value,TAG_WAIT))
    {
        return PR_WAIT;
    }

    return PR_SUCCESS;

}

/********************************************************
 * function    :  Protocol_Lan_Resend
 * Description :  重发操作
 * param       :  protocol  协议栈对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Lan_Resend(PProtocol_ST protocol)
{
    if (!protocol->getparam(protocol,PP_HTTPTYPE))
    {
        return;
    }

    if (!strcmp(protocol->getparam(protocol,PP_HTTPTYPE),"POST"))
    {

        protocol->layers[0]->pack(protocol->layers[0]);
    }
    else
    {
        protocol->layers[3]->pack(protocol->layers[0]);
    }
}

/********************************************************
 * function    :  Protocol_Lan_Sequence_Inc
 * Description :  增加流水号
 * param       :  protocol  协议栈对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void   Protocol_Lan_Sequence_Inc(PProtocol_ST protocol)
{
    int __sequenceid = 0;
    char buffer[16] = {0};
    if (protocol->ParamList[PP_SEQUENCEID] == 0)
    {
        protocol->setparam(protocol,PP_SEQUENCEID,"1");
        return;
    }
    __sequenceid = atoi(protocol->ParamList[PP_SEQUENCEID]);

    __sequenceid++;
    sprintf(buffer,"%d",__sequenceid);
    protocol->setparam(protocol,PP_SEQUENCEID,buffer);
}

/********************************************************
 * function    :  Protocol_Lan_GetTag
 * Description :  读取应用数据
 * param       :  protocol  协议栈对象
 * param       :  tag       应用数据标签
 * param       :  buffer    应用数据读取缓存
 * return      :  返回应用数据长度
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static int Protocol_Lan_GetTag(PProtocol_ST protocol,Protocol_Tag_EM tag,void** buffer)
{
    return protocol->layers[0]->gettag(protocol->layers[0],tag,buffer);
}

/********************************************************
 * function    :  Protocol_Lan_Upgrade
 * Description :  应用升级
 * param       :  protocol  协议栈对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static int   Protocol_Lan_Upgrade(PProtocol_ST protocol,__upgrade upgrade)
{
    int index = (int)PP_MAX;

    Protocol_Lan_SetUpgradeCallback(protocol,upgrade);
    protocol->setparam(protocol,PP_HTTPTYPE,"GET");
    protocol->setparam(protocol,PP_ERRORCODE,TAG_WAIT);
    protocol->layers[0]->clear(protocol->layers[0]);
    ///计算请求的起始地址和结束地址
    if (Protocol_Lan_CalcUpgradeRange(protocol) < 0)
    {
        return -1;
    }
    ///2、将参数下发到协议各层
    while(index)
    {
        protocol->layers[0]->setparam(protocol->layers[0],(Protocol_Param_EM)(index-1),protocol->ParamList[index-1]);
        index--;
    }
    
    protocol->layers[3]->pack(protocol->layers[3]);

    return 0;
}

/********************************************************
 * function    :  Protocol_Lan_PutTag
 * Description :  写入应用数据
 * param       :  protocol  协议栈对象
 * param       :  tag       应用数据标签
 * param       :  data      数据缓存
 * param       :  datalen   数据长度
 * return      :  0 表示增加成功 小于0 表示增加失败
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static int Protocol_Lan_PutTag(PProtocol_ST protocol,Protocol_Tag_EM tag,unsigned char* data,unsigned short datalen)
{

    if (!protocol->layers)
    {
        return -1;
    }
    return protocol->layers[0]->puttag(protocol->layers[0],tag,data,datalen);

}

/********************************************************
 * function    :  Protocol_Lan_Clear
 * Description :  清除应用数据
 * param       :  protocol  协议栈对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Lan_Clear(PProtocol_ST protocol)
{

    if (!protocol->layers)
    {
        return;
    }
    protocol->layers[0]->clear(protocol->layers[0]);

}

/********************************************************
 * function    :  Protocol_Lan_SetParam
 * Description :  设置参数
 * param       :  protocol  协议栈对象
 * param       :  param     参数缓存
 * param       :  value     参数值
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Lan_SetParam(PProtocol_ST protocol,Protocol_Param_EM param,char* value)
{

    int __valuelen =0;
    Pchar __value = 0;
    if (param >= PP_MAX || value == 0)
    {
        return;
    }

    __valuelen = strlen(value);

    if (__valuelen > 100)
    {
        return;
    }

    if (protocol->ParamList[param])
    {
        __value = protocol->ParamList[param];
        protocol->ParamList[param] = 0;
        free(__value);

    }

    __value = malloc(__valuelen+1);
    if (!__value)
    {
        return;	
    }
    memset(__value,0,__valuelen+1);
    memcpy(__value,value,__valuelen);

    protocol->ParamList[param] = __value;

}

/********************************************************
 * function    :  Protocol_Lan_GetParam
 * Description :  应用协议包头协议层初始化函数
 * param       :  protocol  协议栈对象
 * param       :  param     参数标签
 * return      :  参数值指针
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
static char* Protocol_Lan_GetParam(PProtocol_ST protocol,Protocol_Param_EM param)
{

    if (param >= PP_MAX )
    {
        return 0;
    }

    return protocol->ParamList[param];
}

/********************************************************
 * function    :  Protocol_Lan_Initialize
 * Description :  构建以太网协议栈
 * param       :  send  发送回调函数
 * param       :  recv  接收回调函数
 * param       :  free  释放接收缓存回调函数
 * return      :  协议栈对象
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/

PProtocol_ST Protocol_Lan_Initialize(__linksend sendcallback,__linkrecv recvcallback,__linkfree freecallback)
{
    if (!Self)
    {
        Self = (PProtocol_ST)malloc(sizeof(Protocol_ST));
        memset(Self,0,sizeof(Protocol_ST));

        Self->login        = Protocol_Lan_Login;
        Self->syncbatch    = Protocol_Lan_SyncBatch;
        Self->heartpack    = Protocol_Lan_HeartPack;
        Self->upgrade      = Protocol_Lan_Upgrade;
        Self->puttag       = Protocol_Lan_PutTag;
        Self->gettag       = Protocol_Lan_GetTag;
        Self->setparam     = Protocol_Lan_SetParam;
        Self->getparam     = Protocol_Lan_GetParam;
        Self->clear        = Protocol_Lan_Clear;
        Self->resend       = Protocol_Lan_Resend;
        Self->sequence_inc = Protocol_Lan_Sequence_Inc;
        Self->getresponse  = Protocol_Lan_GetResponse;

        Self->ParamList = malloc((int)PP_MAX * sizeof(char*));
        memset(Self->ParamList,0,(int)PP_MAX * sizeof(char*));

        Self->layers = malloc(4 * sizeof(Protocol_Layer_ST));
        memset(Self->layers,0,sizeof(4 * sizeof(Protocol_Layer_ST)));

        Self->layers[0] = Protocol_Application_Initialize();
        Self->layers[1] = Protocol_Encrypt_Initialize();
        Self->layers[2] = Protocol_AppHead_Initialize();
        Self->layers[3] = Protocol_Http_Initialize();

        Self->layers[0]->down = Self->layers[1];
        Self->layers[1]->down = Self->layers[2];
        Self->layers[2]->down = Self->layers[3];

        Self->layers[3]->up = Self->layers[2];
        Self->layers[2]->up = Self->layers[1];
        Self->layers[1]->up = Self->layers[0];

        Self->layers[0]->linksend = sendcallback;
        Self->layers[0]->linkrecv = recvcallback;
        Self->layers[0]->linkfree = freecallback;
        Self->layers[1]->linksend = sendcallback;
        Self->layers[1]->linkrecv = recvcallback;
        Self->layers[1]->linkfree = freecallback;
        Self->layers[2]->linksend = sendcallback;
        Self->layers[2]->linkrecv = recvcallback;
        Self->layers[2]->linkfree = freecallback;
        Self->layers[3]->linksend = sendcallback;
        Self->layers[3]->linkrecv = recvcallback;
        Self->layers[3]->linkfree = freecallback;

        Self->layers[0]->protocol = Self;
        Self->layers[1]->protocol = Self;
        Self->layers[2]->protocol = Self;
        Self->layers[3]->protocol = Self;

        Self->layers[0]->init(Self->layers[0]);
    }
    return Self;
}




