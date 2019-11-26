/**
 * 2019年10月16日09:59:59
 * 阿里云关于MQTT
 */


#ifndef	__aliyunMQTT_h
#define	__aliyunMQTT_h
#include "uhead.h"
#include "myMQTT.h"

#define		PASSWORD_MIN_LEN		20	//定义密码长度



// 阿里云结构体
typedef struct _AliyunStr{
	SessionStr 		ss;					// 会话结构体
	char* 			productKey;			//
	char*			deviceName;
	char*			deviceSecret;		// 密钥
	char* 			clientId;			// 为设备的ID信息。可取任意值，长度在64字符以内。建议使用设备的MAC地址或SN码。


}AliyunStr;


//AliyunStr* ConnectAliyunMqtt(char* url,u16 port,char* productKey,char* deviceName,\
//		char* deviceSecret,char* clientId,char* timestamp);

AliyunStr*  ConnectAliyunMqtt(char* url,u16 port,AliyunStr* as);	// 创建阿里云的MQTT会话

#endif

