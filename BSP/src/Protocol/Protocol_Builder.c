/********************************************************
 * File	       :  protocol_builder.h
 * Description :  Э�鹹���ߣ��ṩ������������Lan��wifi�Լ�485��ʽ���ʵ�Э�����
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



/********************************************************
 * function    :  Protocol_Builder
 * Description :  Э����󴴽���
 * param       :  type  Э���������
 * param       :  send  ��·�㷢�ͻص�����
 * param       :  recv  ��·����ջص�����
 * param       :  free  ��·��������ջ���ص�����
 * return      :  Э�����
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
PProtocol_ST Protocol_Builder(Protocol_Type_EM type,__linksend send,__linkrecv recv,__linkfree Free)
{
    if (type == PT_485)
    {
        return Protocol_Link485_Initialize(send,recv,Free);
    }
    if (type == PT_LAN)
    {
        return Protocol_Lan_Initialize(send,recv,Free);
    }
    return 0;
}



 
