/**
 * 
 * 2019年10月11日06:53:45
 */
#ifndef __mygpio_h
#define __mygpio_h


#include "uhead.h"
#include "gpio.h"

#define		MYGPIO_SET(pin)             GPIO_OUTPUT_SET(GPIO_ID_PIN(pin),1)
#define		MYGPIO_RESET(pin)           GPIO_OUTPUT_SET(GPIO_ID_PIN(pin),0)
#define     MYGPIO_SETMODE_OUTPUT(pin)  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO##pin##_U,FUNC_GPIO##pin)          
#define     MYGPIO_SETMODE_INPUT(pin)   do{\
                    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO##pin##_U,FUNC_GPIO##pin);\
                    GPIO_DIS_OUTPUT(GPIO_ID_PIN(pin));\
                    PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO##pin##_U);\
}while(0)

#define     MYGPIO_PULLUP_EN(pin)       PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO##pin##_U)
#define     MYGPIO_PULLUP_DIS(pin)      PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO##pin##_U)
#define     MYGPIO_READ(pin)            GPIO_INPUT_GET(GPIO_ID_PIN(pin))
#define     MYGPIO_Toggle(pin)          GPIO_OUTPUT_SET(GPIO_ID_PIN(pin),!GPIO_INPUT_GET(GPIO_ID_PIN(pin))) //取反引脚状态
/**
 * GPIO EXTI 外部中断
 */
#define     MYGPIO_EXTI_FALL(pin,fun)    do{\
                    ETS_GPIO_INTR_DISABLE();/*关闭GPIO中断*/\
                    ETS_GPIO_INTR_ATTACH((ets_isr_t)fun,NULL);/*注册回调函数 */\
                    gpio_pin_intr_state_set(GPIO_ID_PIN(pin), GPIO_PIN_INTR_NEGEDGE);/*下降沿中断*/\
                    ETS_GPIO_INTR_ENABLE();\
}while(0)

/**
 * 清除中断状态
 */
#define     MYGPIO_EXTI_CLEAR(pin)      GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS,0X01<<pin)
/**
 *
 * 获得引脚中断状态
 */
#define     MYGPIO_GET_EXTI_STATUS(pin) (GPIO_REG_READ(GPIO_STATUS_ADDRESS)&(0X01<<pin))


#endif
