#ifndef __CONFIG_H__
#define __CONFIG_H__


//#define USE_PARAM_ENABLE  //�����·����ܴ�
//#define USE_SEND433_TEST_ENABLE   //���Թ���  


/******** ϵͳ�ؼ���Ϣ���� ***********/

//�̼��汾��
#define MYVERSION  						34 // hcp 2016-1-21

//Ӳ���汾��
#define BOARDVersion 					"0405"

//���԰汾 �Զ�����������Ϣ������¹��ܵ���  (���μ�Ϊ�����汾)
#define DEBUG_MODE_VERSION

/******** ϵͳ�ؼ��������� ***********/

//�����뷢�ͻ���
#define TCP_MAX_RCV_LEN  			(6*1024)
#define TCP_MAX_SEND_LEN  		(9*1024)


///���������������
#define UPLOADDATA_TIME   		15        //n���ϴ�һ��
#define UPLOADHEART_TIME   		(60*5)   	//n���һ������ 

#define SERVER_COMM_TIMEOUT	 	20     		//ÿ�ν�����ʱ



/******** ϵͳ���ܿ������� ***********/

//������־
#define EN_USING_LOG   				1

//����BLE����
#define EN_USING_BLE					1


//������ӡ������Ϣ
#define EN_DEBUG_MSG					1

//�����¹��ܵ���
#define EN_FUN_TEST						1



/******** ��������(һ�㲻���޸�)***********/

//�汾�� ���� or ��ʽ
#ifdef DEBUG_MODE_VERSION
	#ifndef EN_DEBUG_MSG
		#define EN_DEBUG_MSG 					1
	#endif
	
	#ifndef EN_FUN_TEST
		#define EN_FUN_TEST 					1
	#endif
#else
	#define REALEASE_MODE_VERSION
#endif






#endif