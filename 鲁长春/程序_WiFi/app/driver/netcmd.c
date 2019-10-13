#include "netcmd.h"
#include "myWifi.h"
#include "AppCallBack.h"
//�ַ���ȥ���ո�ͻس��������µ��ַ���
void Str_DelSpace(char* str)
{
	char dat[20] = {0};
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

//�ײ�ƥ���ַ���,���ÿո����ƥ����ͬ���ַ�
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


// ��ȡ�˿ں�,Ҫ��֤���ϸ�� ip:port ��ʽ���ַ���
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
// ȥ���˿ںŽ��ַ��������ַ,Ҫ��֤���ϸ�� ip:port ��ʽ���ַ���
void GetNetIPAddr(char* str)
{
	while(*str != ':') str ++;
	*str = 0;
}
// ��������ִ�в�ͬ����������
void RunNetCmd(char* str)
{
	if(CampareStrInHeadWithSpace(str,TCP_CONNECT)) 		// ���ӵ�TCP_server����
	{
		ESP8266_STA_TCPClient_NetCon_ByStr(&ST_NetCon,str);
	}
	if(CampareStrInHeadWithSpace(str,MESSAGE)) 			// ������Ϣ
	{
		ESP8266_SendMessage_B(&ST_NetCon,str);
	}
	if(CampareStrInHeadWithSpace(str,DIS_CONNECT)){		// �Ͽ��˿�����
		espconn_disconnect(&ST_NetCon);
	}
	if(CampareStrInHeadWithSpace(str,NET_CONNECT)){		// ͨ������������ַ
		Str_DelSpace(str);								// ȥ���ո�ͻس�
		ST_NetCon.proto.tcp->remote_port = GetNetPort(str);// ��ȡ�˿ں�
		GetNetIPAddr(str);								// ��ȡ��ַ

		ESP8266_DNS_GetIP(&ST_NetCon,str,DNS_Over_Cb_JX);//����DNS��ȡ��ַ
	}
	debug("wifi str:	%s\n",str);
}




