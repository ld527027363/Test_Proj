#ifndef SYSTEMPARAMETER_H
#define SYSTEMPARAMETER_H

#include <stdbool.h>

#define  SYS_INIT_FLAG   0x55

typedef struct
{
    unsigned char  InitialFlag;  //必须为 0x55
    unsigned char  WorkMode;     //工作模式 0x00: RS485  0x01:  LAN  0x02: WIFI
    unsigned char  Reversed1[3]; //保留
    unsigned char  BeepFlag;
	unsigned char  ProductSn[18]; //产品序列号 16个字节 暂时未用   
    unsigned char  DHCPEnable;   //DHCP使能   1 打开  0 关闭  适用于LAN   
	unsigned char  LocalIp[4];   //本地IP  适用于LAN
	unsigned char  LocalSubWay[4];   //子网掩码 适用于LAN     
	unsigned char  LocalGateway[4];  //网关     适用于LAN
    unsigned short LocalPort;    //本地IP端口  适用于LAN  用于UDP广播搜索本设备 适用于DHCP使能时查找设备
    unsigned char  DistinIp[4];  //远程IP
	unsigned short DistinPort;   //远程端口
	unsigned char  WIFISSID[34]; //WIFI SSID  32个字节 最后2个字节置0 
    unsigned char  WIFICypt;     //WIFI加密方式  高4位表示加密格式 xxxx 0x00:OPEN  0x10: WPAPSK  0x20: WPA2PSK
                                 //              低4位表示加密协议 0x00: NONE 0x01: WEP 0x02:TKIP 0x03: AES
                                 //高4位与低4位 用 | 进行组合
    unsigned char  WIFIPass[66]; //WIFI密码  64个字节有效字节   ASC明文 不能为0  最后2个字节置0
    unsigned char  RFTxChannel;
    unsigned char  RFRxChannel;
    unsigned char  Reversed2[6]; //保留2  
  
		unsigned short HraerBeatTimeout;		//心跳包间隔  单位分钟  --20160803
		unsigned short SendDataDealyTime;		//发送数据延时时间 单位分钟 --20160803  
		unsigned short SendDataMaxCnt;			//每包最大数据量--20160803  
		char					 Param_Ver[16];						//参数版本--20160803  
		unsigned char  Reversed3[6]; 				//保留2  
}Struct_System_Parameters;           //?字节


///系统参数
typedef enum sysParams_u
{
	SYS_PARAM=0x01, //获取全部
	SYS_INTF,       //接口 0x00: RS485  0x01:  LAN  0x02: WIFI
	SYS_SN,         //16位 sn
	SYS_DHCP,       
	SYS_IP,
	SYS_GW,
	SYS_MASK,
	SYS_PORT,      	//本地管理端口
	SYS_RIP,        //远程ip
	SYS_RPORT, 			//远程端口
	SYS_SSID,				
	SYS_PWD,
	SYS_TXCH,
	SYS_RXCH,
}sysParams_t;

extern unsigned int Battry_Value;
extern unsigned int Power_Value;

void SetDefaultParameter(void);

/* 检测链接状态 */
#define  sys_check_linktype(linktype)\
((strSystemParameter.WorkMode == (u8_t)linktype)?1:0)


/* 设置重启任务标记 */
#define SYS_RESTART() do{g_sysrestart_flag =1;}while(0)

bool  system_set_pram(unsigned char * dat);
unsigned char WriteSystemParameter(void);
unsigned char GetSystemParameter(unsigned char *InitialFlag);
unsigned char WriteSystemParameter(void);
bool ParameterInitial(void);
void DealSysParameter(void);

void DealSysParamBlue(void);

void DealSysParameter(void);
void DealSysParamLan(void);

extern unsigned char system_get_param(unsigned char ** dat);
extern unsigned char system_set_param(unsigned char * paramBuf, unsigned int len);

extern Struct_System_Parameters strSystemParameter;
extern unsigned char g_bStopReadFlag;

extern int bytesdec2hex(unsigned char *dec,unsigned char *hex,int len);

extern void sendBlueReset(void);

extern void sendBlueDisconn(unsigned char delay100ms);

#define		ERR_CODE_LEN						2
#define		BATTRY_GET_GAP				0x07

#define		ERR_NULL							0x0000
#define		ERR_LOSE_POWER				0x0001
#define		ERR_LOSE_BATTRY				0x0002

extern unsigned char Battry_Flag;
extern unsigned char Battry_Alert_Flag;
extern unsigned char Err_Code[ERR_CODE_LEN];

extern unsigned char DHCP_Req ;			//DHCP 申请
extern unsigned char TestReq ;

extern bool g_sysrestart_flag;
extern void sys_restart(void);

//返回系统参数  长度
extern unsigned int sys_get_params(void *pParmas, sysParams_t param);

extern void sys_BootToUpgrade(void);

extern bool sys_write_param(void);
	
#endif


