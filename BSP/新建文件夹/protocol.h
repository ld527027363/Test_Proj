
#ifndef __PROTOCOL_H
#define __PROTOCOL_H

/********************************************************
 * File	       :  protocol.h
 * Description :  Э����⺯���ӿ�
 * Author      :  ��
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾
 * Update      :  2015-06-26
 * History     :

*/

#include "protocol_typedef.h"
#include "protocol.h"  
/********************************************************
 * function    :  Protocol_Builder
 * Description :  ����Э�����
 * param       :  type Э���������
 * param       :  send ��·��ķ��ͻص�����
 * param       :  recv ��·��Ľ��պ���
 * param       :  free ��·�����ս��ջ���Ļص�����
 * return      :  ����Э������ָ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_ST Protocol_Builder(Protocol_Type_EM type,__linksend send,__linkrecv recv,__linkfree free);

/********************************************************
 * function    :  Protocol_Lan_Initialize
 * Description :  ������̫������
 * param       :  send ��·��ķ��ͻص�����
 * param       :  recv ��·��Ľ��պ���
 * param       :  free ��·�����ս��ջ���Ļص�����
 * return      :  ��̫��Э�����
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_ST Protocol_Lan_Initialize(__linksend send,__linkrecv recv,__linkfree free);

/********************************************************
 * function    :  Protocol_Link485_Initialize
 * Description :  ����485����
 * param       :  send ��·��ķ��ͻص�����
 * param       :  recv ��·��Ľ��պ���
 * param       :  free ��·�����ս��ջ���Ļص�����
 * return      :  485Э��ջ����
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_ST Protocol_Link485_Initialize(__linksend send,__linkrecv recv,__linkfree free);

/********************************************************
 * function    :  Protocol_Http_Initialize
 * Description :  ����httpЭ������
 * param       :  ��
 * return      :  ����httpЭ������
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_Layer_ST Protocol_Http_Initialize(void);

/********************************************************
 * function    :  Protocol_AppHead_Initialize
 * Description :  ����Ӧ��Э���ͷ�����
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_Layer_ST Protocol_AppHead_Initialize(void);

/********************************************************
 * function    :  Protocol_Encrypt_Initialize
 * Description :  ����Ӧ��Э��ӽ��ܲ����
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_Layer_ST Protocol_Encrypt_Initialize(void);

/********************************************************
 * function    :  Protocol_Application_Initialize
 * Description :  ����Ӧ��Э���������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_Layer_ST Protocol_Application_Initialize(void);

/********************************************************
 * function    :  Protocol_485_Initialize
 * Description :  ����485Э������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
extern PProtocol_Layer_ST Protocol_485_Initialize(void);


#endif
 

