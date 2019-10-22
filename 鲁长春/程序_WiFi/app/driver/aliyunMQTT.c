#include "aliyunMQTT.h"
#include "hmacsha1.h"
#include "mystring.h"
#include "myMQTT_APP.h"

/*****************************************阿里云******************************************************/
// 创建阿里ClientId
char*  CreatAliyunClientId(char* clientId)
{
	char* buff = (char*)malloc(strlen("|securemode=3,signmethod=hmacsha1|") + strlen(clientId) + 1);
	Str_Insert(buff,"|securemode=3,signmethod=hmacsha1|",0);
	Str_Insert(buff,clientId,0);
	return buff;
}

// 生成阿里用户名: ${YourDeviceName}&${YourPrductKey}
char*  CreatAliyunUserName(char* deviceName,char* productKey)
{
	char* buff = (char*)malloc(strlen("&") + strlen(deviceName) + strlen(productKey) + 1);
	Str_Insert(buff,"&",0);
	Str_Insert(buff,deviceName,0);
	Str_Insert(buff,productKey,strlen(buff));
	return buff;
}

// 生成密码 用DeviceSecret做为密钥对clientId*deviceName*productKey#进行hmacsha1加密后的结果
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
	as->ss.passWord = malloc(len_password*2 + 1);			// 需要free
	HmacSHA1((u8*)as->deviceSecret,strlen(as->deviceSecret),(u8*)buff,strlen(buff),(u8*)as->ss.passWord,&len_password);
	free(buff);
	return as->ss.passWord;
}
//创建阿里云的MQTT会话
AliyunStr*  ConnectAliyunMqtt(	char* url,u16 port,char* productKey,char* deviceName,char* deviceSecret,char* clientId,char* timestamp)
{
	AliyunStr* as = malloc(sizeof(AliyunStr));						// 需要free
	as->deviceName  = deviceName;									// 设备名
	as->deviceSecret = deviceSecret;								// 密钥
	as->productKey = productKey;									//
	as->clientId = clientId;										// ${clientId}为设备的ID信息。可取任意值，长度在64字符以内。建议使用设备的MAC地址或SN码。
//	as->ss.usrName = CreatAliyunUserName(as->deviceName,as->productKey);	// 需要free
//	as->ss.passWord = CreatAliyunPassWord(as);								// 需要free
//	as->ss.clientId = CreatAliyunClientId(as->clientId);					// 需要free

	myMQTT_SessionStrInit(&(as->ss), url,port,CreatAliyunUserName(as->deviceName,as->productKey),\
			CreatAliyunPassWord(as), CreatAliyunClientId(as->clientId));
	return as;
}


