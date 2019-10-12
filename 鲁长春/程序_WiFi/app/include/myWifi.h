/**
 *
 *
 */

#ifndef __myWifi_H
#define __myWifi_H

#include "uhead.h"
#include "espconn.h"
#include "mem.h"
extern	struct espconn ST_NetCon;

//
typedef struct{
	u32 ip;
	u16 port;

}IP_Struct;

enum IFMOD{
	 STA_MOD=0,
	 AP_MOD,
};
//
typedef struct{
    espconn_recv_callback 		recv_callback;
    espconn_sent_callback 		sent_callback;
    espconn_connect_callback 	connect_callback;
    espconn_reconnect_callback 	reconnect_callback;
    espconn_connect_callback 	disconnect_callback;
	espconn_connect_callback 	write_finish_fn;

}RegCBStr;

//
#define JUGE_CB_NULL(fun,con,cb)	do{\
if((cb) != 0){fun(con,cb);};\
}while(0)



/***
 * ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ã²ï¿½ï¿½è£º
 * 	ESP8266_AP_Init
 * 	ESP8266_Regitst_Fun_Init
 * 	ESP8266_NetCon_Init
 * 	Esp8266_SendMessage
 */
void ESP8266_AP_Init(struct espconn* ST_NetCon,const char* ssid,const char* password);
void ESP8266_AP_NetCon_Init(struct espconn* ST_NetCon,enum espconn_type conType,const char* ipstr);
void ESP8266_Regitst_Fun_Init(struct espconn* ST_NetCon,RegCBStr* cbfun);
sint8 ESP8266_ConTCPServer(struct espconn *arg,const char* ipstr);
void ESP8266_SendMessage(struct espconn *arg,u32 ipNum,u16 port,const char* str);	// wifi·¢ËÍÒÔ×Ö·û´®Êı¾İ
void ESP8266_SendData(struct espconn *arg,u32 ipNum,u16 port,uint8* dat,u32 len);// wifi·¢ËÍÒÔ×Ö½ÚÊı¾İ

void ESP8266_Get_ConInfo(struct espconn *arg,struct espconn *ST_NetCon);	// ï¿½ï¿½È¡Ô¶ï¿½ï¿½ï¿½ï¿½Ï¢ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
IP_Struct ESP8266_IPStr2Num(const char* ip);			// ï¿½Ö·ï¿½ï¿½ï¿½×ªï¿½ï¿½ï¿½ï¿½IPï¿½ï¿½Ö·

void ESP8266_STA_Init(struct espconn* ST_NetCon,const char* ssid,const char* password);//Â·ï¿½ï¿½ï¿½ï¿½ï¿½Ëºï¿½ï¿½ï¿½ï¿½ï¿½
void ESP8266_STA_Init_FromFlash(struct espconn* ST_NetCon,u32 flashsector);//´ÓFLASHÖĞ¶ÁÈ¡±£´æµÄÂ·ÓÉÆ÷Ãû³ÆºÍÃÜÂë
void ESP8266_STA_Save2Flash(struct station_config *sta_conf ,u32 flashsector);//±£´æÂ·ÓÉÆ÷ĞÅÏ¢µ½FLASHÖĞ
void ESP8266_STA_Server_NetCon_Init(struct espconn* ST_NetCon,enum espconn_type conType,u16 port);//Ö»ï¿½Ğ»ï¿½È¡IPï¿½ï¿½Ö·ï¿½ï¿½ï¿½ï¿½ï¿½Èµï¿½ï¿½ï¿½ï¿½Ü½ï¿½ï¿½ï¿½ï¿½ï¿½Ò»ï¿½ï¿½
void ESP8266_STA_TCPClient_NetCon_ByStr(struct espconn* ST_NetCon,const char* ipstr);//STA-TCP_Clientï¿½ï¿½ï¿½Ö·ï¿½ï¿½ï¿½ï¿½ï¿½Ê½ï¿½ï¿½ï¿½ï¿½
void ESP8266_STA_TCPClient_NetCon_ByInt(struct espconn* ST_NetCon,u32 ipaddr,u16 port);//STA-TCP_Clientï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê½ï¿½ï¿½ï¿½ï¿½

err_t ESP8266_DNS_GetIP(struct espconn* ST_NetCon,const char* dns,dns_found_callback cb); // DNSæœåŠ¡å™¨è·å–IPåœ°å€
bool ESP8266_Save_LocalIP(struct espconn* ST_NetCon,enum IFMOD modIF);//è·å–è¿æ¥çš„IPä¿¡æ¯,è¿æ¥æˆåŠŸè¿”å›true

u32* ESP8266_REMOTE_IP(struct espconn * arg);	// Ô¶¶ËIP
u32* ESP8266_LOCAL_IP(struct espconn *arg);		// ±¾µØIP


#endif

