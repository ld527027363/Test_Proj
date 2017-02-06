/**
******************************************************************************
* @file    			Socket.c
* @author  			WIZnet Software Team 
* @version 			V1.0
* @date    			2015-xx-xx
* @brief   			Socket编程相关函数 
******************************************************************************
*/
#include "socket.h"
#include "W5500_conf.h"
#include "stdio.h"
#include "w5500New.h"

static uint16 local_port;

/**
*@brief   This function close the socket and parameter is "s" which represent the socket number
*@param		s: socket number.
*@return  None
*/
void close(SOCKET s)
{

   W5500_IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_CLOSE);

   /* wait to process the command... */
   while( W5500_IINCHIP_READ(Sn_CR(s) ) )
       ;/* ------- */
   
	W5500_IINCHIP_WRITE( Sn_IR(s) , 0xFF);	 /* all clear */
}
/**
*@brief   This Socket function initialize the channel in perticular mode, 
					and set the port and wait for W5200 done it.
*@param		s: socket number.
*@param		protocol: The socket to chose.
*@param		port:The port to bind.
*@param		flag: Set some bit of MR,such as **< No Delayed Ack(TCP) flag.
*@return  1 for sucess else 0.
*/
uint8 socket(SOCKET s, uint8 protocol, uint16 port, uint8 flag)
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
      close(s);
      W5500_IINCHIP_WRITE(Sn_MR(s) ,protocol | flag);
      if (port != 0) {
         W5500_IINCHIP_WRITE( Sn_PORT0(s) ,(uint8)((port & 0xff00) >> 8));
         W5500_IINCHIP_WRITE( Sn_PORT1(s) ,(uint8)(port & 0x00ff));
      } else {
          
          printf("local_port\r\n");
         local_port++; // if don't set the source port, set local_port number.
         W5500_IINCHIP_WRITE(Sn_PORT0(s) ,(uint8)((local_port & 0xff00) >> 8));
         W5500_IINCHIP_WRITE(Sn_PORT1(s) ,(uint8)(local_port & 0x00ff));
      }
      W5500_IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_OPEN); // run sockinit Sn_CR

      /* wait to process the command... */
      while( W5500_IINCHIP_READ(Sn_CR(s)) )
         ;
      /* ------- */
      ret = 1;
   }
   else
   {
      ret = 0;
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
uint16 sendto(SOCKET s, const uint8 * buf, uint16 len, uint8 * addr, uint16 port)
{
   uint16 ret=0;

   if (len > getIINCHIP_TxMAX(s)) 
   ret = getIINCHIP_TxMAX(s); // check size not to exceed MAX size.
   else ret = len;

   if( ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) || ((port == 0x00)) )//||(ret == 0) )
   {
      /* added return value */
       
       printf("port ==== 0\r\n");
      ret = 0;
   }
   else
   {
      W5500_IINCHIP_WRITE( Sn_DIPR0(s), addr[0]);
      W5500_IINCHIP_WRITE( Sn_DIPR1(s), addr[1]);
      W5500_IINCHIP_WRITE( Sn_DIPR2(s), addr[2]);
      W5500_IINCHIP_WRITE( Sn_DIPR3(s), addr[3]);
      W5500_IINCHIP_WRITE( Sn_DPORT0(s),(uint8)((port & 0xff00) >> 8));
      W5500_IINCHIP_WRITE( Sn_DPORT1(s),(uint8)(port & 0x00ff));
      // copy data
      send_data_processing(s, (uint8 *)buf, ret);
      W5500_IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_SEND);
      /* wait to process the command... */
      while( W5500_IINCHIP_READ( Sn_CR(s) ) )
	  ;
      /* ------- */
     while( (W5500_IINCHIP_READ( Sn_IR(s) ) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
     {
      if (W5500_IINCHIP_READ( Sn_IR(s) ) & Sn_IR_TIMEOUT)
      {
            /* clear interrupt */
      W5500_IINCHIP_WRITE( Sn_IR(s) , (Sn_IR_SEND_OK | Sn_IR_TIMEOUT)); /* clear SEND_OK & TIMEOUT */
      return 0;
      }
     }
      W5500_IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK);
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
uint16 recvfrom(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16 *port)
{
   uint8 head[8];
   uint16 data_len=0;
   uint16 ptr=0;
   uint32 addrbsb =0;
   if ( len > 0 )
   {
      ptr     = W5500_IINCHIP_READ(Sn_RX_RD0(s) );
      ptr     = ((ptr & 0x00ff) << 8) + W5500_IINCHIP_READ(Sn_RX_RD1(s));
      addrbsb = (uint32)(ptr<<8) +  (s<<5) + 0x18;
      
      switch (W5500_IINCHIP_READ(Sn_MR(s) ) & 0x07)
      {
      case Sn_MR_UDP :
        wiz_read_buf(addrbsb, head, 0x08);        
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
        wiz_read_buf(addrbsb, buf, data_len);                
        ptr += data_len;

        W5500_IINCHIP_WRITE( Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8));
        W5500_IINCHIP_WRITE( Sn_RX_RD1(s), (uint8)(ptr & 0x00ff));
        break;

      case Sn_MR_IPRAW :
//	   	printf("\r\n Sn_MR_IPRAW \r\n");
        wiz_read_buf(addrbsb, head, 0x06);
		       
        ptr += 6;
        addr[0]  = head[0];
        addr[1]  = head[1];
        addr[2]  = head[2];
        addr[3]  = head[3];
        data_len = head[4];
        data_len = (data_len << 8) + head[5];

        addrbsb  = (uint32)(ptr<<8) +  (s<<5) + 0x18;
	   
//		printf(" data：%d \r\n",data_len);
        wiz_read_buf(addrbsb, buf, data_len);
		 	        
        ptr += data_len;

        W5500_IINCHIP_WRITE( Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8));
        W5500_IINCHIP_WRITE( Sn_RX_RD1(s), (uint8)(ptr & 0x00ff));
		
        break;

      case Sn_MR_MACRAW :
//	 printf("\r\n Sn_MR_MCRAW \r\n");
        wiz_read_buf(addrbsb, head, 0x02);
        ptr+=2;
        data_len = head[0];
        data_len = (data_len<<8) + head[1] - 2;
        if(data_len > 1514)
        {
           printf("data_len over 1514\r\n");
           while(1);
        }

        addrbsb  = (uint32)(ptr<<8) +  (s<<5) + 0x18;
        wiz_read_buf(addrbsb, buf, data_len);
        ptr += data_len;

        W5500_IINCHIP_WRITE( Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8));
        W5500_IINCHIP_WRITE( Sn_RX_RD1(s), (uint8)(ptr & 0x00ff));
        break;

      default :
            break;
      }
      W5500_IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_RECV);

      /* wait to process the command... */
      while( W5500_IINCHIP_READ( Sn_CR(s)) ) ;
      /* ------- */
   }
   return data_len;
}
