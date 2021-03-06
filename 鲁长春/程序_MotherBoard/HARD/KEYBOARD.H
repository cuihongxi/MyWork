/*
*�������ɨ�����
*/

#ifndef __KEYBOARD_H
#define __KEYBOARD_H
#include "UHEAD.H"

//------------------------------------����ɨ��----------------------------	

#define KEY_VAL_DER_Z		1
#define KEY_VAL_DER_Y		2
#define KEY_VAL_AM		4
#define KEY_VAL_Y30            	8 
#define KEY_VAL_DM            	0X10

#define KEY_VAL_NULL            0       //û����

//���Ŷ���      
#define		GPIO_DER_Z			GPIOB,GPIO_Pin_0 		// <z ������
#define		GPIO_DER_Y			GPIOB,GPIO_Pin_1		// >Y������
#define		GPIO_AM				GPIOB,GPIO_Pin_2 		// AM ������
#define		GPIO_Y30			GPIOB,GPIO_Pin_3 		// Y30 ������
#ifndef  	DM_LED_RESET               
#define		GPIO_DM				GPIOD,GPIO_Pin_1		// �����
#else
#define		GPIO_DM				GPIOD,GPIO_Pin_7		// �����
#endif

#define     KEY_GPIOMODE        GPIO_Mode_In_PU_IT		

typedef enum{
	off,		//��
	on,			//��
	two,		//˫��
	three,		//����
	four,
	five,
	six,
}keyEnum;

typedef struct{
	GPIO_TypeDef* GPIOx;
	GPIO_Pin_TypeDef GPIO_Pin;
	u32 counter;
	keyEnum val;
	
}keyStr;


extern  u8      flag_exti;
extern  u8 	key_val;

extern  keyStr key_AM ;

void Key_GPIO_Init();
void Key_ScanLeave();                   //���ֳ���


#endif









