#include "netcmd.h"
#include "myWifi.h"
#include "AppCallBack.h"
#include "myHTTP.h"
#include "myMQTT.h"
#include "mystring.h"
#include "myMQTT_APP.h"
extern SessionStr* ss;

// ��������ִ�в�ͬ����������
void ICACHE_FLASH_ATTR RunNetCmd(char* str)
{
	if(CampareCMD(str,TCP_CONNECT)) 		// ���ӵ�TCP_server����
	{
		ESP8266_STA_TCPClient_NetCon_ByStr(&ST_NetCon,str);
	}
	if(CampareCMD(str,MESSAGE)) 			// ������Ϣ
	{
		ESP8266_SendMessage_B(&ST_NetCon,str);
	}
	if(CampareCMD(str,DIS_CONNECT)){		// �Ͽ��˿�����
		espconn_disconnect(&ST_NetCon);
	}
	if(CampareCMD(str,NET_CONNECT)){		// ͨ������������ַ
		ST_NetCon.proto.tcp->remote_port = GetNetPort(str);// ��ȡ�˿ں�
		GetNetIPAddr(str);								// ��ȡ��ַ
		ESP8266_DNS_GetIP(&ST_NetCon,str,DNS_Over_Cb_JX);//����DNS��ȡ��ַ
	}
	if(CampareCMD(str,HTTP_GET)){		// HTTP get ����

		myHTTP_GET(str);				// HTTP GET�����ȡ��Դ
	}

	if(CampareCMD(str,MQTT_CONNET)){		// MQTT CONNECT����

		debug("espconn->state = %d\r\n",ss->espconn->state);
		if(ss->espconn->state != ESPCONN_CONNECT)
		{
			ss->espconn->proto.tcp->remote_port = ss->port;// ��ȡ�˿ں�
			ss->messageType = CONNECT;
			ESP8266_DNS_GetIP(ss->espconn,ss->url,DNS_Over_Cb_JX);//����DNS��ȡ��ַ
		}else ss->Connect(ss);

	}

	if(CampareCMD(str,MQTT_DISCON)){		// MQTT �Ͽ�����

		ss->DisConnect(ss);// �Ͽ�����
	}

	if(CampareCMD(str,MQTT_PING)){		// MQTT PING

		ss->KeepAlive(ss);				// PING
	}
	if(CampareCMD(str,MQTT_ADDSUB)){		// MQTT SUB

		ss->AddSubscribe(ss,"/sys/a1nVPohfr2X/LED0/thing/service/property/set",0);
		//ss->AddSubscribe(ss,"adfad",0);
	}
	if(CampareCMD(str,MQTT_SUB)){		// MQTT SUB

		ss->Subscribe(ss);
	}
}




