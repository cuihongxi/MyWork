/*
*       显示屏程序
*       需要将显示驱动移植过来
*/

#ifndef __LCDCUI_H
#define __LCDCUI_H

#include "UHEAD.H"
    
#define DEBUG_LCDcui    0       //调试开关

typedef void LCD_CREATE_FUNC  (const int* pdata, u16 w, u16 h, int x, int y);


typedef struct 
{
   u16    height ;           /* height of font  */
   u16    space;             /* space of font y */
   u16    num;                   //一个字的字节数
   const int*  prop;             //指向字库
   LCD_CREATE_FUNC* pCreateFun; //指向打点函数
}GUI_FONT;

extern  GUI_FONT font_16x16; 
extern  GUI_FONT font_12x12; 
extern  GUI_FONT font_12x8; 
extern  GUI_FONT font_24x24; 
extern  GUI_FONT font_24x24_2;
extern  GUI_FONT font_16x16_2;

/*
    函数
*/

void LCD_cuiInit(void);
void CUIGUI_SetFont(GUI_FONT* GUI_FontNum);
u16 LCD_ShowStr(int x,int y,char* str);
u16 CUIGUI_DrawWord( int x, int y,const int word);
void CUIGUI_ChangeFun(LCD_CREATE_FUNC* CreateFun);//更换打点函数
void CUIGUI_SetProp(const int*  prop);//更换字库
void LCD_Show(u8 state);//开关显示 SHOW_OPEN /SHOW_CLOSE

#endif