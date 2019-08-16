#include "LCDCUI.H"
#include "COG12864.H" 


GUI_FONT* cuigui_Font = 0;  //定义全局变量


/**************************************
*@brief
*@note 
*@param
*@retval
***************************************/
void LCD_cuiInit(void)
{
    COG12864_Init();
}
/**************************************
*@brief  设定字体
*@note 
*@param
*@retval
***************************************/

void CUIGUI_SetFont(GUI_FONT* GUI_FontNum)
{
    cuigui_Font = GUI_FontNum;

}

//更换打点函数
void CUIGUI_ChangeFun(LCD_CREATE_FUNC* CreateFun)
{
    cuigui_Font->pCreateFun = CreateFun;
}

//更换字库
void CUIGUI_SetProp(const int*  prop)
{
    cuigui_Font ->prop = prop;
}


/************************************************************************************
*-函数名称	：输出一个宽度为w，高度为h的字模到屏幕的 (x,y) 坐标出，
*-参数	        ：文字的点阵数据为 pdata 所指
*返回值	        ：
*-函数功能	：
*-创建者	：
*/
void  _draw_model(const int* pdata, u16 w, u16 h, int x, int y)
{
     cuigui_Font->pCreateFun(pdata,w,h,x,y);
}

/************************************************************************************
*-函数名称	：打印一个字符
*-参数	        ：
*返回值	        ：返回字宽度
*-函数功能	：
*-创建者	：
*/
u16 CUIGUI_DrawWord( int x, int y,const int word)
{
      u16 w;        //宽
      u16 h;        //高     
      u16 i = 0;
    while((cuigui_Font->prop[i]) != (word))
    {
#if  DEBUG_LCDcui> 0
        USART_SendData8(USART1,(cuigui_Font->prop[i]));     
        USART1_SendSTR("\r\n");
#endif
        i += cuigui_Font->num;
    }

      w = cuigui_Font->space;                   //获得宽度
      h = cuigui_Font->height;                  //获得高度        
    _draw_model(&(cuigui_Font->prop[i+1]),w,h,x,y);
    
    return (w/2 );   
}

/**************************************
*@brief
*@note  在坐标是(x,y)开始显示字符串str
*@param
*@retval
***************************************/
u16 LCD_ShowStr(int x,int y,char* str)
{
      u16 w;
      while(*str != 0)
      {
          
          w = CUIGUI_DrawWord(x,y,(unsigned int)*str); 
           str ++;             
           x += w;
      }
      return (u16)x;
}

//开关显示
void LCD_Show(u8 state)
{
    transfer_command(state);
}