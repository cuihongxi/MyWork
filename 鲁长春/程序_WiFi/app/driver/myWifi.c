#include "myWifi.h"

struct espconn ST_NetCon = {0};	// 娉ㄩ敓鏂ゆ嫹閿熸枻鎷烽敓璇畾閿熸枻鎷蜂负鍏ㄩ敓琛楁唻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鑺傛牳鏂ゆ嫹閿熸枻鎷蜂娇閿熺煫姝ゆ唻鎷烽敓鏂ゆ嫹(閿熻妭杈炬嫹)

/***
 * AP妯″紡閿熸枻鎷峰閿熸枻鎷�
 */
void ICACHE_FLASH_ATTR 
ESP8266_AP_Init(struct espconn* ST_NetCon,const char* ssid,const char* password)
{
	struct softap_config AP_Config = {0};				// AP閿熸枻鎷烽敓鏂ゆ嫹閿熺粨鏋勯敓鏂ゆ嫹

	wifi_set_opmode(SOFTAP_MODE);					// 閿熸枻鎷烽敓鏂ゆ嫹涓篈P妯″紡閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鑺ュ埌Flash

	// 閿熺粨鏋勯敓钘夎祴鍊�(娉ㄩ敓瑙ｏ細閿熸枻鎷烽敓鏂ゆ嫹閿熶粖闆嗘唻鎷疯瘑閿熸枻鎷�/閿熸枻鎷烽敓璇�戦敓鏂ゆ嫹閿熸枻鎷蜂负閿熻鍑ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹寮�)
	os_strcpy(AP_Config.ssid,ssid);					// 閿熸枻鎷烽敓鏂ゆ嫹SSID(閿熸枻鎷烽敓琛楀嚖鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鐙＄鎷穝sid閿熸枻鎷烽敓鏂ゆ嫹)
	os_strcpy(AP_Config.password,password);			// 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹(閿熸枻鎷烽敓琛楀嚖鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鐙＄鎷穚assword閿熸枻鎷烽敓鏂ゆ嫹)
	AP_Config.ssid_len=os_strlen(ssid);				// 閿熸枻鎷烽敓鏂ゆ嫹ssid閿熸枻鎷烽敓鏂ゆ嫹(閿熸枻鎷稴SID閿熶茎绛规嫹閿熸枻鎷蜂竴閿熸枻鎷�)
	AP_Config.channel=1;                      		// 閫氶敓鏂ゆ嫹閿熸枻鎷�1閿熸枻鎷�13
	AP_Config.authmode=AUTH_WPA2_PSK;           	// 閿熸枻鎷烽敓鐭》鎷烽敓鏂ゆ嫹妯″紡
	AP_Config.ssid_hidden=0;                  		// 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷稴SID
	AP_Config.max_connection=4;               		// 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
	AP_Config.beacon_interval=100;            		// 閿熻剼鎲嬫嫹閿熸枻鎷锋椂閿熸枻鎷�100閿熸枻鎷�60000 ms
	ST_NetCon->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));	// 閿熸枻鎷烽敓鏂ゆ嫹閿熻妭瀛橈紝涓洪敓鍓挎唻鎷疯瘉閿熻妭瀛樹笉閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓绲婥P閿熻妭杈炬嫹

	wifi_softap_set_config(&AP_Config);				// 閿熸枻鎷烽敓鏂ゆ嫹soft-AP閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鑺ュ埌Flash
}

/***
 * 閿熻鍑ゆ嫹閿熸枻鎷疯浆閿熸枻鎷烽敓鏂ゆ嫹IP閿熸枻鎷峰潃
 * 閿熸枻鎷烽敓鏂ゆ嫹 閿熸枻鎷烽敓鏂ゆ嫹 0
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
	while(ip[i] == ' ') i ++;
	if(ip[i] != 0) return iperro;
	return ipstr;
}

/***
 * 閿熸枻鎷烽敓琛楀嚖鎷烽敓鏂ゆ嫹閿熸枻鎷峰紡閿熸枻鎷烽敓鐭拝鎷锋�両P閿熸枻鎷峰潃
 * modeIF 鍙栧�� STATION_IF閿熸枻鎷稴OFTAP_IF
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
 * 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹寮忛敓鏂ゆ嫹閿熺煫鎾呮嫹鎬両P閿熸枻鎷峰潃
 * modeIF 鍙栧�� STATION_IF閿熸枻鎷稴OFTAP_IF
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
 * 閿熸枻鎷峰閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�(AP妯″紡)
 * conType : ESPCONN_TCP 閿熸枻鎷稥SPCONN_UDP
 * ipstr 閿熸枻鎷峰紡閿熸枻鎷� "255.255.255.255:port"
 */
void ICACHE_FLASH_ATTR
ESP8266_AP_NetCon_Init(struct espconn* ST_NetCon,enum espconn_type conType,const char* ipstr)
{
	struct ip_info ST_ESP8266_IP ;
	IP_Struct IPnum = ESP8266_IPStr2Num(ipstr);
	u8* p = (u8*)&(IPnum.ip);
	ST_NetCon->type = conType;		// 閫氶敓鏂ゆ嫹鍗忛敓鏂ゆ嫹
	*ESP8266_REMOTE_IP(ST_NetCon)  = IPnum.ip;		//閿熸枻鎷烽敓鐭唻鎷烽敓鏂ゆ嫹IP
	ST_NetCon->proto.tcp->local_port  = IPnum.port ;				// 閿熸枻鎷烽敓鐭唻鎷烽敓鎴鍖℃嫹
	//閿熸枻鎷烽敓鏂ゆ嫹IP
	ESP8266SetStateIP_Num(SOFTAP_IF,IPnum.ip);

	// 閿熸澃锝忔嫹閿熸枻鎷烽敓鏂ゆ嫹UDP閿熸枻鎷峰閿熸枻鎷稟PI
	//----------------------------------------------
	if(ST_NetCon->type == ESPCONN_UDP)
		espconn_create(ST_NetCon);	// 閿熸枻鎷峰閿熸枻鎷稶DP閫氶敓鏂ゆ嫹
	else if(ST_NetCon->type == ESPCONN_TCP)
		espconn_accept(ST_NetCon);// 閿熸枻鎷峰閿熸枻鎷稵CP閫氶敓鏂ゆ嫹
}

/***
 * 娉ㄩ敓鏂ゆ嫹姘愰敓鏂ゆ嫹閿熸枻鎷烽敓锟�
 */
void ICACHE_FLASH_ATTR
ESP8266_Regitst_Fun_Init(struct espconn* ST_NetCon,RegCBStr* cbfun)
{
	/**
	 * UDP
	 */
	JUGE_CB_NULL(espconn_regist_sentcb,ST_NetCon,cbfun->sent_callback);	// 娉ㄩ敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎹峰嚖鎷烽敓閰垫垚鐧告嫹閿熶茎鍥炵鎷烽敓鏂ゆ嫹閿熸枻鎷�
	JUGE_CB_NULL(espconn_regist_recvcb,ST_NetCon,cbfun->recv_callback);	// 娉ㄩ敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎹锋枻鎷烽敓绉告垚鐧告嫹閿熶茎鍥炵鎷烽敓鏂ゆ嫹閿熸枻鎷�
	/*
	 * TCP
	 */
	JUGE_CB_NULL(espconn_regist_connectcb,ST_NetCon,cbfun->connect_callback);	//TCP閿熸枻鎷烽敓鎺ユ垚鐧告嫹閿熸埅纰夋嫹
	JUGE_CB_NULL(espconn_regist_disconcb,ST_NetCon,cbfun->disconnect_callback); //TCP閿熻緝鍖℃嫹閿熸枻鎷烽敓鏂ゆ嫹
	JUGE_CB_NULL(espconn_regist_reconcb,ST_NetCon,cbfun->reconnect_callback);	//TCP閿熷眾甯搁敓杈冨尅鎷�
	JUGE_CB_NULL(espconn_regist_write_finish,ST_NetCon,cbfun->write_finish_fn);

}

//閿熸枻鎷峰彇杩滈敓鏂ゆ嫹閿熸枻鎷锋伅閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
void ICACHE_FLASH_ATTR
ESP8266_Get_ConInfo(struct espconn *arg,struct espconn *ST_NetCon)
{
	remot_info * P_port_info = NULL;	// 杩滈敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋伅閿熺粨鏋勯敓鏂ゆ嫹鎸囬敓鏂ゆ嫹
	if(espconn_get_connection_info(arg, &P_port_info, 0)==ESPCONN_OK)	// 閿熸枻鎷峰彇杩滈敓鏂ゆ嫹閿熸枻鎷锋伅
	{
		arg->proto.udp->remote_port = P_port_info->remote_port;
		os_memcpy(arg->proto.udp->remote_ip,P_port_info->remote_ip,4);	// 閿熻妭瀛樻嫹閿熸枻鎷�
		ST_NetCon->proto.udp->remote_port = P_port_info->remote_port;
		os_memcpy(ST_NetCon->proto.udp->remote_ip,P_port_info->remote_ip,4);	// 閿熻妭瀛樻嫹閿熸枻鎷�
	}
}

//wifi发送以字符串数据
void ICACHE_FLASH_ATTR
ESP8266_SendMessage(struct espconn *arg,u32 ipNum,u16 port,const char* str)
{
	arg->proto.udp->remote_port = port;
	*ESP8266_REMOTE_IP(arg) = ipNum;
	espconn_send(arg,(uint8*)str,os_strlen(str));	//发送

}

// wifi发送以字节数据
void ICACHE_FLASH_ATTR
ESP8266_SendData(struct espconn *arg,u32 ipNum,u16 port,uint8* dat,u32 len)
{
	arg->proto.udp->remote_port = port;
	*ESP8266_REMOTE_IP(arg) = ipNum;
	espconn_send(arg,dat,len);	//发送

}


//閿熸枻鎷烽敓鏂ゆ嫹TCP_SERVER閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
sint8 ICACHE_FLASH_ATTR
ESP8266_ConTCPServer(struct espconn *arg,const char* ipstr)
{
	IP_Struct IPnum = ESP8266_IPStr2Num(ipstr);
	*ESP8266_REMOTE_IP(arg)  = IPnum.ip;		//閿熸枻鎷烽敓鏂ゆ嫹杩滈敓鏂ゆ嫹IP
	arg->proto.tcp->remote_port  = IPnum.port ;				// 閿熸枻鎷烽敓鏂ゆ嫹杩滈敓鍓跨鍖℃嫹
	return espconn_connect(arg);

}

/*************************STA*************************************************/

/***
 * sta模式初始化
 */
void ICACHE_FLASH_ATTR
ESP8266_STA_Init(struct espconn* ST_NetCon,const char* ssid,const char* password)
{
	struct station_config STA_Config = {0} ;
	wifi_set_opmode(STATION_MODE);				// sta模式，并保存Flash
	os_strcpy(STA_Config.ssid,ssid);			//
	os_strcpy(STA_Config.password,password);	//
	wifi_station_set_config(&STA_Config);		// 閿熸枻鎷烽敓鏂ゆ嫹STA閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鑺ュ埌Flash
	ST_NetCon->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));	// 閿熸枻鎷烽敓鏂ゆ嫹閿熻妭瀛橈紝涓洪敓鍓挎唻鎷疯瘉閿熻妭瀛樹笉閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓绲婥P閿熻妭杈炬嫹

	// 閿熸枻鎷烽敓绲瑂er_init閿熷彨纰夋嫹閿熸枻鎷穡ifi_station_set_config(...)閿熶茎浼欐嫹閿熸枻鎷烽敓鑺傛牳浼欐嫹閿熺殕璁规嫹閿熸枻鎷稥SP8266閿熸枻鎷烽敓鏂ゆ嫹WIFI
	//----------------------------------------------------------------------------------
	// wifi_station_connect();		// ESP8266閿熸枻鎷烽敓鏂ゆ嫹WIFI閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷穟ser_init閿熸枻鎷峰閿熸枻鎷烽敓鍙鎷烽敓鏂ゆ嫹
}
/***
 * sta模式初始化，从FLASH中读取
 */
void ICACHE_FLASH_ATTR
ESP8266_STA_Init_FromFlash(struct espconn* ST_NetCon,u32 flashsector)
{
	struct station_config STA_Config = {0} ;
	spi_flash_read(flashsector*4096,(uint32 *)&STA_Config, 96);	// 读出【STA参数】(SSID/PASS)
	STA_Config.ssid[31] = 0;		// SSID最后添'\0'
	STA_Config.password[63] = 0;	// APSS最后添'\0'
	wifi_set_opmode(STATION_MODE);				// sta模式，并保存Flash
	wifi_station_set_config(&STA_Config);		// 閿熸枻鎷烽敓鏂ゆ嫹STA閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鑺ュ埌Flash
	ST_NetCon->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));	// 閿熸枻鎷烽敓鏂ゆ嫹閿熻妭瀛橈紝涓洪敓鍓挎唻鎷疯瘉閿熻妭瀛樹笉閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓绲婥P閿熻妭杈炬嫹

	// 閿熸枻鎷烽敓绲瑂er_init閿熷彨纰夋嫹閿熸枻鎷穡ifi_station_set_config(...)閿熶茎浼欐嫹閿熸枻鎷烽敓鑺傛牳浼欐嫹閿熺殕璁规嫹閿熸枻鎷稥SP8266閿熸枻鎷烽敓鏂ゆ嫹WIFI
	//----------------------------------------------------------------------------------
	// wifi_station_connect();		// ESP8266閿熸枻鎷烽敓鏂ゆ嫹WIFI閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷穟ser_init閿熸枻鎷峰閿熸枻鎷烽敓鍙鎷烽敓鏂ゆ嫹
}

/***
 * 保存路由器信息到FLASH中
 *
 */
void ICACHE_FLASH_ATTR
ESP8266_STA_Save2Flash(struct station_config *sta_conf ,u32 flashsector)
{
	spi_flash_erase_sector(flashsector);						// 擦除扇区
	spi_flash_write(flashsector*4096, (uint32 *)sta_conf, 96);	// 写入扇区
}

/***
 * STA模式服务器模式连接
 * conType : ESPCONN_TCP SPCONN_UDP
 *
 */
void ICACHE_FLASH_ATTR
ESP8266_STA_Server_NetCon_Init(struct espconn* ST_NetCon,enum espconn_type conType,u16 port)
{
	struct ip_info ST_ESP8266_IP ;

	ST_NetCon->type = conType;		// 保存类型
	ST_NetCon->proto.tcp->local_port  = port ;						// 保存端口号

	if(conType == ESPCONN_UDP)
		espconn_create(ST_NetCon);
	else if(conType == ESPCONN_TCP)
		espconn_accept(ST_NetCon);
}
/***
 * 通过字符串IP(STA-TCP_Client)
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
	espconn_connect(ST_NetCon);									// 连接TCP-server

}

/***
 * 通过数值IP(STA-TCP_Client)
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
	espconn_connect(ST_NetCon);	// 连接TCP-server
}


/**
 * DNS获取IP地址
 *
 */
err_t ICACHE_FLASH_ATTR
ESP8266_DNS_GetIP(struct espconn* ST_NetCon,const char* dns,dns_found_callback cb)
{
	return espconn_gethostbyname(ST_NetCon, dns, (ip_addr_t *)(&(ST_NetCon->proto.tcp->remote_ip[0])), cb);
}

/**
 * 获取本地IP地址，并保存在ST_NetCon.proto.tcp->local_ip
 * 获取成功返回true
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
		*ESP8266_LOCAL_IP(ST_NetCon) = ipInfo.ip.addr;//保存IP
	}
	return res;
}
// 远端IP
u32* ICACHE_FLASH_ATTR
ESP8266_REMOTE_IP(struct espconn * arg)
{
	return (u32*)&(arg->proto.tcp->remote_ip[0]);
}

// 本地IP
u32* ICACHE_FLASH_ATTR
ESP8266_LOCAL_IP(struct espconn *arg)
{
	return (u32*)&(arg->proto.tcp->local_ip[0]);
}

//HTTP GET请求
//---------以后在写

void ESP8266_Http_Get(struct espconn *ST_NetCon,const char* mes)
{

//	ESP8266_SendMessage(ST_NetCon,*ESP8266_REMOTE_IP(&ST_NetCon),80,mes);//发送消息
}
