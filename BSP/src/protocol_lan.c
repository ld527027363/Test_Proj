/********************************************************
 * File	       :  protocol_lan.h
 * Description :  ��̫��Э�����
 * Author      :  ��
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾
 * Update      :  2015-06-26
 * History     :

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol_typedef.h"
#include "protocol.h"

static PProtocol_ST this = 0;
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
 * Description :  ��¼����
 * param       :  protocol  Э��ջ����
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static int Protocol_Lan_Login(PProtocol_ST protocol)
{
    int index = PP_MAX;
    if (!protocol)
    {
        return -1;
    }

    ///1����Э����Ƚ��г�ʼ��
    protocol->layers[0]->clear(protocol->layers[0]);
    protocol->sequence_inc(protocol);
    protocol->setparam(protocol,PP_TYPE,"2");
    protocol->setparam(protocol,PP_ERRORCODE,TAG_WAIT);
    protocol->setparam(protocol,PP_ACTION,"action_login");
    protocol->setparam(protocol,PP_HTTPTYPE,"POST");
    Protocol_Lan_ClearUpgradeCallback(protocol);
    ///2���������·���Э�����
    while(index)
    {
        protocol->layers[0]->setparam(protocol->layers[0],index-1,protocol->ParamList[index-1]);
        index--;
    }

    ///3������������Ҫ���ݴ�������ݣ��������ݰ���
    protocol->layers[0]->puttag(protocol->layers[0],PT_ACCOUNT,protocol->ParamList[PP_ACCOUNT],strlen(protocol->ParamList[PP_ACCOUNT]));
    protocol->layers[0]->puttag(protocol->layers[0],PT_PASSWORD,protocol->ParamList[PP_PASSWWORD],strlen(protocol->ParamList[PP_PASSWWORD]));
    ///4�����÷�����������з��
    protocol->layers[0]->pack(protocol->layers[0]);

    return 0;
}

/********************************************************
 * function    :  Protocol_Lan_SyncBatch
 * Description :  �����Ͳ���
 * param       :  protocol  Э��ջ����
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static int Protocol_Lan_SyncBatch(PProtocol_ST protocol)
{
    int index = PP_MAX;
    if (!protocol)
    {
        return -1;
    }

    ///1����Э����Ƚ��г�ʼ��
    //protocol->layers[0]->clear(protocol->layers[0]);
    protocol->sequence_inc(protocol);
    protocol->setparam(protocol,PP_TYPE,"3");
    protocol->setparam(protocol,PP_ERRORCODE,TAG_WAIT);
    protocol->setparam(protocol,PP_HTTPTYPE,"POST");
    Protocol_Lan_ClearUpgradeCallback(protocol);
    ///2���������·���Э�����
    while(index)
    {
        protocol->layers[0]->setparam(protocol->layers[0],index-1,protocol->ParamList[index-1]);
        index--;
    }
    protocol->layers[0]->setparam(protocol->layers[0],PP_ACTION,"action_syncbatch1_1");

    ///3�����÷�����������з��
    protocol->layers[0]->pack(protocol->layers[0]);

    return 0;
}

/********************************************************
 * function    :  Protocol_Lan_HeartPack
 * Description :  ����������
 * param       :  protocol  Э��ջ����
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static int   Protocol_Lan_HeartPack(PProtocol_ST protocol)
{
    int index = PP_MAX;
    if (!protocol)
    {
        return -1;
    }

    ///1����Э����Ƚ��г�ʼ��
    protocol->layers[0]->clear(protocol->layers[0]);
    protocol->sequence_inc(protocol);
    protocol->setparam(protocol,PP_TYPE,"3");
    protocol->setparam(protocol,PP_ERRORCODE,TAG_WAIT);
    protocol->setparam(protocol,PP_HTTPTYPE,"POST");
    Protocol_Lan_ClearUpgradeCallback(protocol);
    ///2���������·���Э�����
    while(index)
    {
        protocol->layers[0]->setparam(protocol->layers[0],index-1,protocol->ParamList[index-1]);
        index--;
    }
    protocol->layers[0]->setparam(protocol->layers[0],PP_ACTION,"action_heartbeatpack");
    ///3������������Ҫ���ݴ�������ݣ��������ݰ���
    protocol->layers[0]->puttag(protocol->layers[0],PT_DATE,protocol->ParamList[PP_CREATEDATE],strlen(protocol->ParamList[PP_CREATEDATE]));
    protocol->layers[0]->puttag(protocol->layers[0],PT_TIME,protocol->ParamList[PP_CREATETIME],strlen(protocol->ParamList[PP_CREATETIME]));
    protocol->layers[0]->puttag(protocol->layers[0],PT_MODEL,protocol->ParamList[PP_MODEL],strlen(protocol->ParamList[PP_MODEL]));
    protocol->layers[0]->puttag(protocol->layers[0],PT_SOFTVERSION,protocol->ParamList[PP_SOFTVERSION],strlen(protocol->ParamList[PP_SOFTVERSION]));
    protocol->layers[0]->puttag(protocol->layers[0],PT_HEARTBEAT,"lovicoco",strlen("lovicoco"));

    ///4�����÷�����������з��
    protocol->layers[0]->pack(protocol->layers[0]);

    return 0;
}

/********************************************************
 * function    :  Protocol_Lan_GetResponse
 * Description :  ��ȡ������Ӧ��
 * param       :  protocol  Э��ջ����
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static Protocol_RspStatus_EM   Protocol_Lan_GetResponse(PProtocol_ST protocol)
{
    char* value = 0;
    ///1�����ý���������н��
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
 * Description :  �ط�����
 * param       :  protocol  Э��ջ����
 * return      :  ��
 * Author      :  ��
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
 * Description :  ������ˮ��
 * param       :  protocol  Э��ջ����
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void   Protocol_Lan_Sequence_Inc(PProtocol_ST protocol)
{
    int __sequenceid = 0;
    unsigned char buffer[16] = {0};
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
 * Description :  ��ȡӦ������
 * param       :  protocol  Э��ջ����
 * param       :  tag       Ӧ�����ݱ�ǩ
 * param       :  buffer    Ӧ�����ݶ�ȡ����
 * return      :  ����Ӧ�����ݳ���
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static int Protocol_Lan_GetTag(PProtocol_ST protocol,Protocol_Tag_EM tag,void** buffer)
{
    return protocol->layers[0]->gettag(protocol->layers[0],tag,buffer);
}

/********************************************************
 * function    :  Protocol_Lan_Upgrade
 * Description :  Ӧ������
 * param       :  protocol  Э��ջ����
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static int   Protocol_Lan_Upgrade(PProtocol_ST protocol,__upgrade upgrade)
{
    int index = PP_MAX;
    protocol->layers[0]->clear(protocol->layers[0]);
    Protocol_Lan_SetUpgradeCallback(protocol,upgrade);
    protocol->setparam(protocol,PP_HTTPTYPE,"GET");
    protocol->setparam(protocol,PP_ERRORCODE,TAG_WAIT);
    ///�����������ʼ��ַ�ͽ�����ַ
    if (Protocol_Lan_CalcUpgradeRange(protocol) < 0)
    {
        return -1;
    }
    ///2���������·���Э�����
    while(index)
    {
        protocol->layers[0]->setparam(protocol->layers[0],index-1,protocol->ParamList[index-1]);
        index--;
    }
    
    protocol->layers[3]->pack(protocol->layers[3]);

    return 0;
}

/********************************************************
 * function    :  Protocol_Lan_PutTag
 * Description :  д��Ӧ������
 * param       :  protocol  Э��ջ����
 * param       :  tag       Ӧ�����ݱ�ǩ
 * param       :  data      ���ݻ���
 * param       :  datalen   ���ݳ���
 * return      :  0 ��ʾ���ӳɹ� С��0 ��ʾ����ʧ��
 * Author      :  ��
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
 * Description :  ���Ӧ������
 * param       :  protocol  Э��ջ����
 * return      :  ��
 * Author      :  ��
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
 * Description :  ���ò���
 * param       :  protocol  Э��ջ����
 * param       :  param     ��������
 * param       :  value     ����ֵ
 * return      :  ��
 * Author      :  ��
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

    if (__valuelen > 30)
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
    memset(__value,0,__valuelen+1);
    memcpy(__value,value,__valuelen);

    protocol->ParamList[param] = __value;

}

/********************************************************
 * function    :  Protocol_Lan_GetParam
 * Description :  Ӧ��Э���ͷЭ����ʼ������
 * param       :  protocol  Э��ջ����
 * param       :  param     ������ǩ
 * return      :  ����ֵָ��
 * Author      :  ��
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
 * Description :  ������̫��Э��ջ
 * param       :  send  ���ͻص�����
 * param       :  recv  ���ջص�����
 * param       :  free  �ͷŽ��ջ���ص�����
 * return      :  Э��ջ����
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
PProtocol_ST Protocol_Lan_Initialize(__linksend send,__linkrecv recv,__linkfree free)
{
    if (!this)
    {
        this = (PProtocol_ST)malloc(sizeof(Protocol_ST));
        memset(this,0,sizeof(Protocol_ST));

        this->login        = Protocol_Lan_Login;
        this->syncbatch    = Protocol_Lan_SyncBatch;
        this->heartpack    = Protocol_Lan_HeartPack;
        this->upgrade      = Protocol_Lan_Upgrade;
        this->puttag       = Protocol_Lan_PutTag;
        this->gettag       = Protocol_Lan_GetTag;
        this->setparam     = Protocol_Lan_SetParam;
        this->getparam     = Protocol_Lan_GetParam;
        this->clear        = Protocol_Lan_Clear;
        this->resend       = Protocol_Lan_Resend;
        this->sequence_inc = Protocol_Lan_Sequence_Inc;
        this->getresponse  = Protocol_Lan_GetResponse;

        this->ParamList = malloc(PP_MAX * sizeof(char*));
        memset(this->ParamList,0,PP_MAX * sizeof(char*));

        this->layers = malloc(4 * sizeof(Protocol_Layer_ST));
        memset(this->layers,0,sizeof(4 * sizeof(Protocol_Layer_ST)));

        this->layers[0] = Protocol_Application_Initialize();
        this->layers[1] = Protocol_Encrypt_Initialize();
        this->layers[2] = Protocol_AppHead_Initialize();
        this->layers[3] = Protocol_Http_Initialize();

        this->layers[0]->down = this->layers[1];
        this->layers[1]->down = this->layers[2];
        this->layers[2]->down = this->layers[3];

        this->layers[3]->up = this->layers[2];
        this->layers[2]->up = this->layers[1];
        this->layers[1]->up = this->layers[0];

        this->layers[0]->linksend = send;
        this->layers[0]->linkrecv = recv;
        this->layers[0]->linkfree = free;
        this->layers[1]->linksend = send;
        this->layers[1]->linkrecv = recv;
        this->layers[1]->linkfree = free;
        this->layers[2]->linksend = send;
        this->layers[2]->linkrecv = recv;
        this->layers[2]->linkfree = free;
        this->layers[3]->linksend = send;
        this->layers[3]->linkrecv = recv;
        this->layers[3]->linkfree = free;

        this->layers[0]->protocol = this;
        this->layers[1]->protocol = this;
        this->layers[2]->protocol = this;
        this->layers[3]->protocol = this;

        this->layers[0]->init(this->layers[0]);
    }
    return this;
}


