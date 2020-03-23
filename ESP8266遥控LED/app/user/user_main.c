/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */


#include "uhead.h"
#include "delay.h"
#include "myWifi.h"
#include "mygpio.h"
#include "mytimer.h"
#include "myFlash.h"
#include "mystring.h"
#include "AppCallBack.h"
#include "myMQTT.h"
#include "aliyunMQTT.h"

/* 移植 */
#define		CLIENTID		"led_silou"							// 客户端ID ， 自己定义一个ID号

// 三元组
#define		DEVICENAME		"silou"								// Devicename
#define		DEVICESECRET	"0dJ429AQ54P7ZlOW1Kk63WcdnPAiowoM"		// Product Secret
#define		PRODUCTKEY		"a1OKFgeque8"							// Product Key

SessionStr* ss = 0;
/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
            rf_cal_sec = 512 - 5;
            break;
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
            rf_cal_sec = 1024 - 5;
            break;
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}



void ICACHE_FLASH_ATTR
user_rf_pre_init(void)
{
}


void ICACHE_FLASH_ATTR Switch_State(u8 state)
{
	if(state)
	{
		MYGPIO_SET(SWITCH);

	}else
	{
		MYGPIO_RESET(SWITCH);
	}
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
user_init(void)
{
	RegCBStr cbfun;
	u8 ledState = 1;
	uart_init(115200,115200);		//定义了两个串口的波特率和串口接收函数
	RxSetCallBack(RxOverCallBack);	// 设置接收回调函数
    debug("SDK version:%s\n--->CuiHongXi\n", system_get_sdk_version());
    _MYGPIO_SETMODE_OUTPUT(WIFISTATE_LED);
    _MYGPIO_SETMODE_OUTPUT(SWITCH);

    //Flash_Read(LED_STATE_ADDR, &ledState,1);
    Switch_State(ledState);									// 开灯
    MYGPIO_RESET(WIFISTATE_LED);							//打开LED
    Mytimer_hw_timer_Init(OS_Timer_CB,2000000);				// 定时检测WIFI联网状态
    ESP8266_STA_Init_FromFlash(&ST_NetCon,Sector_STA_INFO);	// 路由器账号密码
    //ESP8266_STA_Init(&ST_NetCon,"aa","12345678");
    cbfun.sent_callback = ESP8266_WIFI_Send_Cb;
    cbfun.recv_callback = ESP8266_WIFI_Recv_Cb;
    cbfun.connect_callback = ESP8266_TCP_Connect_Cb_JX;
    cbfun.disconnect_callback = ESP8266_TCP_Disconnect_Cb_JX;
    cbfun.reconnect_callback = ESP8266_TCP_Break_Cb_JX;
    ESP8266_Regitst_Fun_Init(&ST_NetCon,&cbfun);			// 依据协议注册回调函数

    AliyunStr* as = malloc(sizeof(AliyunStr));				// 申请一个ALIYUN结构体

    as->clientId = CLIENTID;
    as->deviceName = DEVICENAME;
    as->deviceSecret = DEVICESECRET;
    as->productKey = "a1OKFgeque8";
    debug("clientId : %s \n",as->clientId);
    debug("deviceName : %s \n",as->deviceName);
    debug("deviceSecret : %s \n",as->deviceSecret);
    debug("productKey : %s \n",as->productKey);
    ss = (SessionStr*)ConnectAliyunMqtt(".iot-as-mqtt.cn-shanghai.aliyuncs.com",1883,as);


}

