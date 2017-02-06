/**
******************************************************************************
* @file    			DHCP.c
* @author  			WIZnet Software Team 
* @version 			V1.0
* @date    			2015-02-14
* @brief   			DHCP	Client
* @attention		ʹ��DHCP����IP��ַ��Ҫ����
******************************************************************************
*/
#include "global.h"
#include "types.h"
#include "W5500.h"
#include "socket.h"
#include "dhcp.h"
//#include "w5500_conf.h"

//#include "w5500New.h"
#include "utility_socket.h"
#include "SystemParameter.h"

#define SOCKET_DHCP 0

//#define DHCP_DEBUG

DHCP_Get DHCP_GET;

uint8*  GET_SN_MASK    = strSystemParameter.LocalSubWay;					/*��DHCP��������ȡ������������*/
uint8*  GET_GW_IP      = strSystemParameter.LocalGateway;					/*��DHCP��������ȡ�������ص�ַ*/
uint8*   GET_SIP        = strSystemParameter.LocalIp;				/*��DHCP��������ȡ���ı���IP*/

uint8    DHCP_SIP[4] = {0,};											/*�ѷ��ֵ�DNS��������ַ*/
uint8    DHCP_REAL_SIP[4] = {0,}; 								/*��DHCP�б���ѡ��ʹ�õ�DHCP������*/
uint8    OLD_SIP[4];        											/*�����ȡ����IP��ַ*/

uint8    dhcp_state       = STATE_DHCP_READY;       /*DHCP�ͻ���״̬*/
uint8    dhcp_retry_count = 0;                      /*DHCP���Դ���*/
uint8    DHCP_timeout     = 0;                      /*DHCP��ʱ��־λ*/
uint32   dhcp_lease_time;													 /*��Լʱ��*/
uint32   next_dhcp_time  = 0;											/*DHCP��ʱʱ��*/
uint32   dhcp_tick_cnt   = 0;                   	  

uint32  dhcp_time;

uint8 Conflict_flag = 0;
uint32  DHCP_XID        = DEFAULT_XID;				
uint8 EXTERN_DHCPBUF[1024];
RIP_MSG*  pRIPMSG = (RIP_MSG*)EXTERN_DHCPBUF;			/*DHCP��Ϣָ��*/


void  send_DHCP_DISCOVER(void);										/*��DHCP���������ͷ��ֱ���*/
void  send_DHCP_REQUEST(void);										/*��DHCP����������������*/
void  send_DHCP_RELEASE_DECLINE(char msgtype);		/*��DHCP�����������ͷű���*/

void  reset_DHCP_time(void);        							/*��ʼ��DHCP��ʱ��*/
void  DHCP_timer_handler(void);     							/*DHCP��ʱ��*/
void  check_DHCP_Timeout(void);     							/*����Ƿ�ʱ*/
uint8 check_leasedIP(void);												/*����ȡ��IP�Ƿ��ͻ*/
uint8 parseDHCPMSG(uint16 length);								/*��DHCP������������Ϣ������*/

/**
*@brief		DHCP��ʱ��ʼ��
*@param		��
*@return	��
*/
void reset_DHCP_time(void)
{
	dhcp_time = 0;
	dhcp_tick_cnt = 0;
	next_dhcp_time = dhcp_time + DHCP_WAIT_TIME;
	dhcp_retry_count = 0;
}
extern  unsigned char LocalMAC[6];

/**
*@brief		����DISCOVER��Ϣ��DHCP������
*@param		��
*@return	��
*/
void send_DHCP_DISCOVER(void)
{
	uint8 ip[4] = {255,255,255,255};
	uint16 i=0;
	//the host name modified
	uint8 host_name[12];

	memset((void*)pRIPMSG,0,sizeof(RIP_MSG));				/*���pRIPMSG�� sizeof(RIP_MSG)	���ռ�*/
	
	pRIPMSG->op    =   DHCP_BOOTREQUEST;
	pRIPMSG->htype =   DHCP_HTYPE10MB;
	pRIPMSG->hlen  =   DHCP_HLENETHERNET;
	pRIPMSG->hops  =   DHCP_HOPS;
	pRIPMSG->xid   =   htonl(DHCP_XID);
	pRIPMSG->secs  =   htons(DHCP_SECS);
	pRIPMSG->flags =   htons(DHCP_FLAGSBROADCAST);
    GetCPUID();
	pRIPMSG->chaddr[0] = LocalMAC[0];
	pRIPMSG->chaddr[1] = LocalMAC[1];
	pRIPMSG->chaddr[2] = LocalMAC[2];
	pRIPMSG->chaddr[3] = LocalMAC[3];
	pRIPMSG->chaddr[4] = LocalMAC[4];
	pRIPMSG->chaddr[5] = LocalMAC[5];    
	
	/* MAGIC_COOKIE */
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 24)& 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 16)& 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 8)& 0xFF);
	pRIPMSG->OPT[i++] = (uint8)(MAGIC_COOKIE& 0xFF);
	
	/* Option Request Param. */
	pRIPMSG->OPT[i++] = dhcpMessageType;
	pRIPMSG->OPT[i++] = 0x01;
	pRIPMSG->OPT[i++] = DHCP_DISCOVER;
	
	/*Client identifier*/
	pRIPMSG->OPT[i++] = dhcpClientIdentifier;
	pRIPMSG->OPT[i++] = 0x07;
	pRIPMSG->OPT[i++] = 0x01;
	pRIPMSG->OPT[i++] = LocalMAC[0];
	pRIPMSG->OPT[i++] = LocalMAC[1];
	pRIPMSG->OPT[i++] = LocalMAC[2];
	pRIPMSG->OPT[i++] = LocalMAC[3];
	pRIPMSG->OPT[i++] = LocalMAC[4];
	pRIPMSG->OPT[i++] = LocalMAC[5];
	
	// host name
	pRIPMSG->OPT[i++] = hostName;
	// set the host name
	sprintf((char*)host_name,"%.4s-%02X%02X%02X",DEVICE_ID,LocalMAC[3],LocalMAC[4],LocalMAC[5]);
	
	pRIPMSG->OPT[i++] = (uint8)strlen((char*)host_name);
	
	strcpy((char*)(&(pRIPMSG->OPT[i])),(char*)host_name);
	
	
	i+=(uint16)strlen((char*)host_name);	
	
	pRIPMSG->OPT[i++] = dhcpParamRequest;
	pRIPMSG->OPT[i++] = 0x06;
	pRIPMSG->OPT[i++] = subnetMask;
	pRIPMSG->OPT[i++] = routersOnSubnet;
	pRIPMSG->OPT[i++] = dns;
	pRIPMSG->OPT[i++] = domainName;
	pRIPMSG->OPT[i++] = dhcpT1value;
	pRIPMSG->OPT[i++] = dhcpT2value;
	pRIPMSG->OPT[i++] = endOption;
	
	/* send broadcasting packet */
	//printf("send dhcp discover %s\r\n",EXTERN_DHCPBUF);
	W5500_SocketSendto(SOCKET_DHCP, (uint8 *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);	

	#ifdef DHCP_DEBUG	    
	printf("sent DHCP_DISCOVER\r\n");
	#endif	
}

/**
*@brief		��������Ϣ���͵�DHCP������
*@param		��
*@return	��
*/
void send_DHCP_REQUEST(void)
{
	uint8  ip[4];
	uint16 i = 0;
	uint8  host_name[12];
	
	memset((void*)pRIPMSG,0,sizeof(RIP_MSG));
	
	pRIPMSG->op = DHCP_BOOTREQUEST;
	pRIPMSG->htype = DHCP_HTYPE10MB;
	pRIPMSG->hlen = DHCP_HLENETHERNET;
	pRIPMSG->hops = DHCP_HOPS;
	pRIPMSG->xid = htonl(DHCP_XID);
	pRIPMSG->secs = htons(DHCP_SECS);
	
	if(dhcp_state < STATE_DHCP_LEASED)
		pRIPMSG->flags = htons(DHCP_FLAGSBROADCAST);
	else
	{
		pRIPMSG->flags = 0;		// For Unicast
		memcpy(pRIPMSG->ciaddr,GET_SIP,4);
	}		
	
	memcpy(pRIPMSG->chaddr,LocalMAC,6);
	
	/* MAGIC_COOKIE */
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 24) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 16) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 8) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)(MAGIC_COOKIE & 0xFF);
	
	/* Option Request Param. */
	pRIPMSG->OPT[i++] = dhcpMessageType;
	pRIPMSG->OPT[i++] = 0x01;
	pRIPMSG->OPT[i++] = DHCP_REQUEST;
	
	pRIPMSG->OPT[i++] = dhcpClientIdentifier;
	pRIPMSG->OPT[i++] = 0x07;
	pRIPMSG->OPT[i++] = 0x01;
	pRIPMSG->OPT[i++] = 0;
	pRIPMSG->OPT[i++] = 1;
	pRIPMSG->OPT[i++] = 2;
	pRIPMSG->OPT[i++] = 3;
	pRIPMSG->OPT[i++] = 4;
	pRIPMSG->OPT[i++] = 5;	
	
	
	if(dhcp_state < STATE_DHCP_LEASED)
	{
		pRIPMSG->OPT[i++] = dhcpRequestedIPaddr;
		pRIPMSG->OPT[i++] = 0x04;
		pRIPMSG->OPT[i++] = GET_SIP[0];
		pRIPMSG->OPT[i++] = GET_SIP[1];
		pRIPMSG->OPT[i++] = GET_SIP[2];
		pRIPMSG->OPT[i++] = GET_SIP[3];
		
		pRIPMSG->OPT[i++] = dhcpServerIdentifier;
		pRIPMSG->OPT[i++] = 0x04;
		pRIPMSG->OPT[i++] = DHCP_SIP[0];
		pRIPMSG->OPT[i++] = DHCP_SIP[1];
		pRIPMSG->OPT[i++] = DHCP_SIP[2];
		pRIPMSG->OPT[i++] = DHCP_SIP[3];
	}
	
	// host name
	pRIPMSG->OPT[i++] = hostName;
	//set the host name
	sprintf((char*)host_name,(char*)"%.4s-02X02X02X",DEVICE_ID);
	pRIPMSG->OPT[i++] = (uint8)strlen((char*)host_name);	
	strcpy((char*)&(pRIPMSG->OPT[i]),(char*)host_name);
	i+=strlen((char*)host_name);
		
	pRIPMSG->OPT[i++] = dhcpParamRequest;
	pRIPMSG->OPT[i++] = 0x08;
	pRIPMSG->OPT[i++] = subnetMask;
	pRIPMSG->OPT[i++] = routersOnSubnet;
	pRIPMSG->OPT[i++] = dns;
	pRIPMSG->OPT[i++] = domainName;
	pRIPMSG->OPT[i++] = dhcpT1value;
	pRIPMSG->OPT[i++] = dhcpT2value;
	pRIPMSG->OPT[i++] = performRouterDiscovery;
	pRIPMSG->OPT[i++] = staticRoute;
	pRIPMSG->OPT[i++] = endOption;
	
	/* send broadcasting packet */
	if(dhcp_state < STATE_DHCP_LEASED) memset(ip,0xFF,4);
	else 
		memcpy(ip,DHCP_SIP,4);
		W5500_SocketSendto(SOCKET_DHCP, (const uint8 *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
	#ifdef DHCP_DEBUG
		printf("sent DHCP_REQUEST\r\n");
	#endif
}

/**
*@brief		�����ͷ���Ϣ
*@param		msgtype:�Ƿ�Ϊ7
*@return	��
*/
void send_DHCP_RELEASE_DECLINE(char msgtype)
{
	uint16 i =0;
	uint8  ip[4];
	
	memset((void*)pRIPMSG,0,sizeof(RIP_MSG));
	
	pRIPMSG->op = DHCP_BOOTREQUEST;
	pRIPMSG->htype = DHCP_HTYPE10MB;
	pRIPMSG->hlen = DHCP_HLENETHERNET;
	pRIPMSG->hops = DHCP_HOPS;
	pRIPMSG->xid = htonl(DHCP_XID);
	pRIPMSG->secs = htons(DHCP_SECS);
	pRIPMSG->flags =0;// DHCP_FLAGSBROADCAST;
	
	memcpy(pRIPMSG->chaddr,LocalMAC,6);
	
	/* MAGIC_COOKIE */
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 24) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 16) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 8) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)(MAGIC_COOKIE & 0xFF);
	
	/* Option Request Param. */
	pRIPMSG->OPT[i++] = dhcpMessageType;
	pRIPMSG->OPT[i++] = 0x01;
	pRIPMSG->OPT[i++] = ((!msgtype) ? DHCP_RELEASE : DHCP_DECLINE);
	
	pRIPMSG->OPT[i++] = dhcpClientIdentifier;
	pRIPMSG->OPT[i++] = 0x07;
	pRIPMSG->OPT[i++] = 0x01;
	pRIPMSG->OPT[i++] = 0;
	pRIPMSG->OPT[i++] = 1;
	pRIPMSG->OPT[i++] = 2;
	pRIPMSG->OPT[i++] = 3;
	pRIPMSG->OPT[i++] = 4;
	pRIPMSG->OPT[i++] = 5;	
	
	pRIPMSG->OPT[i++] = dhcpServerIdentifier;
	pRIPMSG->OPT[i++] = 0x04;
	pRIPMSG->OPT[i++] = DHCP_SIP[0];
	pRIPMSG->OPT[i++] = DHCP_SIP[1];
	pRIPMSG->OPT[i++] = DHCP_SIP[2];
	pRIPMSG->OPT[i++] = DHCP_SIP[3];
	
	if(msgtype)
	{
	pRIPMSG->OPT[i++] = dhcpRequestedIPaddr;
	pRIPMSG->OPT[i++] = 0x04;
	pRIPMSG->OPT[i++] = GET_SIP[0];
	pRIPMSG->OPT[i++] = GET_SIP[1];
	pRIPMSG->OPT[i++] = GET_SIP[2];
	pRIPMSG->OPT[i++] = GET_SIP[3];		
	#ifdef DHCP_DEBUG		
	printf("sent DHCP_DECLINE\r\n");
	#endif		
	}
	#ifdef DHCP_DEBUG	
	else
	{
	printf("sent DHCP_RELEASE\r\n");
	}
	#endif	
	
	pRIPMSG->OPT[i++] = endOption;	
	
	
	if(msgtype) memset(ip,0xFF,4);
	else 
	memcpy(ip,DHCP_SIP,4);
	//printf("send dhcp decline\r\n");
	W5500_SocketSendto(SOCKET_DHCP, (uint8 *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
}

/**	 
*@brief	 	������õ�DHCP��Ϣ
*@param		length:������Ϣ����
*@return	0������ʧ��  �����������ɹ�
*/
uint8 parseDHCPMSG(uint16 length)
{
	uint8  svr_addr[6];
	uint16 svr_port;
	uint16 len;
	uint8  * p;
	uint8  * e;
	uint8  type;
	uint8  opt_len = 0;
	
	len = W5500_SocketRecvfrom(SOCKET_DHCP, (uint8 *)pRIPMSG, length, svr_addr, &svr_port);
	
	#ifdef DHCP_DEBUG
		printf("DHCP_SIP:%u.%u.%u.%u\r\n",DHCP_SIP[0],DHCP_SIP[1],DHCP_SIP[2],DHCP_SIP[3]);
		printf("DHCP_RIP:%u.%u.%u.%u\r\n",DHCP_REAL_SIP[0],DHCP_REAL_SIP[1],DHCP_REAL_SIP[2],DHCP_REAL_SIP[3]);
		printf("svr_addr:%u.%u.%u.%u\r\n",svr_addr[0],svr_addr[1],svr_addr[2],svr_addr[3]);
	#endif	
	if(pRIPMSG->op != DHCP_BOOTREPLY || svr_port != DHCP_SERVER_PORT)
	{
		#ifdef DHCP_DEBUG	   
		printf("DHCP : NO DHCP MSG\r\n");
		#endif		
		return 0;
	}
	if(memcmp(pRIPMSG->chaddr,LocalMAC,6) != 0 || pRIPMSG->xid != htonl(DHCP_XID))
	{
		#ifdef DHCP_DEBUG
		printf("No My DHCP Message. This message is ignored.\r\n");
		printf("\tSRC_MAC_ADDR(%02X.%02X.%02X.",LocalMAC[0],LocalMAC[1],LocalMAC[2]);
		printf("%02X.%02X.%02X)",LocalMAC[3],LocalMAC[4],LocalMAC[5]);
		printf(", pRIPMSG->chaddr(%02X.%02X.%02X.",(char)pRIPMSG->chaddr[0],(char)pRIPMSG->chaddr[1],(char)pRIPMSG->chaddr[2]);
		printf("%02X.%02X.%02X)",pRIPMSG->chaddr[3],pRIPMSG->chaddr[4],pRIPMSG->chaddr[5]);
		printf("\tpRIPMSG->xid(%08X), DHCP_XID(%08X)",pRIPMSG->xid,(DHCP_XID));
		printf("\tpRIMPMSG->yiaddr:%d.%d.%d.%d\r\n",pRIPMSG->yiaddr[0],pRIPMSG->yiaddr[1],pRIPMSG->yiaddr[2],pRIPMSG->yiaddr[3]);
		#endif				
		return 0;
	}
	
	if( *((uint32*)DHCP_SIP) != 0x00000000 )
	{
		if( *((uint32*)DHCP_REAL_SIP) != *((uint32*)svr_addr) && 
		*((uint32*)DHCP_SIP) != *((uint32*)svr_addr) ) 
		{
			#ifdef DHCP_DEBUG		
			printf("Another DHCP sever send a response message. This is ignored.\r\n");
			printf("\tIP:%u.%u.%u.%u\r\n",svr_addr[0],svr_addr[1],svr_addr[2],svr_addr[3]);
			#endif				
			return 0;								
		}
	}	
    memcpy(GET_SIP,pRIPMSG->yiaddr,4);
	#ifdef DHCP_DEBUG	
		printf("DHCP MSG received\r\n");
		printf("yiaddr : %u.%u.%u.%u\r\n",GET_SIP[0],GET_SIP[1],GET_SIP[2],GET_SIP[3]);
	#endif

	type = 0;
	p = (uint8 *)(&pRIPMSG->op);
	p = p + 240;
	e = p + (len - 240);
	#ifdef DHCP_DEBUG
		printf("p : %08X  e : %08X  len : %d\r\n", (uint32)p, (uint32)e, len);
	#endif
	while ( p < e ) 
	{
		switch ( *p++ ) 
		{
			case endOption :
				return type;
		
			case padOption :
				break;
			
			case dhcpMessageType :
				opt_len = *p++;
				type = *p;
				#ifdef DHCP_DEBUG			
					printf("dhcpMessageType : %02X\r\n", type);
				#endif			
				break;
			
			case subnetMask :
				opt_len =* p++;
				memcpy(GET_SN_MASK,p,4);
				#ifdef DHCP_DEBUG
					printf("subnetMask : ");
					printf("%u.%u.%u.%u\r\n",GET_SN_MASK[0],GET_SN_MASK[1],GET_SN_MASK[2],GET_SN_MASK[3]);
				#endif	
                if(GET_SN_MASK[0] == 0 && GET_SN_MASK[1] == 0 && GET_SN_MASK[2] == 0 && GET_SN_MASK[3] == 0);
                else
                {
                    if(GET_SIP[0] == 0 && GET_SIP[1] == 0 && GET_SIP[2] == 0 && GET_SIP[3] == 0);
                    else
                    {
                        if(DHCP_REAL_SIP[0] == 0 && DHCP_REAL_SIP[1] == 0 && DHCP_REAL_SIP[2] == 0 && DHCP_REAL_SIP[3] == 0);
                        else
                        {
                            memcpy(GET_GW_IP, DHCP_REAL_SIP, 4);
                            dhcp_state = STATE_DHCP_REREQUEST;
                            return DHCP_ACK;
                        }
                    }
                }
				break;
			
			case routersOnSubnet :
				opt_len = *p++;
				memcpy(GET_GW_IP,p,4);
				#ifdef DHCP_DEBUG
					printf("routersOnSubnet : ");
					printf("%u.%u.%u.%u\r\n",GET_GW_IP[0],GET_GW_IP[1],GET_GW_IP[2],GET_GW_IP[3]);
				#endif			
				break;
			
			case dns :
				opt_len = *p++;
				//memcpy(GET_DNS_IP,p,4);
                //printf("Dns\r\n");
				break;
			
			case dhcpIPaddrLeaseTime :
				opt_len = *p++;
				dhcp_lease_time = ntohl(*((uint32*)p));
				#ifdef DHCP_DEBUG			
					printf("dhcpIPaddrLeaseTime : %d\r\n", dhcp_lease_time);
				#endif			
				break;
			
			case dhcpServerIdentifier :
				opt_len = *p++;
				#ifdef DHCP_DEBUG						
					printf("DHCP_SIP : %u.%u.%u.%u\r\n", DHCP_SIP[0], DHCP_SIP[1], DHCP_SIP[2], DHCP_SIP[3]);
				#endif			
				if( *((uint32*)DHCP_SIP) == 0 || 
				*((uint32*)DHCP_REAL_SIP) == *((uint32*)svr_addr) || 
				*((uint32*)DHCP_SIP) == *((uint32*)svr_addr) )
				{
					memcpy(DHCP_SIP,p,4);
					memcpy(DHCP_REAL_SIP,svr_addr,4);	// Copy the real ip address of my DHCP server
					#ifdef DHCP_DEBUG						
						printf("My dhcpServerIdentifier : ");
						printf("%u.%u.%u.%u\r\n", DHCP_SIP[0], DHCP_SIP[1], DHCP_SIP[2], DHCP_SIP[3]);
						printf("My DHCP server real IP address : ");
						printf("%u.%u.%u.%u\r\n", DHCP_REAL_SIP[0], DHCP_REAL_SIP[1], DHCP_REAL_SIP[2], DHCP_REAL_SIP[3]);
					#endif						
				}
				else
				{
				#ifdef DHCP_DEBUG			   
					printf("Another dhcpServerIdentifier : \r\n");
					printf("\tMY(%u.%u.%u.%u) ", DHCP_SIP[0], DHCP_SIP[1], DHCP_SIP[2], DHCP_SIP[3]);
					printf("Another(%u.%u.%u.%u)\r\n", svr_addr[0], svr_addr[1], svr_addr[2], svr_addr[3]);
				#endif				
				}
				break;
				
			default :
				opt_len = *p++;
				#ifdef DHCP_DEBUG			
					printf("opt_len : %u\r\n", opt_len);
				#endif			
				break;
		} // switch
		p+=opt_len;
	} // while	
	return 0;
}

/**	
*@brief	 	���DHCP״̬
*@param		s: socket��
*@return	���صõ���DHCP״̬
					DHCP_RET_NONE: δ��ȡ��IP��ַ
					DHCP_RET_TIMEOUT:��ʱ
					DHCP_RET_UPDATE: ��ȡ�ɹ�
					DHCP_RET_CONFLICT:IP��ַ��ͻ
*/
unsigned char Init_DHCP_Socket(void)
{
    if(!W5500_Socket(SOCKET_DHCP,Sn_MR_UDP,DHCP_CLIENT_PORT,0x00))//��ʼ��socket�Ͷ˿�
	{
			#ifdef DHCP_DEBUG	   
				printf("Fail to create the DHCPC_SOCK(%u)\r\n",SOCK_DHCP);
			#endif   
			return FALSE;														//socket��ʼ������
	}
    else
    {
        if(W5500_getSn_SR(SOCKET_DHCP)!=SOCK_CLOSED)
        {
            return TRUE;
        }
        else return FALSE;
    }        
}

uint8 check_DHCP_state(SOCKET s) 
{
	uint16 len;   																			/*����һ����ʾ�������ݴ�С����*/
	uint8  type;
																			/*����һ����ʾ���շ�����ͱ���*/
	
	type = 0;
    if ((len = W5500_getSn_RX_RSR(s)) > 0)									/*���յ�����*/
    {
          type = parseDHCPMSG(len);												/*�����յ��ķ������*/
    }
	switch ( dhcp_state )
	{
		case STATE_DHCP_READY:													  /*DHCP��ʼ��״̬*/
			DHCP_timeout = 0;																/*DHCP��ʱ��־����Ϊ1*/
			reset_DHCP_time();															/*��λ��ʱʱ��*/
			send_DHCP_DISCOVER();														/*����DISCOVER��*/

																			/*set_timer0(DHCP_timer_handler);  */ 	
			dhcp_state = STATE_DHCP_DISCOVER;								/*DHCP��DISCOVER״̬*/
			break;	 
  
		case STATE_DHCP_DISCOVER:
			if (type == DHCP_OFFER) 
			{
				reset_DHCP_time();														/*��λ��ʱʱ��*/
				send_DHCP_REQUEST();													/*����REQUEST��*/
				dhcp_state = STATE_DHCP_REQUEST;
				#ifdef DHCP_DEBUG	
                    printf("STATE_DHCP_DISCOVER1\r\n");                
					printf("state : STATE_DHCP_REQUEST\r\n");
				#endif			
			}
			else 
				check_DHCP_Timeout();													/*����Ƿ�ʱ*/
			break;
		
		case STATE_DHCP_REQUEST :													/*DHCP��REQUEST״̬*/
			if (type == DHCP_ACK) 													/*���յ�DHCP��������Ӧ��off���*/
			{
				reset_DHCP_time();
				if (check_leasedIP()) 
				{
					#ifdef DHCP_DEBUG					
						printf("state : STATE_DHCP_LEASED\r\n");
					#endif		
					dhcp_state = STATE_DHCP_LEASED;
					return DHCP_RET_UPDATE;
				} 
				else 
				{
					#ifdef DHCP_DEBUG					
						printf("state : STATE_DHCP_DISCOVER\r\n");
					#endif				
					dhcp_state = STATE_DHCP_DISCOVER;
					return DHCP_RET_CONFLICT;
				}
			}	
			else if (type == DHCP_NAK) 
			{
				reset_DHCP_time();														/*��λ��ʱʱ��*/
				dhcp_state = STATE_DHCP_DISCOVER;
				#ifdef DHCP_DEBUG				
					printf("state : STATE_DHCP_DISCOVER\r\n");
				#endif			
			}
			else 
				check_DHCP_Timeout();													/*����Ƿ�ʱ*/
			break;
			
		case STATE_DHCP_LEASED :
			if ((dhcp_lease_time != 0xffffffff) && (dhcp_time>(dhcp_lease_time/2))) 
			{
				type = 0;
				memcpy(OLD_SIP,GET_SIP,4);
				DHCP_XID++;
				send_DHCP_REQUEST();
				dhcp_state = STATE_DHCP_REREQUEST;
				#ifdef DHCP_DEBUG
                    printf("STATE_DHCP_LEASED2\r\n");
					printf("state : STATE_DHCP_REREQUEST\r\n");
				#endif
				reset_DHCP_time();
			}
			break;
		case STATE_DHCP_REREQUEST :
			if (type == DHCP_ACK) 
			{
				if(memcmp(OLD_SIP,GET_SIP,4)!=0)	
				{
					#ifdef DHCP_DEBUG
						printf("The IP address from the DHCP server is updated.\r\n");
						printf("OLD_SIP=%u.%u.%u.%u",OLD_SIP[0],OLD_SIP[1],OLD_SIP[2],OLD_SIP[3]);
						printf("GET_SIP=%u.%u.%u.%u\r\n",GET_SIP[0],GET_SIP[1],GET_SIP[2],GET_SIP[3]);
					#endif
					return DHCP_RET_UPDATE;
				}
				#ifdef DHCP_DEBUG
				else
				{
					printf("state : STATE_DHCP_LEASED : same IP\r\n");
				}
				#endif
				reset_DHCP_time();
				dhcp_state = STATE_DHCP_LEASED;
			} 
			else if (type == DHCP_NAK) 
			{
				reset_DHCP_time();
				dhcp_state = STATE_DHCP_DISCOVER;
				#ifdef DHCP_DEBUG
					printf("state : STATE_DHCP_DISCOVER\r\n");
				#endif
			} 
			else 
				check_DHCP_Timeout();
			break;
			
		default :
			dhcp_state = STATE_DHCP_READY;
			break;
	}
	if (DHCP_timeout == 1)
	{
		dhcp_state = STATE_DHCP_READY;
		return DHCP_RET_TIMEOUT;
	}

	return DHCP_RET_NONE;
}

/**	
*@brief	 	���DHCP�����ͷų�ʱ
*@param		��
*@return	��
*/
void check_DHCP_Timeout(void)
{
	if (dhcp_retry_count < MAX_DHCP_RETRY) 
	{
		if (dhcp_time > next_dhcp_time) 
		{
			dhcp_time = 0;
			next_dhcp_time = dhcp_time + DHCP_WAIT_TIME;
			dhcp_retry_count++;
			switch ( dhcp_state ) 
			{
				case STATE_DHCP_DISCOVER :
					#ifdef DHCP_DEBUG			   
						printf("<<timeout>> state : STATE_DHCP_DISCOVER\r\n");
					#endif				
					send_DHCP_DISCOVER();
					break;
				
				case STATE_DHCP_REQUEST :
					#ifdef DHCP_DEBUG			   
						printf("<<timeout>> state : STATE_DHCP_REQUEST\r\n");
					#endif				
					send_DHCP_REQUEST();
					break;		
				
				case STATE_DHCP_REREQUEST :
					#ifdef DHCP_DEBUG			   
						printf("<<timeout>> state : STATE_DHCP_REREQUEST\r\n");
					#endif				
					send_DHCP_REQUEST();
					break;
				
				default :
					break;
			}
		}
	} 
	else 
	{
		reset_DHCP_time();
		DHCP_timeout = 1;
	
		send_DHCP_DISCOVER();
		dhcp_state = STATE_DHCP_DISCOVER;
		#ifdef DHCP_DEBUG		
			printf("timeout : state : STATE_DHCP_DISCOVER\r\n");
		#endif		
	}
}

/**	
*@brief	  ����ȡ��IP�Ƿ��ͻ
*@param		��
*@return	��
*/
 uint8 check_leasedIP(void)
{
	return 1;
}
	

/**		 
*@brief	  ��ʼ��DHCP�ͻ���
*@param		��
*@return	��
*/
void Init_DHCP_Cient(void)
{
	uint8 ip_0[4]={0,};
	DHCP_XID = 0x12345678;
	memset(OLD_SIP,0,sizeof(OLD_SIP));
	memset(DHCP_SIP,0,sizeof(DHCP_SIP));
	memset(DHCP_REAL_SIP,0,sizeof(GET_SN_MASK));
    
	/*clear ip setted flag */
	W5500_iinchip_init();
	W5500_setSUBR(ip_0);
	W5500_setGAR(ip_0);
	W5500_setSIPR(ip_0);

	W5500_set_w5500_mac();

	W5500_socket_buf_init(txsize, rxsize);
	dhcp_state = STATE_DHCP_READY;
	#ifdef DHCP_DEBUG
		printf("Init_DHCP_Cient:%u\r\n",SOCK_DHCP);
	#endif   
}
/**	
*@brief	 	ִ��DHCP Client
*@param		��
*@return	��
*/

unsigned char DHCP_GetSuccessFlag =0;
void DHCP_QueryTask(void)
{
	uint8 dhcpret=0;
	dhcpret = check_DHCP_state(SOCKET_DHCP);             /*��ȡDHCP����״̬*/
	switch(dhcpret)
	{
		case DHCP_RET_NONE:                              /*IP��ַ��ȡ���ɹ�*/ 
			break;
		
		case DHCP_RET_TIMEOUT:                           /*IP��ַ��ȡ��ʱ*/ 
			break;
		
		case DHCP_RET_UPDATE:							/*�ɹ���ȡ��IP��ַ*/                   
			printf(">>DHCP Success\r\n");
            DHCP_GetSuccessFlag = 1;
	    break;
		
		case DHCP_RET_CONFLICT:                          /*IP��ַ��ȡ��ͻ*/ 
			//printf(" ��DHCP��ȡIP��ַʧ��\r\n");
			break;     
		default:
			break;
	}

}





