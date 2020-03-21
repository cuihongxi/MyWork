#include "AppCallBack.h"
#include "myWifi.h"
#include "myGPIO.H"
#include "myMQTT.h"
#include "myTimer.h"

extern SessionStr* ss;
 u8 flag_sw = 0;
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
void ICACHE_FLASH_ATTR  ESP8266_TCP_Disconnect_Cb_JX(void *arg)
{

	debug("\nESP8266_TCP_Disconnect\n");
	WiFi_StateLed(1);	// LED
	flag_sw = 2;
}

// TCP连接异常断开时的回调函数
//====================================================================
void ICACHE_FLASH_ATTR  ESP8266_TCP_Break_Cb_JX(void *arg,sint8 err)
{
	//debug("\nESP8266_TCP_Break! Err : %d重新连接TCP-server ^！\n",err);
	//ss->messageType = CONNECT;
	//espconn_connect(&ST_NetCon);	// 连接TCP-server
	WiFi_StateLed(1);	// LED
	flag_sw = 2;

}

//收到数据的回调函数
void  ESP8266_WIFI_Recv_Cb(void * arg, char * pdata, unsigned short len)
{
	u8 i = 0;
	debug("\r\n %d.%d.%d.%d:%d ->HEX:\r\n",\
			ST_NetCon.proto.tcp->remote_ip[0],	ST_NetCon.proto.tcp->remote_ip[1],\
			ST_NetCon.proto.tcp->remote_ip[2],ST_NetCon.proto.tcp->remote_ip[3],ST_NetCon.proto.tcp->remote_port);
	for(i = 0;i<len;i++)
		debug(" %X ",pdata[i]);
	debug("\n 共  %d 个字节\r\n",len);

	ss->ServerCB(ss,pdata,len);// MQTT处理服务器回复报文

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

	if(ss->messageType == CONNECT)
	{
		ss->Connect(ss);
	}
	WiFi_StateLed(0);	// LED
}

// DNS_域名解析结束_回调函数【参数1：域名字符串指针 / 参数2：IP地址结构体指针 / 参数3：网络连接结构体指针】
//=========================================================================================================
void  DNS_Over_Cb_JX(const char * name, ip_addr_t *ipaddr, void *arg)
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
    		flag_sw = 4;
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
			//ESP8266_STA_Save2Flash(sta_conf ,Sector_STA_INFO); // 将【SSID】【PASS】保存到【外部Flash】中
			 Flash_Write(Sector_STA_INFO, (u8*) sta_conf,96);
			 debug("\r\n-----》 完成flash保存\r\n");
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
    			debug("--> 成功连接到WIFI <---\n");
    			WiFi_StateLed(0);	// 关闭LED
    			ESP8266_SNTP_Init();// 获取网络时间
    			flag_sw = 3;
    			if(ss->espconn->state != ESPCONN_CONNECT)
    			{
    				ss->espconn->proto.tcp->remote_port = ss->port;// 获取端口号
    				ss->messageType = CONNECT;
    				ESP8266_DNS_GetIP(ss->espconn,ss->url,DNS_Over_Cb_JX);//解析DNS获取地址
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
 * 2s定时器回调函数
 */
void ICACHE_FLASH_ATTR
OS_Timer_CB(void)
{

	struct ip_info infoIP;
	static u8 flag_time = 0;
	static JugeCStr juge = {0};
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
			WiFi_StateLed(0);	// 关闭LED
			ESP8266_SNTP_Init();// 获取网络时间
			flag_sw = 3;
			if(ss->espconn->state != ESPCONN_CONNECT)
			{
				ss->espconn->proto.tcp->remote_port = ss->port;// 获取端口号
				ss->messageType = CONNECT;
				ESP8266_DNS_GetIP(ss->espconn,ss->url,DNS_Over_Cb_JX);//解析DNS获取地址
			}
		}
		else if(S_WIFI_STA_Connect==STATION_NO_AP_FOUND 	||		// 未找到指定WIFI
				S_WIFI_STA_Connect==STATION_WRONG_PASSWORD 	||		// WIFI密码错误
				S_WIFI_STA_Connect==STATION_CONNECT_FAIL		)	// 连接WIFI失败
			{
				debug("\r\n---- S_WIFI_STA_Connect=%d-----------\r\n",S_WIFI_STA_Connect);
				debug("\r\n---- ESP8266 Can't Connect to WIFI-----------\r\n");

				// 微信智能配网设置f
				//…………………………………………………………………………………………………………………………
				//wifi_set_opmode(STATION_MODE);			// 设为STA模式						//【第①步】

				smartconfig_set_type(SC_TYPE_AIRKISS); 	// ESP8266配网方式【AIRKISS】			//【第②步】
				smartconfig_start(smartconfig_done);	// 进入【智能配网模式】,并设置回调函数	//【第③步】
				flag_sw = 1;
			}

	}
	if(flag_sw == 4)	// 配网时LED闪烁
	{
		static bool i = 0;
		i = !i;
		WiFi_StateLed(i);
	}
	if(flag_sw == 2)
	{
		debug(".");
		if(S_WIFI_STA_Connect == STATION_GOT_IP )
		{
			if(ESP8266_Save_LocalIP(&ST_NetCon,STA_MOD) == true)
			{
				debug("ESP8266_IP = %d.%d.%d.%d\n",ST_NetCon.proto.tcp->local_ip[0],ST_NetCon.proto.tcp->local_ip[1],ST_NetCon.proto.tcp->local_ip[2],ST_NetCon.proto.tcp->local_ip[3]);
			}

		  //  ESP8266_DNS_GetIP(&ST_NetCon,WWW_IP_ADDR,DNS_Over_Cb_JX);//解析DNS获取地址
			debug("重连WIFI\n");
			flag_sw = 3;
			if(ss->espconn->state != ESPCONN_CONNECT)
			{
				//ss->espconn->proto.tcp->remote_port = ss->port;// 获取端口号
				ss->messageType = CONNECT;
				espconn_connect(&ST_NetCon);// 重新连接
			}
		}
	}

	if(ss->espconn->state == ESPCONN_CONNECT)
	{
		juge.start = 1;
		if(Juge_counter(&juge, 100))
		{
			ss->KeepAlive(ss);
		}
	}else
	{
		juge.start = 0;
		juge.counter = 0;
	}

}

//接收回调函数
#include "netcmd.h"
void ICACHE_FLASH_ATTR RxOverCallBack(rxBuffStr* rxstr)
{
	os_printf("串口收到数据：len:%d,RX:%s\n",rxstr->len,rxstr->buff);
	RunNetCmd(rxstr->buff);			// 根据串口命令执行不同的网络命令
}
