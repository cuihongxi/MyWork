#include "AppCallBack.h"
#include "myWifi.h"
#include "myGPIO.H"

/**
 * ���ͳɹ��ص�����
 * */
void ICACHE_FLASH_ATTR
ESP8266_WIFI_Send_Cb(void *arg)
{
	debug("Send ok\n");
}


// TCP���ӶϿ��ɹ��Ļص�����
//================================================================
void ICACHE_FLASH_ATTR ESP8266_TCP_Disconnect_Cb_JX(void *arg)
{

	debug("\nESP8266_TCP_Disconnect_OK\n");
}

// TCP�����쳣�Ͽ�ʱ�Ļص�����
//====================================================================
void ICACHE_FLASH_ATTR ESP8266_TCP_Break_Cb_JX(void *arg,sint8 err)
{
	debug("\nESP8266_TCP_Break! ��������TCP-server ^��\n");
	espconn_connect(&ST_NetCon);	// ����TCP-server
}

//�յ����ݵĻص�����
void ESP8266_WIFI_Recv_Cb(void * arg, char * pdata, unsigned short len)
{

	// ������������LED����/��
//	if(pdata[0] == 'k' || pdata[0] == 'K')	LED_ON();			// ����ĸΪ'k'/'K'������
//	else if(pdata[0] == 'g' || pdata[0] == 'G')	LED_OFF();		// ����ĸΪ'g'/'G'������
	debug("\r\n %d.%d.%d.%d:%d	��",\
			ST_NetCon.proto.tcp->remote_ip[0],	ST_NetCon.proto.tcp->remote_ip[1],\
			ST_NetCon.proto.tcp->remote_ip[2],ST_NetCon.proto.tcp->remote_ip[3],ST_NetCon.proto.tcp->remote_port);
	debug("%s\n",pdata);

}

// TCP���ӽ����ɹ��Ļص�����
//====================================================================================================================
void ICACHE_FLASH_ATTR ESP8266_TCP_Connect_Cb_JX(void *arg)
{
	debug("\n--------------- ESP8266_TCP_Connect_OK ---------------\n");
	ESP8266_Get_ConInfo(arg,&ST_NetCon);
	// ��ӡ���ӵ�Զ��IP
	debug("\r\n remote_ip = %d.%d.%d.%d\r\n",\
			ST_NetCon.proto.tcp->remote_ip[0],	ST_NetCon.proto.tcp->remote_ip[1],\
			ST_NetCon.proto.tcp->remote_ip[2],ST_NetCon.proto.tcp->remote_ip[3]);
	//ESP8266_SendMessage(&ST_NetCon,*ESP8266_REMOTE_IP(&ST_NetCon),80,HTTP_Message_485Comm);//������Ϣ

}

// DNS_������������_�ص�����������1�������ַ���ָ�� / ����2��IP��ַ�ṹ��ָ�� / ����3���������ӽṹ��ָ�롿
//=========================================================================================================
void ICACHE_FLASH_ATTR DNS_Over_Cb_JX(const char * name, ip_addr_t *ipaddr, void *arg)
{
	struct espconn * T_arg = (struct espconn *)arg;	// �����������ӽṹ��ָ��
	//������������������������������������������������������������
	if(ipaddr == NULL)		// ��������ʧ��
	{
		debug("\r\n---- ��������ʧ�� ----\r\n");
		return;
	}

	//����������������������������������������������������������������������������������
	else if (ipaddr != NULL && ipaddr->addr != 0)		// ���������ɹ�
	{
		debug("\r\n---- ���������ɹ� ----\r\n");
		debug("---> name :%s\n",name);

		*ESP8266_REMOTE_IP(T_arg) = ipaddr->addr;// ���������ķ�����IP��ַ��ΪTCP���ӵ�Զ��IP��ַ
		debug("\r\n remote_ip = %d.%d.%d.%d : %d\r\n",\
				ST_NetCon.proto.tcp->remote_ip[0],	ST_NetCon.proto.tcp->remote_ip[1],\
				ST_NetCon.proto.tcp->remote_ip[2],ST_NetCon.proto.tcp->remote_ip[3],ST_NetCon.proto.tcp->remote_port);

		// ���� TCP server
		//----------------------------------------------------------
		ESP8266_STA_TCPClient_NetCon_ByInt(arg,*ESP8266_REMOTE_IP(&ST_NetCon),ST_NetCon.proto.tcp->remote_port);

	}
}

// SmartConfig״̬�����ı�ʱ������˻ص�����
//--------------------------------------------
// ����1��sc_status status / ����2��������ָ�롾�ڲ�ͬ״̬�£�[void *pdata]�Ĵ�������ǲ�ͬ�ġ�
//=================================================================================================================
void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata)
{
	debug("\r\n------ smartconfig_done ------\r\n");	// ESP8266����״̬�ı�

    switch(status)
    {
        case SC_STATUS_WAIT:						// ��ʼֵ, CmartConfig�ȴ�
            debug("\r\nSC_STATUS_WAIT\r\n");
        break;
        case SC_STATUS_FIND_CHANNEL:				// ���֡�WIFI�źš���8266������״̬�µȴ�������
            debug("\r\nSC_STATUS_FIND_CHANNEL\r\n");
    		debug("\r\n---- Please Use WeChat to SmartConfig ------\r\n\r\n");
    	break;
        case SC_STATUS_GETTING_SSID_PSWD: 			// ���ڻ�ȡ��SSID����PSWD����8266����ץȡ�����ܡ�SSID+PSWD����
            debug("\r\nSC_STATUS_GETTING_SSID_PSWD\r\n");
			sc_type *type = pdata;					// ��ȡ��SmartConfig���͡�ָ��

            if (*type == SC_TYPE_ESPTOUCH)			// ������ʽ == ��AIRKISS��||��ESPTOUCH_AIRKISS��
            	debug("\r\nSC_TYPE:SC_TYPE_ESPTOUCH\r\n");
            else
            	debug("\r\nSC_TYPE:SC_TYPE_AIRKISS\r\n");
	    break;
        case SC_STATUS_LINK:						// �ɹ���ȡ����SSID����PSWD��������STA������������WIFI
            debug("\r\nSC_STATUS_LINK\r\n");
            struct station_config *sta_conf = pdata;	// ��ȡ��STA������ָ��
			ESP8266_STA_Save2Flash(sta_conf ,Sector_STA_INFO); // ����SSID����PASS�����浽���ⲿFlash����

			wifi_station_set_config(sta_conf);			// ����STA������Flash��
	        wifi_station_disconnect();					// �Ͽ�STA����
	        wifi_station_connect();						// ESP8266����WIFI
	    break;
        case SC_STATUS_LINK_OVER: 						// ESP8266��ΪSTA���ɹ����ӵ�WIFI
            debug("\r\nSC_STATUS_LINK_OVER\r\n");
            smartconfig_stop();		// ֹͣSmartConfig���ͷ��ڴ�
    		if(wifi_station_get_connect_status() == STATION_GOT_IP )// ��ʾESP8266��IP��ַ
    		{
    			if(ESP8266_Save_LocalIP(&ST_NetCon,STA_MOD) == true)
    			{
    				debug("ESP8266_IP = %d.%d.%d.%d\n",ST_NetCon.proto.tcp->local_ip[0],ST_NetCon.proto.tcp->local_ip[1],ST_NetCon.proto.tcp->local_ip[2],ST_NetCon.proto.tcp->local_ip[3]);
    			}
    		}
			debug("\r\n---- ESP8266 Connect to WIFI Successfully ----\r\n");

			//*****************************************************
			// WIFI���ӳɹ���ִ�к������ܡ�	�磺SNTP/UDP/TCP/DNS��
			//*****************************************************
	    break;
    }
}

/***
 *
 */
void ICACHE_FLASH_ATTR
OS_Timer_CB(void)
{
	static u8 flag_sw = 0;
	struct ip_info infoIP;
	static u8 flag_time = 0;
	uint8 S_WIFI_STA_Connect = wifi_station_get_connect_status();
	if(flag_sw==0)
	{
		debug(".");
		if(S_WIFI_STA_Connect == STATION_GOT_IP )
		{
			if(ESP8266_Save_LocalIP(&ST_NetCon,STA_MOD) == true)
			{
				debug("ESP8266_IP = %d.%d.%d.%d\n",ST_NetCon.proto.tcp->local_ip[0],ST_NetCon.proto.tcp->local_ip[1],ST_NetCon.proto.tcp->local_ip[2],ST_NetCon.proto.tcp->local_ip[3]);
			}

		  //  ESP8266_DNS_GetIP(&ST_NetCon,WWW_IP_ADDR,DNS_Over_Cb_JX);//����DNS��ȡ��ַ
			debug("--> �ɹ����ӵ�WIFI\n");
			ESP8266_SNTP_Init();
			flag_sw = 1;

		}
		else if(S_WIFI_STA_Connect==STATION_NO_AP_FOUND 	||		// δ�ҵ�ָ��WIFI
				S_WIFI_STA_Connect==STATION_WRONG_PASSWORD 	||		// WIFI�������
				S_WIFI_STA_Connect==STATION_CONNECT_FAIL		)	// ����WIFIʧ��
			{
				debug("\r\n---- S_WIFI_STA_Connect=%d-----------\r\n",S_WIFI_STA_Connect);
				debug("\r\n---- ESP8266 Can't Connect to WIFI-----------\r\n");

				// ΢��������������
				//��������������������������������������������������������������������������������������������
				//wifi_set_opmode(STATION_MODE);			// ��ΪSTAģʽ						//���ڢٲ���

				smartconfig_set_type(SC_TYPE_AIRKISS); 	// ESP8266������ʽ��AIRKISS��			//���ڢڲ���

				smartconfig_start(smartconfig_done);	// ���롾��������ģʽ��,�����ûص�����	//���ڢ۲���
				flag_sw = 1;
			}

	}
//	if(flag_time == 0 && flag_sw == 1)
//	{
//		if(Get_SNTPTime()!=0)
//		{
//			flag_time = 1;
//			debug(" ��ǰʱ��: %s \n",Get_SNTPTime());
//			//debug("-------------- ����TCP-Server -------------\n");
//			//ESP8266_STA_TCPClient_NetCon_ByStr(&ST_NetCon,"192.168.31.67:6666");
//		}
//
//	}

}

