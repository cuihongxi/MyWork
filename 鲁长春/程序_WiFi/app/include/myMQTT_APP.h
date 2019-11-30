/**
 * 2019年10月22日09:35:37
 * 关于myMQTT应用层的函数
 */

#ifndef		__myMQTT_APP_H
#define		__myMQTT_APP_H
#include "uhead.h"
#include "myMQTT.h"




// 创建一个新会话
void myMQTT_SessionStrDefaultInit(SessionStr* ss,char* url,u16 port, char* name, char* password, char* clientId);


#endif

