#ifndef __SYS_CONFIG_H__
#define __SYS_CONFIG_H__


//#define USE_PARAM_ENABLE  //�����·����ܴ�
//#define USE_SEND433_TEST_ENABLE   //���Թ���  

/*************************************************/

#define ERRO_LOSE_POWER			0x0001



/*************************************************/


/******** ϵͳ�ؼ���Ϣ���� ***********/

//�̼��汾��
#define MYVERSION  						55
#define VERSION_STR           "0.1.55 Bulid-161208"

//Ӳ���汾��
#define BOARDVersion 					"0405"

//���԰汾 �Զ�����������Ϣ������¹��ܵ���  (���μ�Ϊ�����汾)
#define DEBUG_MODE_VERSION

/******** ϵͳ�ؼ��������� ***********/

//�����뷢�ͻ���
#define TCP_MAX_RCV_LEN  					(6*1024)
#define TCP_MAX_SEND_LEN  				(9*1024)


///���������������
#define UPLOADDATA_TIME   				15        //n���ϴ�һ��
#define UPLOADHEART_TIME   				(60*5)   	//n���һ������ 
#define UPLOADDATA_RECORD_CNT			(100)					//����100�������ϴ�һ��

#define SERVER_COMM_TIMEOUT	 			20     		//ÿ�ν�����ʱ


//test
#define RESTRAT_TEST_TIMEOUT      (6)   //ά��ģʽ�� ϵͳ���� ��ƽʱ��8�� Լ1Сʱ

//rf433
#define DEASOM_RF433_TIMER_L1	    (300)    //5���� ���ճ�ʱ ��λоƬ
#define DEASOM_RF433_TIMER_L2     (600)    //10���� ���ճ�ʱ ��λϵͳ


//ble��������
#define ble_timeout_sec 					(60)
#define conn_timeout_sec 					(20)

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



#define  IF_FUN_TEST       	#if (EN_FUN_TEST)
#define  END_FUN_TEST				#endif


#endif //__SYS_CONFIG_H__

