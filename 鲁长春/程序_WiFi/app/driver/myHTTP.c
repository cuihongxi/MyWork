#include "myHTTP.h"
#include "myWifi.h"

// ��sor�ַ�����position���ַ��������str
// ����ֻ��256���ֽ�
void Str_Insert(char* sor,char* str,u32 position)
{

	char back[256] = {0};
	u8 i = 0;
	for(i=0;sor[position+i] != 0;i++) back[i] = sor[position+i];	// �������������Ϣ
	for(i = 0;str[i]!=0;i++)sor[position+i] = str[i];				// ����
	u8 j = i;
	for(;back[i-j] != 0;i++)sor[position+i] = back[i-j];

}

void myHTTP_GET(char* url)
{
	char* str = "GET http:// HTTP/1.1\nHost:\nConnection:close\n\n";
	char hoststr[30] = {0};	// ��ȡhost ��������
	char* p = url;
//	while(*p != 'w') p++;
	u8 j = 0;
	while(p[j] != '/')
	{
		hoststr[j] = p[j];
		j++;
	}
	hoststr[j] = 0;
	char* getstr = malloc(strlen(url) + strlen(str) + strlen(hoststr) + 1);
	Str_Insert(getstr,str,0);
	Str_Insert(getstr,url,strlen("GET http://"));
	Str_Insert(getstr,hoststr,strlen("GET http://") + strlen(url) + strlen(" HTTP/1.1\nHost:"));

	ESP8266_SendMessage_B(&ST_NetCon,getstr);
	free(getstr);
}
