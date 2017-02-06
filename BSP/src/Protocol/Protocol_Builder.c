/********************************************************
 * File	       :  protocol_builder.h
 * Description :  协议构建者，提供工厂方法创建Lan、wifi以及485方式访问的协议对象
 * Author      :  左俊
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
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
 * Description :  协议对象创建者
 * param       :  type  协议对象类型
 * param       :  send  链路层发送回调函数
 * param       :  recv  链路层接收回调函数
 * param       :  free  链路层清除接收缓存回调函数
 * return      :  协议对象
 * Author      :  左俊
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



 
