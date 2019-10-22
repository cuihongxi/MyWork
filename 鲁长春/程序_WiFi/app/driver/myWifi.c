#include "myWifi.h"
#include "myflash.h"

struct espconn ST_NetCon = {0};
/***
 * 设置AP模式参数
 */
void ICACHE_FLASH_ATTR 
ESP8266_AP_Init(struct espconn* ST_NetCon,const char* ssid,const char* password)
{
	struct softap_config AP_Config = {0};			// AP参数结构体

	wifi_set_opmode(SOFTAP_MODE);					// 设置AP模式并且保存到Flash

	os_strcpy(AP_Config.ssid,ssid);					// 设置SSID，WIFI名
	os_strcpy(AP_Config.password,password);			// WiFi密码
	AP_Config.ssid_len=os_strlen(ssid);				// 长度 ,允许1~13
	AP_Config.channel=1;                      		// 通道
	AP_Config.authmode=AUTH_WPA2_PSK;           	// 加密方式
	AP_Config.ssid_hidden=0;                  		// 是否隐藏WiFi名
	AP_Config.max_connection=4;               		// 最大连接个数
	AP_Config.beacon_interval=100;            		// 信标间隔时槽，100~60000ms
	ST_NetCon->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));	//

	wifi_softap_set_config(&AP_Config);				// 设置AP参数
}

/***
 * 字符串型IP转化成INT型
 *
 */
IP_Struct ICACHE_FLASH_ATTR
ESP8266_IPStr2Num(const char* ip)
{
	IP_Struct ipstr = {0};
	IP_Struct iperro = {0};
	u8 i = 0;
	u8 dat = 0;
	u8 j = 0;
	for(j=0;j<4;j++)
	{
		while(ip[i] == ' ' || ip[i] == '.') i ++;
		while(ip[i] != '.' && ip[i] != ' ' && ip[i] != ':')
		{
			if(ip[i]<'0' || ip[i] > '9') return iperro;
			dat = dat*10 + (ip[i] - '0');
			i ++;
			if(ip[i] == 0) return iperro;
		}
		ipstr.ip = ((u32)dat << 8*j)|ipstr.ip;
		dat = 0;
	}
	while(ip[i] != ':')
	{
		i ++;
		if(ip[i] == 0) return ipstr;
	}
	i++;
	while(ip[i] == ' ') i ++;
	while(ip[i]>='0' && ip[i] <= '9' && ip[i] != 0)
	{
		ipstr.port = ipstr.port*10 + (ip[i] - '0');
		i ++;

	}
	while(ip[i] == ' '|| ip[i] == '\r'|| ip[i] == '\n') i ++;
	if(ip[i] != 0) return iperro;
	return ipstr;
}

/***
 * 以字符串形式，设置静态IP
 * modeIF ： STATION_IF  SOFTAP_IF
 */
bool ICACHE_FLASH_ATTR
ESP8266SetStateIP_Str(u8 modeIF,const char* ipstr)
{
	bool res = false;
	struct ip_info ST_ESP8266_IP ;
	IP_Struct IPnum = ESP8266_IPStr2Num(ipstr);
	u8* p = (u8*)&(IPnum.ip);
	if(modeIF == STATION_IF) wifi_station_dhcpc_stop();
	else wifi_softap_dhcps_stop();
	ST_ESP8266_IP.ip.addr = IPnum.ip;
	IP4_ADDR(&ST_ESP8266_IP.gw, p[0], p[1], p[2], 1);
	IP4_ADDR(&ST_ESP8266_IP.netmask, 255, 255, 255, 0);
	wifi_set_ip_info(modeIF, &ST_ESP8266_IP);
	if(modeIF == STATION_IF) res = wifi_station_dhcpc_start();
	else res = wifi_softap_dhcps_start();
	return res;
}
/***
 * 以INT型数字设置静态IP
 * modeIF ： STATION_IF  SOFTAP_IF
 */
bool ICACHE_FLASH_ATTR
ESP8266SetStateIP_Num(u8 modeIF,u32 ipNum)
{
	bool res = false;
	struct ip_info ST_ESP8266_IP ;
	u8* p = (u8*)&(ipNum);
	if(modeIF == STATION_IF) wifi_station_dhcpc_stop();
	else wifi_softap_dhcps_stop();
	ST_ESP8266_IP.ip.addr = ipNum;
	IP4_ADDR(&ST_ESP8266_IP.gw, p[0], p[1], p[2], 1);
	IP4_ADDR(&ST_ESP8266_IP.netmask, 255, 255, 255, 0);
	wifi_set_ip_info(modeIF, &ST_ESP8266_IP);
	if(modeIF == STATION_IF) res = wifi_station_dhcpc_start();
	else res = wifi_softap_dhcps_start();
	return res;
}

/***
 * AP模式设置网络连接
 * conType : ESPCONN_TCP  PCONN_UDP
 * ipstr   "255.255.255.255:port"
 */
void ICACHE_FLASH_ATTR
ESP8266_AP_NetCon_Init(struct espconn* ST_NetCon,enum espconn_type conType,const char* ipstr)
{
	struct ip_info ST_ESP8266_IP ;
	IP_Struct IPnum = ESP8266_IPStr2Num(ipstr);
	u8* p = (u8*)&(IPnum.ip);
	ST_NetCon->type = conType;
	*ESP8266_REMOTE_IP(ST_NetCon)  = IPnum.ip;
	ST_NetCon->proto.tcp->local_port  = IPnum.port ;

	ESP8266SetStateIP_Num(SOFTAP_IF,IPnum.ip);

	//----------------------------------------------
	if(ST_NetCon->type == ESPCONN_UDP)
		espconn_create(ST_NetCon);
	else if(ST_NetCon->type == ESPCONN_TCP)
		espconn_accept(ST_NetCon);
}

/***
 * 设置网络连接的回调函数
 */
void ICACHE_FLASH_ATTR
ESP8266_Regitst_Fun_Init(struct espconn* ST_NetCon,RegCBStr* cbfun)
{
	/**
	 * UDP
	 */
	JUGE_CB_NULL(espconn_regist_sentcb,ST_NetCon,cbfun->sent_callback);	//
	JUGE_CB_NULL(espconn_regist_recvcb,ST_NetCon,cbfun->recv_callback);	//
	/*
	 * TCP
	 */
	JUGE_CB_NULL(espconn_regist_connectcb,ST_NetCon,cbfun->connect_callback);	//TCP
	JUGE_CB_NULL(espconn_regist_disconcb,ST_NetCon,cbfun->disconnect_callback); //TCP
	JUGE_CB_NULL(espconn_regist_reconcb,ST_NetCon,cbfun->reconnect_callback);	//TCP
	JUGE_CB_NULL(espconn_regist_write_finish,ST_NetCon,cbfun->write_finish_fn);

}

//
void ICACHE_FLASH_ATTR
ESP8266_Get_ConInfo(struct espconn *arg,struct espconn *ST_NetCon)
{
	remot_info * P_port_info = NULL;
	if(espconn_get_connection_info(arg, &P_port_info, 0)==ESPCONN_OK)	// 从arg中获得连接信息
	{
		arg->proto.udp->remote_port = P_port_info->remote_port;
		os_memcpy(arg->proto.udp->remote_ip,P_port_info->remote_ip,4);
		ST_NetCon->proto.udp->remote_port = P_port_info->remote_port;
		os_memcpy(ST_NetCon->proto.udp->remote_ip,P_port_info->remote_ip,4);
	}
}

//wifi回复远端IP消息
void ICACHE_FLASH_ATTR
ESP8266_SendMessage(struct espconn *arg,u32 ipNum,u16 port,const char* str)
{
	arg->proto.udp->remote_port = port;
	*ESP8266_REMOTE_IP(arg) = ipNum;
	espconn_send(arg,(uint8*)str,os_strlen(str));	//

}

//已建立连接后wifi发送消息
void ICACHE_FLASH_ATTR
ESP8266_SendMessage_B(struct espconn *arg,const char* str)
{
	espconn_send(arg,(uint8*)str,os_strlen(str));	//

}

// wifi回复远端IP消息
void ICACHE_FLASH_ATTR
ESP8266_SendData(struct espconn *arg,u32 ipNum,u16 port,uint8* dat,u32 len)
{
	arg->proto.udp->remote_port = port;
	*ESP8266_REMOTE_IP(arg) = ipNum;
	espconn_send(arg,dat,len);	//

}


//
sint8 ICACHE_FLASH_ATTR
ESP8266_ConTCPServer(struct espconn *arg,const char* ipstr)
{
	IP_Struct IPnum = ESP8266_IPStr2Num(ipstr);
	*ESP8266_REMOTE_IP(arg)  = IPnum.ip;
	arg->proto.tcp->remote_port  = IPnum.port ;
	return espconn_connect(arg);

}

/*************************STA*************************************************/

/***
 * sta模式
 */
void ICACHE_FLASH_ATTR
ESP8266_STA_Init(struct espconn* ST_NetCon,const char* ssid,const char* password)
{
	struct station_config STA_Config = {0} ;
	wifi_set_opmode(STATION_MODE);				//
	os_strcpy(STA_Config.ssid,ssid);			//
	os_strcpy(STA_Config.password,password);	//
	wifi_station_set_config(&STA_Config);		//
	ST_NetCon->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));	//

	//----------------------------------------------------------------------------------
	// wifi_station_connect();		//
}
/***
 * sta模式，从FLASH中读数据
 */
void ICACHE_FLASH_ATTR
ESP8266_STA_Init_FromFlash(struct espconn* ST_NetCon,u32 flashsector)
{
	struct station_config STA_Config = {0} ;
	//spi_flash_read(flashsector*4096,(uint32 *)&STA_Config, 96);	// (SSID/PASS)
	Flash_Read(flashsector, (u8*)&STA_Config,96);
	STA_Config.ssid[31] = 0;		// SSID'\0'
	STA_Config.password[63] = 0;	// APSS'\0'
	wifi_set_opmode(STATION_MODE);				//
	wifi_station_set_config(&STA_Config);		//
	ST_NetCon->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));	//

	//
	//----------------------------------------------------------------------------------
	// wifi_station_connect();
}

/***
 *
 *
 */
void ICACHE_FLASH_ATTR
ESP8266_STA_Save2Flash(struct station_config *sta_conf ,u32 flashsector)
{
	spi_flash_erase_sector(flashsector);						//
	spi_flash_write(flashsector*4096, (uint32 *)sta_conf, 96);	//
}

/***
 * STA
 * conType : ESPCONN_TCP SPCONN_UDP
 *
 */
void ICACHE_FLASH_ATTR
ESP8266_STA_Server_NetCon_Init(struct espconn* ST_NetCon,enum espconn_type conType,u16 port)
{
	struct ip_info ST_ESP8266_IP ;

	ST_NetCon->type = conType;		//
	ST_NetCon->proto.tcp->local_port  = port ;						//

	if(conType == ESPCONN_UDP)
		espconn_create(ST_NetCon);
	else if(conType == ESPCONN_TCP)
		espconn_accept(ST_NetCon);
}
/***
 *(STA-TCP_Client)
 *
 */
void ICACHE_FLASH_ATTR
ESP8266_STA_TCPClient_NetCon_ByStr(struct espconn* ST_NetCon,const char* ipstr)
{
	struct ip_info ST_ESP8266_IP ;
	IP_Struct IPnum = ESP8266_IPStr2Num(ipstr);
	u8* p = (u8*)&(IPnum.ip);
	ST_NetCon->type = ESPCONN_TCP;
	*(u32*)&(ST_NetCon->proto.tcp->remote_ip[0])  = IPnum.ip;
	ST_NetCon->proto.tcp->remote_port  = IPnum.port ;
	ST_NetCon->proto.tcp->local_port  = espconn_port() ;
	espconn_connect(ST_NetCon);									//

}

/***
 * (STA-TCP_Client)
 *
 */
void ICACHE_FLASH_ATTR
ESP8266_STA_TCPClient_NetCon_ByInt(struct espconn* ST_NetCon,u32 ipaddr,u16 port)
{
	struct ip_info ST_ESP8266_IP ;
	ST_NetCon->type = ESPCONN_TCP;
	*(u32*)&(ST_NetCon->proto.tcp->remote_ip[0])  = ipaddr;
	ST_NetCon->proto.tcp->remote_port  = port ;
	ST_NetCon->proto.tcp->local_port  = espconn_port();
	espconn_connect(ST_NetCon);	//
}


/**
 * DNS
 *
 */
err_t ICACHE_FLASH_ATTR
ESP8266_DNS_GetIP(struct espconn* ST_NetCon,const char* dns,dns_found_callback cb)
{
	return espconn_gethostbyname(ST_NetCon, dns, (ip_addr_t *)(&(ST_NetCon->proto.tcp->remote_ip[0])), cb);
}

/**
 * _NetCon.proto.tcp->local_ip
 *
 * modIF :TATION_IF SOFTAP_IF
 */
bool ICACHE_FLASH_ATTR
ESP8266_Save_LocalIP(struct espconn* ST_NetCon,enum IFMOD modIF)
{
	struct ip_info ipInfo;
	bool res = false;
	if(wifi_get_ip_info((u8)modIF,&ipInfo) == true)
	{
		res = true;
		*ESP8266_LOCAL_IP(ST_NetCon) = ipInfo.ip.addr;//
	}
	return res;
}
// 返回远端IP地址
u32* ICACHE_FLASH_ATTR
ESP8266_REMOTE_IP(struct espconn * arg)
{
	return (u32*)&(arg->proto.tcp->remote_ip[0]);
}

// 返回本地IP
u32* ICACHE_FLASH_ATTR
ESP8266_LOCAL_IP(struct espconn *arg)
{
	return (u32*)&(arg->proto.tcp->local_ip[0]);
}

//HTTP GET
//---------

void  ESP8266_Http_Get(struct espconn *ST_NetCon,const char* mes)
{

//	ESP8266_SendMessage(ST_NetCon,*ESP8266_REMOTE_IP(&ST_NetCon),80,mes);//
}

void ESP8266_SendtoService(uint8 *psent, uint16 length)
{
	espconn_send(&ST_NetCon,psent,length);
}
