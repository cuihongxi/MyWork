#include "netcmd.h"
#include "myWifi.h"
#include "AppCallBack.h"
#include "myHTTP.h"
#include "myMQTT.h"
extern DataMessageStr*  ds;
//字符串去掉空格和回车，生成新的字符串
void Str_DelSpace(char* str)
{
	char dat[256] = {0};
	u8 i = 0;
	u8 j = 0;
	while(str[i] != 0)
	{
		while(str[i] == ' ' || str[i] == '\r'|| str[i] == '\n') i ++;
		dat[j] = str[i];
		i ++;
		j ++;
	}
	i = 0;
	while(dat[i])
	{
		str[i] = dat[i];
		i ++;
	}
	str[i] = 0;
}

//首部匹配字符串,并用空格替代匹配相同的字符
bool CampareStrInHeadWithSpace(char* str,char* cmd)
{
	 while(*cmd)
	 {
		 if( (*cmd)^(*str)) return false;
		 else *str = ' ';
		 cmd ++;
		 str ++;
	 }

	return true;
}


// 获取端口号,要保证是严格的 ip:port 格式的字符串
u16 GetNetPort(char* str)
{
	u16 port = 0;
	while(*str != ':')str ++;
	str ++;
	while(*str>='0' && *str <= '9' && *str != 0)
	{
		port = port*10 + (*str - '0');
		str ++;
	}
	return port;
}
// 去掉端口号将字符串变成网址,要保证是严格的 ip:port 格式的字符串
void GetNetIPAddr(char* str)
{
	while(*str != ':') str ++;
	*str = 0;
}
// 根据命令执行不同的网络命令
void RunNetCmd(char* str)
{
	if(CampareStrInHeadWithSpace(str,TCP_CONNECT)) 		// 连接到TCP_server命令
	{
		ESP8266_STA_TCPClient_NetCon_ByStr(&ST_NetCon,str);
	}
	if(CampareStrInHeadWithSpace(str,MESSAGE)) 			// 发送消息
	{
		ESP8266_SendMessage_B(&ST_NetCon,str);
	}
	if(CampareStrInHeadWithSpace(str,DIS_CONNECT)){		// 断开端口连接
		espconn_disconnect(&ST_NetCon);
	}
	if(CampareStrInHeadWithSpace(str,NET_CONNECT)){		// 通过域名访问网址
		Str_DelSpace(str);								// 去掉空格和回车
		ST_NetCon.proto.tcp->remote_port = GetNetPort(str);// 获取端口号
		GetNetIPAddr(str);								// 截取网址
		ESP8266_DNS_GetIP(&ST_NetCon,str,DNS_Over_Cb_JX);//解析DNS获取地址
	}
	if(CampareStrInHeadWithSpace(str,HTTP_GET)){		// HTTP get 请求
		Str_DelSpace(str);								// 去掉空格和回车
		myHTTP_GET(str);								// HTTP GET命令获取资源
	}

	if(CampareStrInHeadWithSpace(str,MQTT_CONNET)){		// MQTT CONNECT报文

		espconn_send(&ST_NetCon,ds->pdat,ds->length);
	}

	if(CampareStrInHeadWithSpace(str,MQTT_DISCON)){		// MQTT 断开连接

		myMQTT_Disconnect();	// 断开连接
	}


}




