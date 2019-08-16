/*
    MAIN函数中用到的函数
*/

#ifndef __APP_H
#define __APP_H


#include "UHEAD.H"
#include "MYCLK.H"
#include "stm8l15x_tim2.h"
#include "LCDCUI.H"
#include "PROJECT_LCD.H"
#include "SH20.H"
#include "24l01.h"
#include "KEYBOARD.H"
#include "stm8l15x_flash.h"
#include "stm8l15x_exti.h"
#include "stm8l15x_rtc.h"
#include "COG12864.H"
#include "stm8l15x_pwr.h"

typedef  enum
{
    MODE_BLOOM   =  0X40,
    MODE_VEG     =  0X80,
    MODE_ALLON   =  0XC0,
    MODE_ON      =  0x20,//状态标志
}MODTYPE;

#define         MODE_HOUR1      16
#define         MODE_HOUR2      14
#define         MODE_HOUR3      12
#define         MODE_HOUR4      8

#define         THR_CHECK_TIME  2       //温湿度检测间隔S
#define         LOWHIGH_TIME    86400   //12个小时间隔(500ms)

#define         EEPROM_ADDR     0x1000  //定义保存地址 0x1000开始
#define         EEPROM_ADDR_HZ  0x1010  //保存RF通讯频率

#define         EEPROM_ADDRESS0     0x1020  //保存NRF通讯地址
#define         EEPROM_ADDRESS1     0x1030  //保存NRF通讯地址
#define         EEPROM_ADDRESS2     0x1040  //保存NRF通讯地址
#define         EEPROM_ADDRESS3     0x1050  //保存NRF通讯地址
#define         EEPROM_ADDRESS4     0x1060  //保存NRF通讯地址
#define         EEPROM_BK           0x1070  //保存后背密码

#define         INIT_DAT             0x56   //初始密码      

#define         SlEEP_TIME      1      //单位s

#define         HEAD_MESSAGE    0xaa    //开头密码,跟接收板要一样
#define         MATCH_MESSAGE   0X36    //配对密码

#define         MAX_HZ          0x7f    //最高的频率范围(0~0x3f)

#define         ENTER_TIMES     4       //长按匹配时间

extern  u8 key_val              ;               //键值
extern  u8 flag_THRstart        ;               //温湿度检测开始标志
extern  u8 flag_THRstarting     ;               //温湿度检测进行中标志
extern  u8 counter_THR          ;               //温湿度检测计数器
extern  s16 max_T[2]           ;                //最高温度
extern  s16 max_H[2]           ;                //最高湿度
extern  s16 min_T[2]           ;                //最低温度
extern  s16 min_H[2]           ;                //最低湿度
extern  u32 counter_timer       ;               //小时计时器  
extern  u32 counter_enter       ;
extern  u8      flag_enter_match ;              //进入匹配标志
extern  u8 flag_lowhightime     ;               //小时更新标志
extern  u8 flag_sharp           ;               //闪烁标志
extern  u8 flag_sharpstart      ;               //闪烁开始
extern  u8 flag_sharping        ;               //闪烁已经进行
extern  u8 mod             ;                    //模式
extern  u8      hour            ;               //on小时
extern  u8      flag_nrf        ;               //当该标志位为1时NRF发送数据
extern  u8      flag_onffhour   ;               //刷新开关小时标志
extern  u8      flag_mode       ; 
extern  u8      flag_wake       ;               //睡眠\唤醒标识
extern  u16     time_wakeup     ;               //唤醒10秒间隔
extern  u16      count_sleep     ;               //睡眠计数器
extern  u8      on_off_switch   ;               //开关切换标志
extern  u8      Tem_HR_Check_i  ;

extern  u8  ADDRESS1[TX_ADR_WIDTH];
extern  u8  ADDRESS2[TX_ADR_WIDTH];

/*函数*/
void SendNrfDate(u8 mode,u8 hour); //NRF发送数据
void Rolloarray(s16* array);            //去掉24小时前的数据
s16 FindMin(s16* array);                //找出最大最小值
s16 FindMax(s16* array);
void TIM2_INIT();                       //TIM2初始化
u8 Tem_HR_Check();                    //温湿度检测
void Clear_Lcd();                       //清除模式位置的数字
void Show_Tonoff(u8 dat);               //显示模式位置的数字
void Sleep_Fun();                       //睡眠设置
void FreeNRFGPIO();
void FreeLCDGPIO();
void DATA_Init();                       // 参数初始化
void Wake_CheckHT();
void Show_MaxMin();

#endif