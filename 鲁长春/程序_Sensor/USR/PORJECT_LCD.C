#include "PROJECT_LCD.H"

#include "COG12864.H"

u8 arraybuf[4] ={0};            //显示用
 
extern const int bmp_80x8[10][8];       //indoor humidity
extern const int bmp2_80x8[10][8] ;     //indoor TEMP

extern  GUI_FONT font_12x8_2 ;

//在显示屏上显示数值,不带正负
void LCD_Show_Data(GUI_FONT* font,u8 posx,u8 posy,s16 dat)
{
   
    if(dat<0)
    {
        dat = -dat;     //取反
       arraybuf[0] = '-';
    } 
    else if(dat<10) arraybuf[0] = ' ';
    else arraybuf[0] =dat%100/10 + 0x30;
    arraybuf[1] = dat%10 + 0x30; 
    CUIGUI_SetFont(font);
    LCD_ShowStr(posx,posy,(char*)arraybuf);
}

//在显示屏上显示数值,带正负
void LCD_Show_ZFData(GUI_FONT* font,u8 posx,u8 posy,s16 dat)
{
   u8 buf[4] ={0};
    if(dat<0)
    {
        buf[0] = '-';
        dat = -dat;     //取反
    }
    else if(dat>=100)
    {
        buf[0] = '1';
    }
    else   buf[0] = ' ';       
    if(dat<10) buf[1] = ' ';
    else buf[1] = dat%100/10 + 0x30;
        buf[2] = dat%10 + 0x30; 
    CUIGUI_SetFont(font);
    LCD_ShowStr(posx,posy,(char*)buf);
}
//初始化显示界面
void LCD_ShowInit()
{
    clear_screen();        //清屏
    CUIGUI_SetFont(&font_12x12);
    LCD_ShowStr(0,0,"                    ");//底下画横线
    CUIGUI_SetFont(&font_24x24);
    LCD_ShowStr(26+LCD_HR_POS,"%");
    LCD_ShowStr(26+LCD_TP_POS,"F");

    CUIGUI_SetFont(&font_12x12);
    LCD_ShowStr(0,0,"M:");
    LCD_ShowStr(60,0,"T:");       
    LCD_ShowStr(84,0,"on");     //有两种O,所以这里用小写来区分
    LCD_ShowStr(110,0,"off");
    
    CUIGUI_SetFont(&font_16x16);        
    LCD_ShowStr(17+LCD_MIN_HR_POS,"%");
    LCD_ShowStr(17+LCD_MAX_HR_POS,"%");
    CUIGUI_SetFont(&font_16x16_2);
    LCD_ShowStr(17+LCD_MIN_TP_POS,"F");
    LCD_ShowStr(17+LCD_MAX_TP_POS,"F"); 
    
    
    COG12864_Display((const int* )bmp_80x8,160,8,52, 2);////LCD_ShowStr(38,2,"indoor humidity");
    COG12864_Display((const int* )bmp2_80x8,160,8,52, 5);//  //LCD_ShowStr(38,5,"indoor Temp");
    CUIGUI_SetFont(&font_12x8);
    LCD_ShowStr(77,4,"L");
    LCD_ShowStr(116,4,"H");
  CUIGUI_SetFont(&font_12x8_2);
    LCD_ShowStr(77,7,"L");
    LCD_ShowStr(116,7,"H");    
    
}
