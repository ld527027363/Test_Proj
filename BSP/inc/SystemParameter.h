#ifndef SYSTEMPARAMETER_H
#define SYSTEMPARAMETER_H

#include <stdbool.h>

#define  SYS_INIT_FLAG   0x55

typedef struct
{
    unsigned char  InitialFlag;  //����Ϊ 0x55
    unsigned char  WorkMode;     //����ģʽ 0x00: RS485  0x01:  LAN  0x02: WIFI
    unsigned char  Reversed1[3]; //����
    unsigned char  BeepFlag;
	unsigned char  ProductSn[18]; //��Ʒ���к� 16���ֽ� ��ʱδ��   
    unsigned char  DHCPEnable;   //DHCPʹ��   1 ��  0 �ر�  ������LAN   
	unsigned char  LocalIp[4];   //����IP  ������LAN
	unsigned char  LocalSubWay[4];   //�������� ������LAN     
	unsigned char  LocalGateway[4];  //����     ������LAN
    unsigned short LocalPort;    //����IP�˿�  ������LAN  ����UDP�㲥�������豸 ������DHCPʹ��ʱ�����豸
    unsigned char  DistinIp[4];  //Զ��IP
	unsigned short DistinPort;   //Զ�̶˿�
	unsigned char  WIFISSID[34]; //WIFI SSID  32���ֽ� ���2���ֽ���0 
    unsigned char  WIFICypt;     //WIFI���ܷ�ʽ  ��4λ��ʾ���ܸ�ʽ xxxx 0x00:OPEN  0x10: WPAPSK  0x20: WPA2PSK
                                 //              ��4λ��ʾ����Э�� 0x00: NONE 0x01: WEP 0x02:TKIP 0x03: AES
                                 //��4λ���4λ �� | �������
    unsigned char  WIFIPass[66]; //WIFI����  64���ֽ���Ч�ֽ�   ASC���� ����Ϊ0  ���2���ֽ���0
    unsigned char  RFTxChannel;
    unsigned char  RFRxChannel;
    unsigned char  Reversed2[6]; //����2  
  
		unsigned short HraerBeatTimeout;		//���������  ��λ����  --20160803
		unsigned short SendDataDealyTime;		//����������ʱʱ�� ��λ���� --20160803  
		unsigned short SendDataMaxCnt;			//ÿ�����������--20160803  
		char					 Param_Ver[16];						//�����汾--20160803  
		unsigned char  Reversed3[6]; 				//����2  
}Struct_System_Parameters;           //?�ֽ�


///ϵͳ����
typedef enum sysParams_u
{
	SYS_PARAM=0x01, //��ȡȫ��
	SYS_INTF,       //�ӿ� 0x00: RS485  0x01:  LAN  0x02: WIFI
	SYS_SN,         //16λ sn
	SYS_DHCP,       
	SYS_IP,
	SYS_GW,
	SYS_MASK,
	SYS_PORT,      	//���ع���˿�
	SYS_RIP,        //Զ��ip
	SYS_RPORT, 			//Զ�̶˿�
	SYS_SSID,				
	SYS_PWD,
	SYS_TXCH,
	SYS_RXCH,
}sysParams_t;

extern unsigned int Battry_Value;
extern unsigned int Power_Value;

void SetDefaultParameter(void);

/* �������״̬ */
#define  sys_check_linktype(linktype)\
((strSystemParameter.WorkMode == (u8_t)linktype)?1:0)


/* �������������� */
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

extern unsigned char DHCP_Req ;			//DHCP ����
extern unsigned char TestReq ;

extern bool g_sysrestart_flag;
extern void sys_restart(void);

//����ϵͳ����  ����
extern unsigned int sys_get_params(void *pParmas, sysParams_t param);

extern void sys_BootToUpgrade(void);

extern bool sys_write_param(void);
	
#endif


