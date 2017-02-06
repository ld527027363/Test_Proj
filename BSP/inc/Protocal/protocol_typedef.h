
#ifndef __PROTOCOL_TYPEDEF_H
#define __PROTOCOL_TYPEDEF_H

/********************************************************
 * File	       :  protocol_typedef.h
 * Description :  Э�����Ͷ����ļ�
 * Author      :  ��
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾
 * Update      :  2015-06-26
 * History     :

*/

#ifdef __cplusplus
extern "C" {
#endif

#define assert_null(p)   do{break;}while(1)
//#define assert_null(p)   do{if (!p){exit(-1);} break;}while(1)

#define UPGRADE_REQUEST_SIZE (4096)
typedef char* Pchar;


typedef enum protocol_rspstatus_em
{
    PR_WAIT = 0x10,
    PR_SUCCESS
}Protocol_RspStatus_EM;

typedef enum protocol_param_em
{
    ///�˺�
    PP_ACCOUNT=0,
    ///����
    PP_MAGIC,
    ///����
    PP_TYPE,
    ///�ỰID
    PP_SESSIONID,
    ///��������
    PP_CREATEDATE,
    ///����ʱ��
    PP_CREATETIME,
    ///��ˮ��
    PP_SEQUENCEID,
    ///ͨ������
    PP_WEBTYPE,
    ///������
    PP_ACTION,
    ///������
    PP_HOST,
    ///�����˿�
    PP_PORT,
    ///�Ự��Կ
    PP_SESSIONKEY,
    ///��������
    PP_PASSWWORD,
    ///�����MD5ֵ
    PP_PSDMD5,
    ///�ͺ�
    PP_MODEL,
    ///����汾
    PP_SOFTVERSION,
		//AP����
    PP_APPARAM,    
		//AP�����汾
    PP_APPARAMVER,
		//AP�����б�
		PP_APWARNING,
    ///������
    PP_ERRORCODE,
    ///Ӧ����
    PP_RESULTCODE,
    ///HTTP����
    PP_HTTPTYPE,
    ///�����ļ����ص�ַ
    PP_URL,
    ///�����ļ���ʼƫ��
    PP_RANGSTART,
    ///�����ļ�����ƫ��
    PP_RANGENDEND,
    ///�����ļ���С
    PP_FILESIZE,
    PP_MAX,
		
		
}Protocol_Param_EM;

typedef enum protocol_tag_em
{
    PT_ACCOUNT = 0x01,
    PT_PASSWORD,
    PT_DATE,
    PT_TIME,
    PT_MODEL,
    PT_SOFTVERSION,
    PT_APPARAMVER,
    PT_APWARNING,
    PT_HEARTBEAT,
    PT_ACTION,
    PT_SESSIONKEY,
    
    PT_DEVICE = 0x50,
    PT_DEVICEPARAM,
}Protocol_Tag_EM;

typedef enum protocol_cmd_em
{
    PC_WALK=0x50,
    PC_RUN,
    PC_NAP,
    PC_SLEEP
}Protocol_Cmd_EM;

typedef enum protocol_type_em
{
    PT_NONE = 0,
    PT_LAN,
    PT_WIFI,
    PT_485,
    PT_MAX
}Protocol_Type_EM;

typedef struct device_collect_st
{
    Pchar deviceinfo;
    unsigned short len;
}Device_Collect_ST;

typedef  Device_Collect_ST* PDevice_Collect_ST;


typedef struct device_param_st
{
    char* deviceid;
    char* params;
    unsigned short paramlen;
}Device_Param_ST;

typedef Device_Param_ST* PDevice_Param_ST;


struct protocol_st;
typedef struct protocol_st Protocol_ST;
typedef Protocol_ST* PProtocol_ST;

struct protocol_layer_st;
typedef struct protocol_layer_st  Protocol_Layer_ST;
typedef Protocol_Layer_ST* PProtocol_Layer_ST;

typedef void           (*__init)(PProtocol_Layer_ST layer);
typedef void           (*__pack)(PProtocol_Layer_ST layer);
typedef void           (*__unpack)(PProtocol_Layer_ST layer);
typedef unsigned int   (*__getsize)(PProtocol_Layer_ST layer);
typedef void           (*__setparam)(PProtocol_Layer_ST layer,Protocol_Param_EM param,Pchar value);
typedef Pchar          (*__getparam)(PProtocol_Layer_ST layer,Protocol_Param_EM param);
typedef unsigned short (*__get)(PProtocol_Layer_ST layer,unsigned short index,unsigned char* data,unsigned short datalen);
typedef int            (*__puttag)(PProtocol_Layer_ST layer,Protocol_Tag_EM tag,unsigned char* data,unsigned short datalen);
typedef int            (*__gettag)(PProtocol_Layer_ST layer,Protocol_Tag_EM tag,void** buffer);
typedef void           (*__clear)(PProtocol_Layer_ST layer);
typedef void           (*__send)(PProtocol_Layer_ST layer);
typedef void           (*__recv)(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen);
typedef void           (*__linksend)(unsigned char* data,unsigned short datalen);
typedef unsigned char* (*__linkrecv)(unsigned short *datalen);
typedef void           (*__linkfree)(void);
typedef unsigned short (*__getcrc)(PProtocol_Layer_ST layer);
typedef void           (*__upgrade)(int start,int end,int filesize,unsigned char* data,unsigned short datalen);
typedef void           (*__setcrc)(PProtocol_Layer_ST layer,unsigned short crc);

struct protocol_layer_st
{
    PProtocol_Layer_ST  down;
    PProtocol_Layer_ST  up;
    Pchar               *ParamList;
    Pchar               data;
    unsigned short      datalen;
    PDevice_Collect_ST* deviceinfo;
    PDevice_Param_ST*   deviceparam;
    unsigned short      devicesize;
    PProtocol_ST        protocol;
    unsigned short      crc;
    __init              init;
    __pack              pack;
    __unpack            unpack;
    __getsize           getsize;
    __puttag            puttag;
    __gettag            gettag;
    __get               get;
    __clear             clear;
    __setparam          setparam;
    __getparam          getparam;
    __send              send;
    __recv              recv;
    __getcrc            getcrc;
    __setcrc            setcrc;
    __linksend          linksend;
    __linkrecv          linkrecv;
    __linkfree          linkfree;
    __upgrade           upgrade;

};



typedef int                     (*__protocol_login)(PProtocol_ST protocol);
typedef int                     (*__protocol_syncbatch)(PProtocol_ST protocol);
typedef int                     (*__protocol_heartpack)(PProtocol_ST protocol);
typedef Protocol_RspStatus_EM   (*__protocol_getresponse)(PProtocol_ST protocol);
typedef int                     (*__protocol_puttag)(PProtocol_ST protocol,Protocol_Tag_EM tag,unsigned char* data,unsigned short datalen);
typedef int                     (*__protocol_gettag)(PProtocol_ST protocol,Protocol_Tag_EM tag,void** buffer);
typedef int                     (*__protocol_upgrade)(PProtocol_ST protocol,__upgrade upgrade);
typedef void                    (*__protocol_setparam)(PProtocol_ST protocol,Protocol_Param_EM param,char* value);
typedef char*                   (*__protocol_getparam)(PProtocol_ST protocol,Protocol_Param_EM param);
typedef void                    (*__protocol_clear)(PProtocol_ST protocol);

typedef void                    (*__protocol_resend)(PProtocol_ST protocol);
typedef void                    (*__protocol_sequence_inc)(PProtocol_ST protocol);

struct protocol_st
{
    PProtocol_Layer_ST      *layers;
    Pchar*                  ParamList;
    __protocol_login        login;
    __protocol_syncbatch    syncbatch;
    __protocol_heartpack    heartpack;
    __protocol_getresponse  getresponse;
    __protocol_puttag       puttag;
    __protocol_gettag       gettag;
    __protocol_upgrade      upgrade;
    __protocol_setparam     setparam;
    __protocol_getparam     getparam;
    __protocol_clear        clear;
    __protocol_resend       resend;
    __protocol_sequence_inc sequence_inc;

};

#define DEVICE_NUM_MAX  160//
#define DEVICE_DATA_MAX  2048 //(DEVICE_NUM_MAX+2)*13


extern int g_memCnt;

#ifdef __cplusplus
}
#endif

#endif
