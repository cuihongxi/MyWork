#include "netcmd.h"
#include "myWifi.h"
#include "AppCallBack.h"
#include "myHTTP.h"
#include "myMQTT.h"
#include "mystring.h"
#include "myMQTT_APP.h"
extern SessionStr* ss;

// 根据命令执行不同的网络命令
void  RunNetCmd(char* str)
{
	if(CampareCMD(str,TCP_CONNECT)) 		// 连接到TCP_server命令
	{
		ESP8266_STA_TCPClient_NetCon_ByStr(&ST_NetCon,str);
	}
	if(CampareCMD(str,MESSAGE)) 			// 发送消息
	{
		ESP8266_SendMessage_B(&ST_NetCon,str);
	}
	if(CampareCMD(str,DIS_CONNECT)){		// 断开端口连接
		espconn_disconnect(&ST_NetCon);
	}
	if(CampareCMD(str,NET_CONNECT)){		// 通过域名访问网址
		ST_NetCon.proto.tcp->remote_port = GetNetPort(str);// 获取端口号
		GetNetIPAddr(str);								// 截取网址
		ESP8266_DNS_GetIP(&ST_NetCon,str,DNS_Over_Cb_JX);//解析DNS获取地址
	}
	if(CampareCMD(str,HTTP_GET)){		// HTTP get 请求

		myHTTP_GET(str);								// HTTP GET命令获取资源
	}

	if(CampareCMD(str,MQTT_CONNET)){		// MQTT CONNECT报文

		ss->Connect(ss);
	}

	if(CampareCMD(str,MQTT_DISCON)){		// MQTT 断开连接

		ss->DisConnect();// 断开连接
	}


}




