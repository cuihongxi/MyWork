/**

 */
#ifndef __mytimer_h
#define __mytimer_h


#include "uhead.h"
#include "ets_sys.h"

#define		USE_SNTP
#ifdef 		USE_SNTP

#include "sntp.h"				// SNTP
#include "mem.h"

#endif


extern os_timer_t  os_timer1;


typedef enum{
    ONLY_ONCE,
    REPEAT_TIMES,
}TIMER_REPEAT;        // 重复结构体




//
// 软件定时器初始化
//
void  Mytimer_OS_Timer_Init(os_timer_t* timer, void * timer_cb, u32 time_ms, TIMER_REPEAT time_repetitive);


/*****************************************************************************************************
 *
 */

#define US_TO_RTC_TIMER_TICKS(t)          \
    ((t) ?                                   \
     (((t) > 0x35A) ?                   \
      (((t)>>2) * ((APB_CLK_FREQ>>4)/250000) + ((t)&0x3) * ((APB_CLK_FREQ>>4)/1000000))  :    \
      (((t) *(APB_CLK_FREQ>>4)) / 1000000)) :    \
     0)

#define FRC1_ENABLE_TIMER   BIT7
#define FRC1_AUTO_LOAD      BIT6

//TIMER PREDIVED MODE
typedef enum {
    DIVDED_BY_1 = 0,		//timer clock
    DIVDED_BY_16 = 4,		//divided by 16
    DIVDED_BY_256 = 8,		//divided by 256
} TIMER_PREDIVED_MODE;

typedef enum {			//timer interrupt mode
    TM_LEVEL_INT = 1,	// level interrupt
    TM_EDGE_INT   = 0,	//edge interrupt
} TIMER_INT_MODE;

typedef enum {
    FRC1_SOURCE = 0,
    NMI_SOURCE = 1,
} FRC1_TIMER_SOURCE_TYPE;

//定时判断数据类型
typedef struct{
	u8 start;			//启动定时
	u8 switchon;		//定时到，开关打开
	u32 counter;		//计数
}JugeCStr;

#define	IRQ_PERIOD	2

typedef void hw_timer_cb_fun (void);
/**
 * 硬件定时器初始化
 */
void Mytimer_hw_timer_Init(hw_timer_cb_fun* fun, u32 time_us);

#ifdef 		USE_SNTP


void ESP8266_SNTP_Init(void);//

char* Get_SNTPTime();			//
bool Juge_counter(JugeCStr* juge, u32 swdat);

#endif

#endif
