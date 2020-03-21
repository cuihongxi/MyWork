/**
 * 2019年10月22日09:35:37
 * 关于myMQTT应用层的函数
 * 使用方法：
 * 	myMQTT_SessionStrDefaultInit(。。。)函数初始化会话SessionStr* ss。
 * 	但是该函数一般不会被用户直接使用，通常是由阿里云或者其他的云初始化时引用。
 * 	该文件定义了控制报文的所有函数和相关回调函数，并注册进会话结构体ss中
 * 	将各自的回调函数放入相应的网络接收位置以便产生相应功能。
 */

#ifndef		__myMQTT_APP_H
#define		__myMQTT_APP_H
#include "uhead.h"
#include "myMQTT.h"




// 创建一个新会话
void myMQTT_SessionStrDefaultInit(SessionStr* ss,char* url,u16 port, char* name, char* password, char* clientId);


#endif

