#include "nokia_5110.h"
#include "english_6x8_pixel.h"
#include "write_chinese_string_pixel.h"
#include "move_chinese_string_pixel.h"
//#include "stm32f10x_lib.h"

void LCD_init(void);
void LCD_clear(void);
void LCD_write_english_string(unsigned char X,unsigned char Y,char *s);
void LCD_write_chinese_string(unsigned char X, unsigned char Y,
                              unsigned char ch_with,unsigned char num,
                              unsigned char line,unsigned char row);
void chinese_string(unsigned char X, unsigned char Y, unsigned char T);
void LCD_write_char(unsigned char c);
void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                        unsigned char Pix_x,unsigned char Pix_y);
void LCD_write_byte(unsigned char dat, unsigned char dc);
void delay_1u(void);

/*-----------------------------------------------------------------------
LCD_init          : 3310LCD初始化

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 
-----------------------------------------------------------------------*/

void delay_1u(void)                 //1us延时函数
{
    unsigned int i;
    for(i=0;i<1000;i++);
}

void delay_1m(void)                 //1ms延时函数
{
    unsigned int i;
    for (i=0;i<1140;i++);
}

void delay_nms(unsigned int n)       //N ms延时函数
{
    unsigned int i=0;
    for (i=0;i<n;i++)
        delay_1m();
}

static void LcdPinmux()
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	RCC_APB2PeriphClockCmd( RCC_SCLK | RCC_SDIN | RCC_LCD_DC |
                           RCC_LCD_CE  | RCC_LCD_RST  , ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_SCLK;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(  GPIO_SCLK_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_SDIN;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(  GPIO_SDIN_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_LCD_DC;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(  GPIO_LCD_DC_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_LCD_CE;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(  GPIO_LCD_CE_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_LCD_RST;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(  GPIO_LCD_RST_PORT, &GPIO_InitStructure);
	
}

void LCD_init(void)
{
	
	LcdPinmux();
	
	
    // 产生一个让LCD复位的低电平脉冲
    GPIO_ResetBits(GPIO_LCD_RST_PORT,GPIO_LCD_RST);//LCD_RST = 0;
    delay_1u();

    GPIO_SetBits(GPIO_LCD_RST_PORT,GPIO_LCD_RST);//LCD_RST = 1;
    
    // 关闭LCD
    GPIO_ResetBits(GPIO_LCD_CE_PORT,GPIO_LCD_CE);//LCD_CE = 0;
    delay_1u();
    // 使能LCD
    GPIO_SetBits(GPIO_LCD_CE_PORT,GPIO_LCD_CE);//LCD_CE = 1;
    delay_1u();

    LCD_write_byte(0x21, 0);	// 使用扩展命令设置LCD模式
    LCD_write_byte(0xc8, 0);	// 设置偏置电压
    LCD_write_byte(0x06, 0);	// 温度校正
    LCD_write_byte(0x13, 0);	// 1:48
    LCD_write_byte(0x20, 0);	// 使用基本命令
    LCD_clear();	        // 清屏
    LCD_write_byte(0x0c, 0);	// 设定显示模式，正常显示

    // 关闭LCD
    GPIO_ResetBits(GPIO_LCD_CE_PORT,GPIO_LCD_CE);//LCD_CE = 0;
}

/*-----------------------------------------------------------------------
LCD_clear         : LCD清屏函数

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 
-----------------------------------------------------------------------*/
void LCD_clear(void)
{
    unsigned int i;

    LCD_write_byte(0x0c, 0);			
    LCD_write_byte(0x80, 0);			

    for (i=0; i<504; i++)
        LCD_write_byte(0, 1);
}

/*-----------------------------------------------------------------------
LCD_set_XY        : 设置LCD坐标函数

输入参数：X       ：0－83
          Y       ：0－5

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 
-----------------------------------------------------------------------*/
void LCD_set_XY(unsigned char X, unsigned char Y)
{
    LCD_write_byte(0x40 | Y, 0);		// column
    LCD_write_byte(0x80 | X, 0);          	// row
}

/*-----------------------------------------------------------------------
LCD_write_char    : 显示英文字符

输入参数：c       ：显示的字符；

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 
-----------------------------------------------------------------------*/
void LCD_write_char(unsigned char c)
{
    unsigned char line;

    c -= 32;

    for (line=0; line<6; line++)
        LCD_write_byte(font6x8[c][line], 1);
}

/*-----------------------------------------------------------------------
LCD_write_english_String  : 英文字符串显示函数

输入参数：*s      ：英文字符串指针；
          X、Y    : 显示字符串的位置,x 0-83 ,y 0-5

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 		
-----------------------------------------------------------------------*/
void LCD_write_english_string(unsigned char X,unsigned char Y,char *s)
{
    LCD_set_XY(X,Y);
    while (*s) 
    {
        LCD_write_char(*s);
        s++;
    }
}
/*-----------------------------------------------------------------------
LCD_write_chinese_string: 在LCD上显示汉字

输入参数：X、Y    ：显示汉字的起始X、Y坐标；
          ch_with ：汉字点阵的宽度
          num     ：显示汉字的个数；  
          line    ：汉字点阵数组中的起始行数
          row     ：汉字显示的行间距
编写日期          ：2004-8-11 
最后修改日期      ：2004-8-12 
测试：
	LCD_write_chi(0,0,12,7,0,0);
	LCD_write_chi(0,2,12,7,0,0);
	LCD_write_chi(0,4,12,7,0,0);	
-----------------------------------------------------------------------*/                        
void LCD_write_chinese_string(unsigned char X, unsigned char Y, 
                              unsigned char ch_with,unsigned char num,
                              unsigned char line,unsigned char row)
{
    unsigned char i,n;
    
    LCD_set_XY(X,Y);                             //设置初始位置
    
    for (i=0;i<num;)
    {
      	for (n=0; n<ch_with*2; n++)              //写一个汉字
        {
      	    if (n==ch_with)                      //写汉字的下半部分
            {
      	        if (i==0) LCD_set_XY(X,Y+1);
      	        else
                    LCD_set_XY((X+(ch_with+row)*i),Y+1);
            }
      	    LCD_write_byte(write_chinese[line+i][n],1);
        }
      	i++;
      	LCD_set_XY((X+(ch_with+row)*i),Y);
    }
}

/*-----------------------------------------------------------------------
LCD_move_chinese_string: 汉字移动

输入参数：X、Y    ：显示汉字的起始X、Y坐标；
          T       ：移动速度；

编写日期          ：2004-8-13 
最后修改日期      ：2004-8-13 
-----------------------------------------------------------------------*/                        
void chinese_string (unsigned char X, unsigned char Y, unsigned char T)
{
    unsigned char i,n,j=0;
    unsigned char buffer_h[84]={0};
    unsigned char buffer_l[84]={0};

    for (i=0; i<MAX_MOVE_TEXT; i++)
    {
        buffer_h[83] = move_chinese_string[i/12][j];
        buffer_l[83] = move_chinese_string[i/12][j+12];
        j++;
        if (j==12) j=0;
        
        for (n=0; n<83; n++)
        {
            buffer_h[n]=buffer_h[n+1];
            buffer_l[n]=buffer_l[n+1];
        }
        
        LCD_set_XY(X,Y);
        for (n=0; n<83; n++)
        {
            LCD_write_byte(buffer_h[n],1);
        }
        
        LCD_set_XY(X,Y+1); 
        for (n=0; n<83; n++)
        {
            LCD_write_byte(buffer_l[n],1);
        }

        delay_nms(T);
    }
}

/*-----------------------------------------------------------------------
LCD_draw_map      : 位图绘制函数

输入参数：X、Y    ：位图绘制的起始X、Y坐标；
          *map    ：位图点阵数据；
          Pix_x   ：位图像素（长）
          Pix_y   ：位图像素（宽）

编写日期          ：2004-8-13
最后修改日期      ：2004-8-13 
-----------------------------------------------------------------------*/
void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                        unsigned char Pix_x,unsigned char Pix_y)
{
    unsigned int i,n;
    unsigned char row;
    
    if (Pix_y%8==0) row=Pix_y/8;      //计算位图所占行数
    else
        row=Pix_y/8+1;
    
    for (n=0;n<row;n++)
    {
      	LCD_set_XY(X,Y);
        for(i=0; i<Pix_x; i++)
        {
            LCD_write_byte(map[i+n*Pix_x], 1);
        }
        Y++;                         //换行
    }
}

/*-----------------------------------------------------------------------
LCD_write_byte    : 使用SPI接口写数据到LCD

输入参数：data    ：写入的数据；
          command ：写数据/命令选择；

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-13 
-----------------------------------------------------------------------*/
void LCD_write_byte(unsigned char dat, unsigned char command)
{
    unsigned char i;
    GPIO_ResetBits(GPIO_LCD_CE_PORT,GPIO_LCD_CE);//LCD_CE = 0;
    
    if (command == 0)
        GPIO_ResetBits(GPIO_LCD_DC_PORT,GPIO_LCD_DC);//LCD_DC = 0;
    else
        GPIO_SetBits(GPIO_LCD_DC_PORT,GPIO_LCD_DC);//LCD_DC = 1;
    for(i=0;i<8;i++)
    {
        if(dat&0x80)
            GPIO_SetBits(GPIO_SDIN_PORT,GPIO_SDIN);//SDIN = 1;
        else
            GPIO_ResetBits(GPIO_SDIN_PORT,GPIO_SDIN);//SDIN = 0;
        GPIO_ResetBits(GPIO_SCLK_PORT,GPIO_SCLK);//SCLK = 0;
        dat = dat << 1;
        GPIO_SetBits(GPIO_SCLK_PORT,GPIO_SCLK);//SCLK = 1;
    }
    GPIO_SetBits(GPIO_LCD_CE_PORT,GPIO_LCD_CE);//LCD_CE = 1;
}


