
#ifndef __PROTOCOL_H
#define __PROTOCOL_H

/********************************************************
 * File	       :  protocol.h
 * Description :  协议对外函数接口
 * Author      :  左俊
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
 * Update      :  2015-06-26
 * History     :

*/
#include "protocol_typedef.h"

/********************************************************
 * function    :  Protocol_Builder
 * Description :  构建协议对象
 * param       :  type 协议对象类型
 * param       :  sendcallback 链路层的发送回调函数
 * param       :  recvcallback 链路层的接收函数
 * param       :  freecallback 链路层的清空接收缓存的回调函数
 * return      :  返回协议对象的指针
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_ST Protocol_Builder(Protocol_Type_EM type,__linksend sendcallback,__linkrecv recvcallback,__linkfree freecallback);

/********************************************************
 * function    :  Protocol_Lan_Initialize
 * Description :  创建以太网对象
 * param       :  send 链路层的发送回调函数
 * param       :  recv 链路层的接收函数
 * param       :  free 链路层的清空接收缓存的回调函数
 * return      :  以太网协议对象
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_ST Protocol_Lan_Initialize(__linksend sendcallback,__linkrecv recvcallback,__linkfree freecallback);

/********************************************************
 * function    :  Protocol_Link485_Initialize
 * Description :  创建485对象
 * param       :  send 链路层的发送回调函数
 * param       :  recv 链路层的接收函数
 * param       :  free 链路层的清空接收缓存的回调函数
 * return      :  485协议栈对象
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_ST Protocol_Link485_Initialize(__linksend sendcallback,__linkrecv recvcallback,__linkfree freecallback);

/********************************************************
 * function    :  Protocol_Http_Initialize
 * Description :  创建http协议层对象
 * param       :  无
 * return      :  返回http协议层对象
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_Layer_ST Protocol_Http_Initialize(void);

/********************************************************
 * function    :  Protocol_AppHead_Initialize
 * Description :  创建应用协议包头层对象
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_Layer_ST Protocol_AppHead_Initialize(void);

/********************************************************
 * function    :  Protocol_Encrypt_Initialize
 * Description :  创建应用协议加解密层对象
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_Layer_ST Protocol_Encrypt_Initialize(void);

/********************************************************
 * function    :  Protocol_Application_Initialize
 * Description :  创建应用协议包体层对象
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_Layer_ST Protocol_Application_Initialize(void);

/********************************************************
 * function    :  Protocol_485_Initialize
 * Description :  创建485协议层对象
 * param       :  layer  协议层对象
 * return      :  无
 * Author      :  左俊
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_Layer_ST Protocol_485_Initialize(void);


#endif
