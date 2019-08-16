#include "COG12864.H"



//=============transfer command to LCM===============串口
void transfer_command(u8 data1)

{
	char i;
        CS0_OUT_0;
	RS_OUT_0;
	for(i=0;i<8;i++)
	{
          	if(data1&0x80) SDA_OUT_1;
		else SDA_OUT_0;
                data1 <<= 1;
                DELAY_TIMES
		CLK_OUT_0;
		DELAY_TIMES //加少量延时
		CLK_OUT_1;
		DELAY_TIMES //加少量延时		
	}
	CS0_OUT_1;
}

//-----------transfer data to LCM---------------
void transfer_data(u8 data1)
{
	char i;
	CS0_OUT_0;
	RS_OUT_1;
	for(i=0;i<8;i++)
	{
          	if(data1&0x80) SDA_OUT_1;
		else SDA_OUT_0;
                data1 <<= 1;
                 DELAY_TIMES
		CLK_OUT_0;
                DELAY_TIMES
		CLK_OUT_1;
                DELAY_TIMES		
	}
	CS0_OUT_1;
}

/*LCD 模块初始化*/
void initial_lcd()
{
	RST_OUT_0; /*低电平复位*/
        DELAY_3US;
	RST_OUT_1; /*复位完毕*/
        DELAY_6MS;
	transfer_command(0xe2); /*软复位*/
	DELAY_3US;
	transfer_command(0x2c); /*升压步聚1*/
	DELAY_3US;
	transfer_command(0x2e); /*升压步聚2*/
	DELAY_3US;
	transfer_command(0x2f); /*升压步聚3*/
	DELAY_3US;
	transfer_command(0x24); /*粗调对比度，可设置范围0x20～0x27*/
	transfer_command(0x81); /*微调对比度*/
	transfer_command(0x1f); /*0x1a,微调对比度的值，可设置范围0x00～0x3f 1f*/
	transfer_command(0xa2); /*1/9 偏压比（bias）*/
	transfer_command(0xc0); /*行扫描顺序：从上到下*/
	transfer_command(0xa1); /*列扫描顺序：从左到右*/
	transfer_command(0x40); /*起始行：第一行开始*/

}

void lcd_address(u8 page,u8 column)
{
	CS0_OUT_0;
	transfer_command(0xb0+page); 
	transfer_command(((column>>4)&0x0f)|0x10); //设置列地址的高4 位
	transfer_command(column&0x0f); //设置列地址的低4 位
        CS0_OUT_1;
}

/*全屏清屏*/
void clear_screen()
{
	unsigned char i,j;
	CS0_OUT_0;
	for(i=0;i<8;i++)
	{
		lcd_address(i,0+PIAN);
	for(j=0;j<128;j++)
	{	
                transfer_data(0x00);
	}
	}
	CS0_OUT_1;
}


//打点函数
//posY--页,posX列,w宽度,h高度
void COG12864_Display(const int* pdata, u16 w, u16 h, int posX, int posY)
{
     u16 i = 0,j = 0;
     lcd_address(posY,posX+PIAN);
     w = (w+1)/2;
     h = (h +7)/8;
     
     for(j=0;j<h;j++)
    {
        for(i=0;i<w;i++)
        {
            transfer_data(pdata[j*w+i]);
        }
        posY ++;
        lcd_address(posY,posX+PIAN);
    }
    
}

//加下划线的打点
void COG12864_Display_add_(const int* pdata, u16 w, u16 h, int posX, int posY)
{
     u8 i = 0,j = 0;
     lcd_address(posY,posX+PIAN);
     w = (w+1)/2;
     h = (h +7)/8;
     for(j=0;j<h;j++)
    {
      if(j<(h-1))
      {
        for(i=0;i<w;i++)
        {
            transfer_data(pdata[j*w+i]);
        }
             
      }else
      {
            for(i=0;i<w;i++)
            {
                transfer_data(pdata[j*w+i]|0x40);
            }
      }
      posY ++;
        lcd_address(posY,posX+PIAN); 
        
    }
    
}


void COG12864_GPIO_Init()
{
    //引脚初始化
    GPIO_Init(COG12864_CS0_PIN,COG12864_GPIO_MODE);      
    GPIO_Init(COG12864_RST_PIN,COG12864_GPIO_MODE);  
    GPIO_Init(COG12864_SDA_PIN,COG12864_GPIO_MODE);  
    GPIO_Init(COG12864_RS_PIN,COG12864_GPIO_MODE);  
    GPIO_Init(COG12864_CLK_PIN,COG12864_CLK_MODE); 
}
/**************************************
*@brief
*@note 
*@param
*@retval
***************************************/
void COG12864_Init(void)
{
    COG12864_GPIO_Init();
    initial_lcd();
    clear_screen();
}

void COG12864_Sleep(void)
{
 // transfer_command(0xAE);
//  transfer_command(0);
SDA_OUT_0;
RS_OUT_0;
CLK_OUT_0;
    
}

void COG12864_Wakeup(void)
{
  //transfer_command(0xAD);

 //  transfer_command(0xAF);
  
}
