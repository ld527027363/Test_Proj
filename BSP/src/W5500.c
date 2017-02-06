
#include "global.h"
#include "linklayer.h"
#include "SystemParameter.h"
#include "spi.h"
#include "W5500.h"

uint8 txsize[MAX_SOCK_NUM] = {8,2,4};//tx buffer set	K bits
uint8 rxsize[MAX_SOCK_NUM] = {8,2,4};//rx buffet set  K bits
uint32 SSIZE[MAX_SOCK_NUM]={0}; // Max Tx buffer
uint32 RSIZE[MAX_SOCK_NUM]={0}; // Max Rx buffer

unsigned char LanConnectFlag = 0;
unsigned char LocalMAC[6];



typedef struct
{
	unsigned char sock_request;		//外部对socket的请求
	unsigned char sock_status;			//socket当前状态
}STRUCT_SOCK;

void W5500_set_w5500_ip(void)
{	
	W5500_setSUBR(strSystemParameter.LocalSubWay);
	W5500_setGAR(strSystemParameter.LocalGateway);
	W5500_setSIPR(strSystemParameter.LocalIp);
}

/****************
** 2015-12-25 改为ap id做mac唯一，防冲突
**********/
void GetCPUID(void)
{
//	unsigned int mcuID[3];
	//mcuID[0] = *(__IO u32*)(0x1FFF7A10);
//	mcuID[1] = *(__IO u32*)(0x1FFF7A14);
//	mcuID[2] = *(__IO u32*)(0x1FFF7A18);
	LocalMAC[0] = 0;
	//LocalMAC[1] = (unsigned char)mcuID[1];
	
	bytesdec2hex(&strSystemParameter.ProductSn[2],&LocalMAC[1],14);
	
	//memcpy( &LocalMAC[2], &mcuID[2], 4); 
}

void W5500_set_w5500_mac(void)
{
	GetCPUID();
	W5500_setSHAR(LocalMAC);	/**/
}

void W5500_iinchip_csoff(void)
{
	CLR_W5500_CS();
}

void W5500_iinchip_cson(void)
{	
	SET_W5500_CS();
}

void W5500_Reset(void)
{
	unsigned int i = 200; 

	SET_W5500_RST();    
	Delay_100us(10);	

	CLR_W5500_RST();
	Delay_100us(100);

	SET_W5500_RST();
	SET_W5500_CS();
	while(i--)
	{
		Delay_100us(20);	//150ms
		
	}

}

uint8  W5500_IINCHIP_SpiSendData(uint8 dat)
{
	return(W5500_SPI(dat));
}

void W5500_IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
	W5500_iinchip_csoff();                              		
	W5500_IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	
	W5500_IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	W5500_IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);  
	W5500_IINCHIP_SpiSendData(data);                   
	W5500_iinchip_cson();                            
}

unsigned char W5500_IINCHIP_READ(unsigned int addrbsb)
{
	uint8 data = 0;
	W5500_iinchip_csoff();                            
	W5500_IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	W5500_IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	W5500_IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
	data = W5500_IINCHIP_SpiSendData(0x00);            
	W5500_iinchip_cson();   
	return data;    
}

uint16 W5500_wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
	uint16 idx = 0;
	W5500_iinchip_csoff();                               
	W5500_IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	W5500_IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	W5500_IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4); 
	for(idx = 0; idx < len; idx++)
	{
		W5500_IINCHIP_SpiSendData(buf[idx]);
	}

	W5500_iinchip_cson();    
	return len;  
}


uint16 W5500_wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
	uint16 idx = 0;
	if(len == 0)
	{
	return 0;
	}
	W5500_iinchip_csoff();                                
	W5500_IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	W5500_IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	W5500_IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    
	for(idx = 0; idx < len; idx++)                   
	{
	buf[idx] = W5500_IINCHIP_SpiSendData(0x00);
	}
	W5500_iinchip_cson();                                  
	return len;
}


void W5500_iinchip_init(void)
{
	W5500_setMR( MR_RST );
}

void W5500_setGAR(uint8 * addr )
{
    W5500_wiz_write_buf(W5500_GAR0, addr, 4);
}

void W5500_getGAR(uint8 * addr)
{
    W5500_wiz_read_buf(W5500_GAR0, addr, 4);
}

unsigned char Get_W5500Version(void)
{
    return W5500_IINCHIP_READ(W5500_VERSIONR); 
}

void W5500_setSUBR(uint8 * addr)
{   
    W5500_wiz_write_buf(W5500_SUBR0, addr, 4);
}

void W5500_setSHAR(uint8 * addr)
{
	W5500_wiz_write_buf(W5500_SHAR0, addr, 6);  
}

void W5500_setSIPR(uint8 * addr)
{
    W5500_wiz_write_buf(W5500_SIPR0, addr, 4);  
}

void W5500_getSUBR(uint8 * addr)
{
    W5500_wiz_read_buf(W5500_SUBR0, addr, 4);
}

void W5500_getSHAR(uint8 * addr)
{
    W5500_wiz_read_buf(W5500_SHAR0, addr, 6);
}

void W5500_getSIPR(uint8 * addr)
{
    W5500_wiz_read_buf(W5500_SIPR0, addr, 4);
}

void W5500_setMR(uint8 val)
{
	W5500_IINCHIP_WRITE(W5500_MR,val);
}

uint8 W5500_getIR( void )
{
	return W5500_IINCHIP_READ(W5500_IR);
}

void W5500_setRTR(uint16 timeout)
{
	W5500_IINCHIP_WRITE(W5500_RTR0,(uint8)((timeout & 0xff00) >> 8));
	W5500_IINCHIP_WRITE(W5500_RTR1,(uint8)(timeout & 0x00ff));
}

void W5500_setRCR(uint8 retry)
{
	W5500_IINCHIP_WRITE(W5500_WIZ_RCR,retry);
}

void W5500_clearIR(uint8 mask)
{
	W5500_IINCHIP_WRITE(W5500_IR, ~mask | W5500_getIR() ); 
}

void W5500_setSn_MSS(SOCKET s, uint16 iSn_MSSR)
{
	W5500_IINCHIP_WRITE( W5500_Sn_MSSR0(s), (uint8)((iSn_MSSR & 0xff00) >> 8));
	W5500_IINCHIP_WRITE( W5500_Sn_MSSR1(s), (uint8)(iSn_MSSR & 0x00ff));
}

void W5500_setSn_TTL(SOCKET s, uint8 ttl)
{    
	W5500_IINCHIP_WRITE( W5500_Sn_TTL(s) , ttl);
}

uint8 W5500_getSn_IR(SOCKET s)
{
	return W5500_IINCHIP_READ(W5500_Sn_IR(s));
}

void W5500_setSn_IR(uint8 s, uint8 val)
{
	W5500_IINCHIP_WRITE(W5500_Sn_IR(s), val);
}

uint8 W5500_getSn_SR(SOCKET s)
{
	return W5500_IINCHIP_READ(W5500_Sn_SR(s));
}

uint16 W5500_getSn_TX_FSR(SOCKET s)
{
	uint16 val=0,val1=0;
	do
	{
		val1 = W5500_IINCHIP_READ(W5500_Sn_TX_FSR0(s));
		val1 = (val1 << 8) + W5500_IINCHIP_READ(W5500_Sn_TX_FSR1(s));
		if (val1 != 0)
		{
			val = W5500_IINCHIP_READ(W5500_Sn_TX_FSR0(s));
			val = (val << 8) + W5500_IINCHIP_READ(W5500_Sn_TX_FSR1(s));
		}
	} while (val != val1);
	return val;
}


uint16 W5500_getSn_RX_RSR(SOCKET s)
{
	uint16 val=0,val1=0;
	do
	{
		val1 = W5500_IINCHIP_READ(W5500_Sn_RX_RSR0(s));
		val1 = (val1 << 8) + W5500_IINCHIP_READ(W5500_Sn_RX_RSR1(s));
		if(val1 != 0)
		{
			val = W5500_IINCHIP_READ(W5500_Sn_RX_RSR0(s));
			val = (val << 8) + W5500_IINCHIP_READ(W5500_Sn_RX_RSR1(s));
		}
	} while (val != val1);
	return val;
}

uint32 W5500_getIINCHIP_TxMAX(SOCKET s)
{
	return SSIZE[s];
}

void W5500_send_data_processing(SOCKET s, uint8 *data, uint16 len)
{
	uint16 ptr =0;
	uint32 addrbsb =0;

	ptr = W5500_IINCHIP_READ( W5500_Sn_TX_WR0(s) );
	ptr = ((ptr & 0x00ff) << 8) + W5500_IINCHIP_READ(W5500_Sn_TX_WR1(s));

	addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x10;
	W5500_wiz_write_buf(addrbsb, data, len);

	ptr += len;
	W5500_IINCHIP_WRITE( W5500_Sn_TX_WR0(s) ,(uint8)((ptr & 0xff00) >> 8));
	W5500_IINCHIP_WRITE( W5500_Sn_TX_WR1(s),(uint8)(ptr & 0x00ff));
}


void W5500_recv_data_processing(SOCKET s, uint8 *data, uint16 len)
{
	uint16 ptr = 0;
	uint32 addrbsb = 0;
	if(len == 0)
	{
		return;
	}
	ptr = W5500_IINCHIP_READ( W5500_Sn_RX_RD0(s) );
	ptr = ((ptr & 0x00ff) << 8) + W5500_IINCHIP_READ( W5500_Sn_RX_RD1(s) );

	addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x18;
	W5500_wiz_read_buf(addrbsb, data, len);
	ptr += len;

	W5500_IINCHIP_WRITE( W5500_Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8));
	W5500_IINCHIP_WRITE( W5500_Sn_RX_RD1(s), (uint8)(ptr & 0x00ff));
}


//hcp
void W5500_socket_buf_init(uint8 * tx_size, uint8 * rx_size)
{
  int i;
	int txTemp,rxTemp;
	int ssum=0,rsum=0;
	
	// 缓存 大于16k就断言 hcp 2015-10-13
	txTemp = 0;
	rxTemp = 0;
	for (i = 0 ; i < MAX_SOCK_NUM; i++)
  {
		txTemp += tx_size[i];
		rxTemp += rx_size[i];
	}
	if (txTemp >16 || rxTemp >16)
	{
		//报错
	}
		
	
  for (i = 0 ; i < MAX_SOCK_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
  {
          W5500_IINCHIP_WRITE( (W5500_Sn_TXMEM_SIZE(i)), tx_size[i]);
          W5500_IINCHIP_WRITE( (W5500_Sn_RXMEM_SIZE(i)), rx_size[i]);
          
#ifdef __DEF_IINCHIP_DBG__
         printf("tx_size[%d]: %d, Sn_TXMEM_SIZE = %d\r\n",i, tx_size[i], IINCHIP_READ(Sn_TXMEM_SIZE(i)));
         printf("rx_size[%d]: %d, Sn_RXMEM_SIZE = %d\r\n",i, rx_size[i], IINCHIP_READ(Sn_RXMEM_SIZE(i)));
#endif
		SSIZE[i] = (int16)(0);
    RSIZE[i] = (int16)(0);
	
    if (ssum <= 16384)
    {
			SSIZE[i] = (int16)tx_size[i]*(1024);
    }

		if (rsum <= 16384)
		{
			RSIZE[i]=(int16)rx_size[i]*(1024);
    }
			ssum += SSIZE[i];
			rsum += RSIZE[i];
  }
}

void W5500_SocketClose(SOCKET s)
{
	W5500_IINCHIP_WRITE( W5500_Sn_CR(s) ,Sn_CR_CLOSE);
   
	while( W5500_IINCHIP_READ(W5500_Sn_CR(s) ) )						/*Wait to process the command*/
	;  
	W5500_IINCHIP_WRITE( W5500_Sn_IR(s) , 0xFF);					/*All clear*/
}

uint8 W5500_SocketListen(SOCKET s)
{
	uint8 ret;
	if (W5500_IINCHIP_READ( W5500_Sn_SR(s) ) == SOCK_INIT)
	{
		W5500_IINCHIP_WRITE( W5500_Sn_CR(s) ,Sn_CR_LISTEN);

		while( W5500_IINCHIP_READ(W5500_Sn_CR(s) ) )				 	/*Wait to process the command*/
		 ;
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	return ret;
}

uint16 W5500_SocketSend(SOCKET s, const uint8 * buf, uint16 len)
{
	uint8 status=0;
	uint16 ret=0;
	uint16 freesize=0;
	ret = len;

	// if freebuf is available, start.
	do
	{
		freesize = W5500_getSn_TX_FSR(s);
		status = W5500_IINCHIP_READ(W5500_Sn_SR(s));
		if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
		{
			ret = 0;
			break;
		}
	} while (freesize < ret);
	// copy data
	W5500_send_data_processing(s, (uint8 *)buf, ret);
	W5500_IINCHIP_WRITE( W5500_Sn_CR(s) ,Sn_CR_SEND);
	/* wait to process the command... */
	while( W5500_IINCHIP_READ(W5500_Sn_CR(s) ) );
	while ( (W5500_IINCHIP_READ(W5500_Sn_IR(s) ) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
	{
		status = W5500_IINCHIP_READ(W5500_Sn_SR(s));
		if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT) )
		{
			//printf("SEND_OK Problem!!\r\n");
			W5500_SocketClose(s);
			return 0;
		}
	}
	W5500_IINCHIP_WRITE( W5500_Sn_IR(s) , Sn_IR_SEND_OK);

	return ret;
}

uint16 W5500_SocketRecv(SOCKET s, uint8 * buf, uint16 len)
{
	uint16 ret=0;
	if ( len > 0 )
	{
		W5500_recv_data_processing(s, buf, len);
		W5500_IINCHIP_WRITE( W5500_Sn_CR(s) ,Sn_CR_RECV);
		while( W5500_IINCHIP_READ(W5500_Sn_CR(s) ))
		  ;								 /*Wait to process the command*/												
		ret = len;
	}
	return ret;
}
 
uint8 W5500_Socket(SOCKET s, uint8 protocol, uint16 port, uint8 flag)
{
	uint8 ret;
	if (
		((protocol&0x0F) == Sn_MR_TCP)    ||
		((protocol&0x0F) == Sn_MR_UDP)    ||
		((protocol&0x0F) == Sn_MR_IPRAW)  ||  
		((protocol&0x0F) == Sn_MR_MACRAW) ||
		((protocol&0x0F) == Sn_MR_PPPOE)
	  )
	{
		W5500_SocketClose(s);
		W5500_IINCHIP_WRITE( W5500_Sn_MR(s) ,protocol | flag);
		W5500_IINCHIP_WRITE( W5500_Sn_PORT0(s) ,(uint8)((port & 0xff00) >> 8));
		W5500_IINCHIP_WRITE( W5500_Sn_PORT1(s) ,(uint8)(port & 0x00ff));
		W5500_IINCHIP_WRITE( W5500_Sn_CR(s) ,Sn_CR_OPEN); 			/*Run sockinit Sn_CR*/ 
		while( W5500_IINCHIP_READ(W5500_Sn_CR(s)) )					/*Wait to process the command*/
		 ;
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	return ret;
}


void W5500_SocketDiscon(SOCKET s)
{
	W5500_IINCHIP_WRITE( W5500_Sn_CR(s) ,Sn_CR_DISCON);
   
	while( W5500_IINCHIP_READ(W5500_Sn_CR(s) ) )						/*Wait to process the command*/
	;  
	//W5500_IINCHIP_WRITE( W5500_Sn_IR(s) , 0xFF);					/*All clear*/
}


/**
@brief   This function established  the connection for the channel in Active (client) mode.
      This function waits for the untill the connection is established.

@return  1 for success else 0.
*/
uint8 W5500_SocketConnect(SOCKET s, uint8 * addr, uint16 port)
{
	uint8 ret;				
	if							
        (
            ((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
            ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
            (port == 0x00)
        )
	{
		ret = 0;			
	}
	else					
	{
		ret = 1;
		W5500_IINCHIP_WRITE( W5500_Sn_DIPR0(s), addr[0]);
		W5500_IINCHIP_WRITE( W5500_Sn_DIPR1(s), addr[1]);
		W5500_IINCHIP_WRITE( W5500_Sn_DIPR2(s), addr[2]);
		W5500_IINCHIP_WRITE( W5500_Sn_DIPR3(s), addr[3]);
		W5500_IINCHIP_WRITE( W5500_Sn_DPORT0(s), (uint8)((port & 0xff00) >> 8));
		W5500_IINCHIP_WRITE( W5500_Sn_DPORT1(s), (uint8)(port & 0x00ff));

		W5500_IINCHIP_WRITE( W5500_Sn_CR(s) ,Sn_CR_CONNECT);						// Sn_CR????0x04,???TCP??????

		while ( W5500_IINCHIP_READ(W5500_Sn_CR(s) ) ) ;									// MCU??Sn_CR(s)???

		while ( W5500_IINCHIP_READ(W5500_Sn_SR(s)) != SOCK_SYNSENT )		// ??Sn_SR(s)???????SOCK_SYNSENT,??????????????
		{
			if(W5500_IINCHIP_READ(W5500_Sn_SR(s)) == SOCK_ESTABLISHED)	// Socket??????,????
			{
				break;
			}
			if (W5500_getSn_IR(s) & Sn_IR_TIMEOUT)								// ?ARPto?TCPto??,??
			{
				W5500_IINCHIP_WRITE(W5500_Sn_IR(s), (Sn_IR_TIMEOUT));   // ??MCU???,????
				ret = 0;																		// ret?0,????
				break;
			}
		}
	}

	return ret;
}


/**
*@brief   This function is an application I/F function which is used to send the data for other then TCP mode.
					Unlike TCP transmission, The peer's destination address and the port is needed.
*@param		s: socket number.
*@param		buf: data buffer to send.
*@param		len: data length.
*@param		addr: IP address to send.
*@param		port: IP port to send.
*@return  This function return send data size for success else 0.
*/
uint16 W5500_SocketSendto(SOCKET s, const uint8 * buf, uint16 len, uint8 * addr, uint16 port)
{
   uint16 ret=0;

   if (len > W5500_getIINCHIP_TxMAX(s)) 
   ret = W5500_getIINCHIP_TxMAX(s); // check size not to exceed MAX size.
   else ret = len;

   if( ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) || ((port == 0x00)) )//||(ret == 0) )
   {
      /* added return value */
       
       printf("port ==== 0\r\n");
      ret = 0;
   }
   else
   {
      W5500_IINCHIP_WRITE( W5500_Sn_DIPR0(s), addr[0]);
      W5500_IINCHIP_WRITE( W5500_Sn_DIPR1(s), addr[1]);
      W5500_IINCHIP_WRITE( W5500_Sn_DIPR2(s), addr[2]);
      W5500_IINCHIP_WRITE( W5500_Sn_DIPR3(s), addr[3]);
      W5500_IINCHIP_WRITE( W5500_Sn_DPORT0(s),(uint8)((port & 0xff00) >> 8));
      W5500_IINCHIP_WRITE( W5500_Sn_DPORT1(s),(uint8)(port & 0x00ff));
      // copy data
      W5500_send_data_processing(s, (uint8 *)buf, ret);
      W5500_IINCHIP_WRITE( W5500_Sn_CR(s) ,Sn_CR_SEND);
      /* wait to process the command... */
      while( W5500_IINCHIP_READ( W5500_Sn_CR(s) ) )
	  ;
      /* ------- */
     while( (W5500_IINCHIP_READ( W5500_Sn_IR(s) ) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
     {
      if (W5500_IINCHIP_READ( W5500_Sn_IR(s) ) & Sn_IR_TIMEOUT)
      {
            /* clear interrupt */
      W5500_IINCHIP_WRITE( W5500_Sn_IR(s) , (Sn_IR_SEND_OK | Sn_IR_TIMEOUT)); /* clear SEND_OK & TIMEOUT */
      return 0;
      }
     }
      W5500_IINCHIP_WRITE( W5500_Sn_IR(s) , Sn_IR_SEND_OK);
   }
   return ret;
}

/**
*@brief   This function is an application I/F function which is used to receive the data in other then
					TCP mode. This function is used to receive UDP, IP_RAW and MAC_RAW mode, and handle the header as well.
*@param		s: socket number.
*@param		buf: data buffer to receive.
*@param		len: data length.
*@param		addr: IP address to receive.
*@param		port: IP port to receive.
*@return	This function return received data size for success else 0.
*/
uint16 W5500_SocketRecvfrom(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16 *port)
{
   uint8 head[8];
   uint16 data_len=0;
   uint16 ptr=0;
   uint32 addrbsb =0;
   if ( len > 0 )
   {
      ptr     = W5500_IINCHIP_READ(W5500_Sn_RX_RD0(s) );
      ptr     = ((ptr & 0x00ff) << 8) + W5500_IINCHIP_READ(W5500_Sn_RX_RD1(s));
      addrbsb = (uint32)(ptr<<8) +  (s<<5) + 0x18;
      
      switch (W5500_IINCHIP_READ(W5500_Sn_MR(s) ) & 0x07)
      {
      case Sn_MR_UDP :
        W5500_wiz_read_buf(addrbsb, head, 0x08);        
        ptr += 8;
        // read peer's IP address, port number.
        addr[0]  = head[0];
        addr[1]  = head[1];
        addr[2]  = head[2];
        addr[3]  = head[3];
        *port    = head[4];
        *port    = (*port << 8) + head[5];
        data_len = head[6];
        data_len = (data_len << 8) + head[7];

        addrbsb = (uint32)(ptr<<8) +  (s<<5) + 0x18;
        W5500_wiz_read_buf(addrbsb, buf, data_len);                
        ptr += data_len;

        W5500_IINCHIP_WRITE( W5500_Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8));
        W5500_IINCHIP_WRITE( W5500_Sn_RX_RD1(s), (uint8)(ptr & 0x00ff));
        break;

      case Sn_MR_IPRAW :
//	   	printf("\r\n Sn_MR_IPRAW \r\n");
        W5500_wiz_read_buf(addrbsb, head, 0x06);
		       
        ptr += 6;
        addr[0]  = head[0];
        addr[1]  = head[1];
        addr[2]  = head[2];
        addr[3]  = head[3];
        data_len = head[4];
        data_len = (data_len << 8) + head[5];

        addrbsb  = (uint32)(ptr<<8) +  (s<<5) + 0x18;
	   
//		printf(" data：%d \r\n",data_len);
        W5500_wiz_read_buf(addrbsb, buf, data_len);
		 	        
        ptr += data_len;

        W5500_IINCHIP_WRITE( W5500_Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8));
        W5500_IINCHIP_WRITE( W5500_Sn_RX_RD1(s), (uint8)(ptr & 0x00ff));
		
        break;

      case Sn_MR_MACRAW :
//	 printf("\r\n Sn_MR_MCRAW \r\n");
        W5500_wiz_read_buf(addrbsb, head, 0x02);
        ptr+=2;
        data_len = head[0];
        data_len = (data_len<<8) + head[1] - 2;
        if(data_len > 1514)
        {
           printf("data_len over 1514\r\n");
           while(1);
        }

        addrbsb  = (uint32)(ptr<<8) +  (s<<5) + 0x18;
        W5500_wiz_read_buf(addrbsb, buf, data_len);
        ptr += data_len;

        W5500_IINCHIP_WRITE( W5500_Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8));
        W5500_IINCHIP_WRITE( W5500_Sn_RX_RD1(s), (uint8)(ptr & 0x00ff));
        break;

      default :
            break;
      }
      W5500_IINCHIP_WRITE( W5500_Sn_CR(s) ,Sn_CR_RECV);

      /* wait to process the command... */
      while( W5500_IINCHIP_READ( W5500_Sn_CR(s)) ) ;
      /* ------- */
   }
   return data_len;
}


unsigned char W5500Init(void)
{
	unsigned char bTemp = 0;
	W5500_Reset();					  
	W5500_iinchip_init();
	W5500_set_w5500_mac();                 
	W5500_set_w5500_ip();                    

	W5500_IINCHIP_WRITE(W5500_SIMR,0x00);      
	//W5500_socket_buf_init(16, 16);  
	W5500_socket_buf_init(txsize, rxsize);

	W5500_wiz_read_buf(W5500_SUBR0, &bTemp, 1);  
	if( bTemp== strSystemParameter.LocalSubWay[0])
		return 1;
	else
		return 0;
}

void Lan_loopback_tcps(void)
{
	unsigned short len = 0;
	static unsigned char sock_init_flag = 0;
//	u8_t state = 0;
	
	W5500_getSn_SR(0);
	
	//if (state !=0x17)
	//printf("tcp state:0x%02x\r\n", state);
	
	switch(W5500_getSn_SR(0))											
	{
		case SOCK_INIT:		 						
			W5500_SocketConnect(0, strSystemParameter.DistinIp,strSystemParameter.DistinPort);	
			LanConnectFlag = 0;		
			sock_init_flag = 0;		
			break;
		case SOCK_SYNSENT:
			sock_init_flag = 0;
			break;
		case SOCK_ESTABLISHED:
			sock_init_flag = 0;
			if(W5500_getSn_IR(0) & Sn_IR_CON)   					
			{
				W5500_setSn_IR(0, Sn_IR_CON);
				LanConnectFlag = 1;
				
				printf("Lan Connect Server Suc\r\n");
			}
			len=W5500_getSn_RX_RSR(0);			
			if(len>0)
			{
				//printf("lan recv %d\r\n", len);
				
				if((len + g_iRevStartPos) >= TCP_MAX_RCV_LEN) 
				{
					len = W5500_SocketRecv(0,&g_arrRecvBuffer[4096],len);
				}
				else
				{
					len = W5500_SocketRecv(0,&g_arrRecvBuffer[g_iRevStartPos],len);
					g_iRevStartPos = g_iRevStartPos + len;
					if( memcmp(g_arrHttp500Error, g_arrRecvBuffer, 12) == 0)
					{
						printf("HTTP/1.1 500 ERROR\r\n");
						memset(g_arrRecvBuffer, 0, g_iRevStartPos);
						g_iRevStartPos = 0;
						g_bCommReceiveFlag = 0;
					}
					else if( memcmp(g_arrHttp400Error, g_arrRecvBuffer, 12) == 0)
					{
						printf("HTTP/1.1 400 ERROR\r\n");
						memset(g_arrRecvBuffer, 0, g_iRevStartPos);
						g_iRevStartPos = 0;
						g_bCommReceiveFlag = 0;
					}
					else if( memcmp(g_arrHttp20, g_arrRecvBuffer, 11) == 0)
					{
						g_bCommReceiveFlag = 1;
						//printf(g_arrRecvBuffer);
					}
					else
					{
						printf("HTTP/1.1 200 ERROR\r\n");
						memset(g_arrRecvBuffer, 0, g_iRevStartPos);
						g_iRevStartPos = 0;
						g_bCommReceiveFlag = 0;
					}
					
				}
			}
			break;
		case SOCK_CLOSE_WAIT:	           
			W5500_SocketClose(0);	
		  printf(" closed.\r\n");
			break;
		case SOCK_CLOSED:	  
		if(sock_init_flag == 0)
		{
			W5500_Socket(0,Sn_MR_TCP,strSystemParameter.LocalPort,0);	//Sn_MR_ND hcp 2015-10-20
			printf(" Socket closed.\r\n");
			sock_init_flag = 1;
			
			
		}
		
		break;
	}
}



uint8   W5500_getLink(void)
{
   return (W5500_IINCHIP_READ(W5500_PHYCFGR)&0x01);
}


