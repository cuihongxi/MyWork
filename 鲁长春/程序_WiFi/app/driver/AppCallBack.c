#include "AppCallBack.h"
#include "myWifi.h"
#include "myGPIO.H"

/**
 * 发送成功回调函数
 * */
void ICACHE_FLASH_ATTR
ESP8266_WIFI_Send_Cb(void *arg)
{
	debug("Send ok\n");
}


// TCP连接断开成功的回调函数
//================================================================
void ICACHE_FLASH_ATTR ESP8266_TCP_Disconnect_Cb_JX(void *arg)
{

	debug("\nESP8266_TCP_Disconnect_OK\n");
}

// TCP连接异常断开时的回调函数
//====================================================================
void ICACHE_FLASH_ATTR ESP8266_TCP_Break_Cb_JX(void *arg,sint8 err)
{
	debug("\nESP8266_TCP_Break! 重新连接TCP-server ^！\n");
	espconn_connect(&ST_NetCon);	// 连接TCP-server
}

//收到数据的回调函数
void ESP8266_WIFI_Recv_Cb(void * arg, char * pdata, unsigned short len)
{

	// 根据数据设置LED的亮/灭
//	if(pdata[0] == 'k' || pdata[0] == 'K')	LED_ON();			// 首字母为'k'/'K'，灯亮
//	else if(pdata[0] == 'g' || pdata[0] == 'G')	LED_OFF();		// 首字母为'g'/'G'，灯灭
	debug("\r\n %d.%d.%d.%d:%d	：",\
			ST_NetCon.proto.tcp->remote_ip[0],	ST_NetCon.proto.tcp->remote_ip[1],\
			ST_NetCon.proto.tcp->remote_ip[2],ST_NetCon.proto.tcp->remote_ip[3],ST_NetCon.proto.tcp->remote_port);
	debug("%s\n",pdata);

}

// TCP连接建立成功的回调函数
//====================================================================================================================
void ICACHE_FLASH_ATTR ESP8266_TCP_Connect_Cb_JX(void *arg)
{
	debug("\n--------------- ESP8266_TCP_Connect_OK ---------------\n");
	ESP8266_Get_ConInfo(arg,&ST_NetCon);
	// 打印连接的远端IP
	debug("\r\n remote_ip = %d.%d.%d.%d\r\n",\
			ST_NetCon.proto.tcp->remote_ip[0],	ST_NetCon.proto.tcp->remote_ip[1],\
			ST_NetCon.proto.tcp->remote_ip[2],ST_NetCon.proto.tcp->remote_ip[3]);
	//ESP8266_SendMessage(&ST_NetCon,*ESP8266_REMOTE_IP(&ST_NetCon),80,HTTP_Message_485Comm);//发送消息

}

// DNS_域名解析结束_回调函数【参数1：域名字符串指针 / 参数2：IP地址结构体指针 / 参数3：网络连接结构体指针】
//=========================================================================================================
void ICACHE_FLASH_ATTR DNS_Over_Cb_JX(const char * name, ip_addr_t *ipaddr, void *arg)
{
	struct espconn * T_arg = (struct espconn *)arg;	// 缓存网络连接结构体指针
	//………………………………………………………………………………
	if(ipaddr == NULL)		// 域名解析失败
	{
		debug("\r\n---- 域名解析失败 ----\r\n");
		return;
	}

	//……………………………………………………………………………………………………………
	else if (ipaddr != NULL && ipaddr->addr != 0)		// 域名解析成功
	{
		debug("\r\n---- 域名解析成功 ----\r\n");
		debug("---> name :%s\n",name);

		*ESP8266_REMOTE_IP(T_arg) = ipaddr->addr;// 将解析到的服务器IP地址设为TCP连接的远端IP地址
		debug("\r\n remote_ip = %d.%d.%d.%d : %d\r\n",\
				ST_NetCon.proto.tcp->remote_ip[0],	ST_NetCon.proto.tcp->remote_ip[1],\
				ST_NetCon.proto.tcp->remote_ip[2],ST_NetCon.proto.tcp->remote_ip[3],ST_NetCon.proto.tcp->remote_port);

		// 连接 TCP server
		//----------------------------------------------------------
		ESP8266_STA_TCPClient_NetCon_ByInt(arg,*ESP8266_REMOTE_IP(&ST_NetCon),ST_NetCon.proto.tcp->remote_port);

	}
}

// SmartConfig状态发生改变时，进入此回调函数
//--------------------------------------------
// 参数1：sc_status status / 参数2：无类型指针【在不同状态下，[void *pdata]的传入参数是不同的】
//=================================================================================================================
void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata)
{
	debug("\r\n------ smartconfig_done ------\r\n");	// ESP8266网络状态改变

    switch(status)
    {
        case SC_STATUS_WAIT:						// 初始值, CmartConfig等待
            debug("\r\nSC_STATUS_WAIT\r\n");
        break;
        case SC_STATUS_FIND_CHANNEL:				// 发现【WIFI信号】（8266在这种状态下等待配网）
            debug("\r\nSC_STATUS_FIND_CHANNEL\r\n");
    		debug("\r\n---- Please Use WeChat to SmartConfig ------\r\n\r\n");
    	break;
        case SC_STATUS_GETTING_SSID_PSWD: 			// 正在获取【SSID】【PSWD】（8266正在抓取并解密【SSID+PSWD】）
            debug("\r\nSC_STATUS_GETTING_SSID_PSWD\r\n");
			sc_type *type = pdata;					// 获取【SmartConfig类型】指针

            if (*type == SC_TYPE_ESPTOUCH)			// 配网方式 == 【AIRKISS】||【ESPTOUCH_AIRKISS】
            	debug("\r\nSC_TYPE:SC_TYPE_ESPTOUCH\r\n");
            else
            	debug("\r\nSC_TYPE:SC_TYPE_AIRKISS\r\n");
	    break;
        case SC_STATUS_LINK:						// 成功获取到【SSID】【PSWD】，保存STA参数，并连接WIFI
            debug("\r\nSC_STATUS_LINK\r\n");
            struct station_config *sta_conf = pdata;	// 获取【STA参数】指针
			ESP8266_STA_Save2Flash(sta_conf ,Sector_STA_INFO); // 将【SSID】【PASS】保存到【外部Flash】中

			wifi_station_set_config(sta_conf);			// 设置STA参数【Flash】
	        wifi_station_disconnect();					// 断开STA连接
	        wifi_station_connect();						// ESP8266连接WIFI
	    break;
        case SC_STATUS_LINK_OVER: 						// ESP8266作为STA，成功连接到WIFI
            debug("\r\nSC_STATUS_LINK_OVER\r\n");
            smartconfig_stop();		// 停止SmartConfig，释放内存
    		if(wifi_station_get_connect_status() == STATION_GOT_IP )// 显示ESP8266的IP地址
    		{
    			if(ESP8266_Save_LocalIP(&ST_NetCon,STA_MOD) == true)
    			{
    				debug("ESP8266_IP = %d.%d.%d.%d\n",ST_NetCon.proto.tcp->local_ip[0],ST_NetCon.proto.tcp->local_ip[1],ST_NetCon.proto.tcp->local_ip[2],ST_NetCon.proto.tcp->local_ip[3]);
    			}
    		}
			debug("\r\n---- ESP8266 Connect to WIFI Successfully ----\r\n");

			//*****************************************************
			// WIFI连接成功，执行后续功能。	如：SNTP/UDP/TCP/DNS等
			//*****************************************************
	    break;
    }
}

/***
 *
 */
void ICACHE_FLASH_ATTR
OS_Timer_CB(void)
{
	static u8 flag_sw = 0;
	struct ip_info infoIP;
	static u8 flag_time = 0;
	uint8 S_WIFI_STA_Connect = wifi_station_get_connect_status();
	if(flag_sw==0)
	{
		debug(".");
		if(S_WIFI_STA_Connect == STATION_GOT_IP )
		{
			if(ESP8266_Save_LocalIP(&ST_NetCon,STA_MOD) == true)
			{
				debug("ESP8266_IP = %d.%d.%d.%d\n",ST_NetCon.proto.tcp->local_ip[0],ST_NetCon.proto.tcp->local_ip[1],ST_NetCon.proto.tcp->local_ip[2],ST_NetCon.proto.tcp->local_ip[3]);
			}

		  //  ESP8266_DNS_GetIP(&ST_NetCon,WWW_IP_ADDR,DNS_Over_Cb_JX);//解析DNS获取地址
			debug("--> 成功连接到WIFI\n");
			ESP8266_SNTP_Init();
			flag_sw = 1;

		}
		else if(S_WIFI_STA_Connect==STATION_NO_AP_FOUND 	||		// 未找到指定WIFI
				S_WIFI_STA_Connect==STATION_WRONG_PASSWORD 	||		// WIFI密码错误
				S_WIFI_STA_Connect==STATION_CONNECT_FAIL		)	// 连接WIFI失败
			{
				debug("\r\n---- S_WIFI_STA_Connect=%d-----------\r\n",S_WIFI_STA_Connect);
				debug("\r\n---- ESP8266 Can't Connect to WIFI-----------\r\n");

				// 微信智能配网设置
				//…………………………………………………………………………………………………………………………
				//wifi_set_opmode(STATION_MODE);			// 设为STA模式						//【第①步】

				smartconfig_set_type(SC_TYPE_AIRKISS); 	// ESP8266配网方式【AIRKISS】			//【第②步】

				smartconfig_start(smartconfig_done);	// 进入【智能配网模式】,并设置回调函数	//【第③步】
				flag_sw = 1;
			}

	}
//	if(flag_time == 0 && flag_sw == 1)
//	{
//		if(Get_SNTPTime()!=0)
//		{
//			flag_time = 1;
//			debug(" 当前时间: %s \n",Get_SNTPTime());
//			//debug("-------------- 连接TCP-Server -------------\n");
//			//ESP8266_STA_TCPClient_NetCon_ByStr(&ST_NetCon,"192.168.31.67:6666");
//		}
//
//	}

}

