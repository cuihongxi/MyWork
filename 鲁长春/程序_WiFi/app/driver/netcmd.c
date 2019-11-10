#include "netcmd.h"
#include "myWifi.h"
#include "AppCallBack.h"
#include "myHTTP.h"
#include "myMQTT.h"
#include "mystring.h"
#include "myMQTT_APP.h"
extern SessionStr* ss;

// ��������ִ�в�ͬ����������
void  RunNetCmd(char* str)
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

		myHTTP_GET(str);								// HTTP GET�����ȡ��Դ
	}

	if(CampareCMD(str,MQTT_CONNET)){		// MQTT CONNECT����

		ss->Connect(ss);
	}

	if(CampareCMD(str,MQTT_DISCON)){		// MQTT �Ͽ�����

		ss->DisConnect();// �Ͽ�����
	}


}




