#include "myHTTP.h"
#include "myWifi.h"
#include "mystring.h"

void ICACHE_FLASH_ATTR  myHTTP_GET(char* url)
{
	char* str = "GET http:// HTTP/1.1\nHost:\nConnection:close\n\n";
	char hoststr[30] = {0};	// 获取host 主机域名
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
