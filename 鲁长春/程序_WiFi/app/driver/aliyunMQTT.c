#include "aliyunMQTT.h"
#include "hmacsha1.h"
#include "mystring.h"
#include "myMQTT_APP.h"

/*****************************************������******************************************************/
// ��������ClientId
char*  CreatAliyunClientId(char* clientId)
{
	char* buff = (char*)malloc(strlen("|securemode=3,signmethod=hmacsha1|") + strlen(clientId) + 1);
	Str_Insert(buff,"|securemode=3,signmethod=hmacsha1|",0);
	Str_Insert(buff,clientId,0);
	return buff;
}

// ���ɰ����û���: ${YourDeviceName}&${YourPrductKey}
char*  CreatAliyunUserName(char* deviceName,char* productKey)
{
	char* buff = (char*)malloc(strlen("&") + strlen(deviceName) + strlen(productKey) + 1);
	Str_Insert(buff,"&",0);
	Str_Insert(buff,deviceName,0);
	Str_Insert(buff,productKey,strlen(buff));
	return buff;
}

// �������� ��DeviceSecret��Ϊ��Կ��clientId*deviceName*productKey#����hmacsha1���ܺ�Ľ��
char*  CreatAliyunPassWord(AliyunStr* as)
{
	char* buff = malloc(1 + strlen(as->clientId) + strlen(as->deviceName) + strlen(as->productKey) + strlen("clientIddeviceNameproductKey"));
	Str_Insert(buff,as->productKey,0);
	Str_Insert(buff,"productKey",0);
	Str_Insert(buff,as->deviceName,0);
	Str_Insert(buff,"deviceName",0);
	Str_Insert(buff,as->clientId,0);
	Str_Insert(buff,"clientId",0);
	int len_password = PASSWORD_MIN_LEN;
	as->ss.passWord = malloc(len_password*2 + 1);			// ��Ҫfree
	HmacSHA1((u8*)as->deviceSecret,strlen(as->deviceSecret),(u8*)buff,strlen(buff),(u8*)as->ss.passWord,&len_password);
	free(buff);
	return as->ss.passWord;
}
//���������Ƶ�MQTT�Ự
AliyunStr*  ConnectAliyunMqtt(	char* url,u16 port,char* productKey,char* deviceName,char* deviceSecret,char* clientId,char* timestamp)
{
	AliyunStr* as = malloc(sizeof(AliyunStr));						// ��Ҫfree
	as->deviceName  = deviceName;									// �豸��
	as->deviceSecret = deviceSecret;								// ��Կ
	as->productKey = productKey;									//
	as->clientId = clientId;										// ${clientId}Ϊ�豸��ID��Ϣ����ȡ����ֵ��������64�ַ����ڡ�����ʹ���豸��MAC��ַ��SN�롣
//	as->ss.usrName = CreatAliyunUserName(as->deviceName,as->productKey);	// ��Ҫfree
//	as->ss.passWord = CreatAliyunPassWord(as);								// ��Ҫfree
//	as->ss.clientId = CreatAliyunClientId(as->clientId);					// ��Ҫfree

	myMQTT_SessionStrInit(&(as->ss), url,port,CreatAliyunUserName(as->deviceName,as->productKey),\
			CreatAliyunPassWord(as), CreatAliyunClientId(as->clientId));
	return as;
}


