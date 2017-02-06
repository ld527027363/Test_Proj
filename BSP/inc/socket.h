
#ifndef	_SOCKET_H_
#define	_SOCKET_H_

#include "types.h"

//#define NETBIOS_SOCK    6 //在netbios.c已定义

extern uint8 socket(SOCKET s, uint8 protocol, uint16 port, uint8 flag); // Opens a socket(TCP or UDP or IP_RAW mode)
extern void close(SOCKET s); // Close socket
extern uint16 send(SOCKET s, const uint8 * buf, uint16 len); // Send data (TCP)
extern uint16 sendto(SOCKET s, const uint8 * buf, uint16 len, uint8 * addr, uint16 port); // Send data (UDP/IP RAW)
extern uint16 recvfrom(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16  *port); // Receive data (UDP/IP RAW)


#endif
/* _SOCKET_H_ */

