#include "mytimer.h"


os_timer_t  os_timer1;	// 定义软件定时器变量

/**
 * timer_cb :回调函数
 * time_ms： 延时时间
 * time_repetitive：只执行一次或者重复执行
 */
void ICACHE_FLASH_ATTR 
Mytimer_OS_Timer_Init(os_timer_t* timer, void * timer_cb, u32 time_ms, TIMER_REPEAT time_repetitive)
{
	os_timer_disarm(timer);										// 关闭软件定时器
	os_timer_setfn(timer,(os_timer_func_t *)timer_cb, NULL);	// 设置回调函数
	os_timer_arm(timer, time_ms, time_repetitive);				// 设置参数并使能
}


/***********************************************************************************
 *
 */
/******************************************************************************
* FunctionName : hw_timer_arm
* Description  : set a trigger timer delay for this timer.
* Parameters   : uint32 val :
in autoload mode
                        50 ~ 0x7fffff;  for FRC1 source.
                        100 ~ 0x7fffff;  for NMI source.
in non autoload mode:
                        10 ~ 0x7fffff;
* Returns      : NONE
*******************************************************************************/
void   hw_timer_arm(u32 val)
{
    RTC_REG_WRITE(FRC1_LOAD_ADDRESS, US_TO_RTC_TIMER_TICKS(val));
}

static void  (* user_hw_timer_cb)(void) = NULL;
/******************************************************************************
* FunctionName : hw_timer_set_func
* Description  : set the func, when trigger timer is up.
* Parameters   : void (* user_hw_timer_cb_set)(void):
                        timer callback function,
* Returns      : NONE
*******************************************************************************/
void   hw_timer_set_func(void (* user_hw_timer_cb_set)(void))
{
    user_hw_timer_cb = user_hw_timer_cb_set;
}

static void  hw_timer_isr_cb(void *arg)
{
    if (user_hw_timer_cb != NULL) {
        (*(user_hw_timer_cb))();
    }
}

static void  hw_timer_nmi_cb(void)
{
    if (user_hw_timer_cb != NULL) {
        (*(user_hw_timer_cb))();
    }
}

/******************************************************************************
* FunctionName : hw_timer_init
* Description  : initilize the hardware isr timer
* Parameters   :
FRC1_TIMER_SOURCE_TYPE source_type:
                        FRC1_SOURCE,    timer use frc1 isr as isr source.
                        NMI_SOURCE,     timer use nmi isr as isr source.
u8 req:
                        0,  not autoload,
                        1,  autoload mode,
* Returns      : NONE
*******************************************************************************/
void  hw_timer_init(FRC1_TIMER_SOURCE_TYPE source_type, u8 req)
{
    if (req == 1) {
        RTC_REG_WRITE(FRC1_CTRL_ADDRESS,
                      FRC1_AUTO_LOAD | DIVDED_BY_16 | FRC1_ENABLE_TIMER | TM_EDGE_INT);
    } else {
        RTC_REG_WRITE(FRC1_CTRL_ADDRESS,
                      DIVDED_BY_16 | FRC1_ENABLE_TIMER | TM_EDGE_INT);
    }

    if (source_type == NMI_SOURCE) {
        ETS_FRC_TIMER1_NMI_INTR_ATTACH(hw_timer_nmi_cb);
    } else {
        ETS_FRC_TIMER1_INTR_ATTACH(hw_timer_isr_cb, NULL);
    }

    TM1_EDGE_INT_ENABLE();
    ETS_FRC1_INTR_ENABLE();
}

/**************************************************************
 *
 *
 */


void  Mytimer_hw_timer_Init(hw_timer_cb_fun* fun, u32 time_us)
{
    hw_timer_init(FRC1_SOURCE,1);
    hw_timer_set_func(fun);
    hw_timer_arm(time_us);
}


#ifdef USE_SNTP
/*********************************************************/
//
void ICACHE_FLASH_ATTR
ESP8266_SNTP_Init(void)
{
	ip_addr_t * addr = (ip_addr_t *)os_zalloc(sizeof(ip_addr_t));

	sntp_setservername(0, "us.pool.ntp.org");
	sntp_setservername(1, "ntp.sjtu.edu.cn");	//

	ipaddr_aton("210.72.145.44", addr);			//
	sntp_setserver(2, addr);					//
	os_free(addr);								//

	sntp_init();	// SNTP

}

//
char*
Get_SNTPTime()
{
	char * Str_RealTime = 0;			//
	uint32	TimeStamp;		//
	TimeStamp = sntp_get_current_timestamp();
	if(TimeStamp)	//
	{
		Str_RealTime = sntp_get_real_time(TimeStamp);
	}
	return Str_RealTime;
}






#endif

