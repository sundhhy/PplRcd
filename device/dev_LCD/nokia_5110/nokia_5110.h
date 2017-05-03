#ifndef _NOKIA_5110_H_
#define _NOKIA_5110_H_
#include "stm32f10x_gpio.h"


// pin define for n5110lcd_8key board
// change this if your hardware changed!

#define SERIAL_PORT "Serial port [OK]"
#define START_GSM   "Start Gsm ......"
#define SIM_TCPIP_OK "TCP/IP    [OK]"
#define SIM_TCPIP_FALSE "TCP/IP    [NO]"
#define SIM_TCPIP_ALREADY_CONNECT "ALREADY CONN  "
#define SIM_TCPIP "TCP/IP ...  "
#define SIM_SEND_GPRS_DATA_TEST "SEND GPRS DATA"
#define GPS_INIT "Gps Check [OK]"

#define SIM_LOGO           "^ STM32+GPRS ^"
#define SIM_REGISTER_FALSE "GSM NET   [NO]"
#define SIM_REGISTER      "GSM REGISTER.."
#define SIM_START_FALSE   "SIM START [NO]"	  
#define SIM_START_SUCCEED "SIM START [OK]"
#define SIMCARD_SUCCEED   "SIMCARD   [OK]"
#define SIMCARD_FALSE     "SIMCARD   [NO]"
#define LCD_BANK_LINE "              "
#define SIM_GPRS_FALSE "GPRS NET  [NO]"
#define SIM_GPRS_TRUE "GPRS NET  [OK]"
#define SIM_GPRS "GPRS NET ...  "
#define SIM_SEND_TEST_GPRS_DATA "test send gprs succeed \r\n"
#define LCD_WIDTH_PIXELS 84
#define NO_TEM "no template"


#define RCC_SCLK                                        RCC_APB2Periph_GPIOC		 /*按键1使用的GPIO时钟*/
#define GPIO_SCLK_PORT                                  GPIOA    					 /*按键1使用的GPIO组*/
#define GPIO_SCLK                                       GPIO_Pin_5					 /*按键1连接的GPIO管脚号*/

#define RCC_SDIN                                        RCC_APB2Periph_GPIOC	     /*按键2使用的GPIO时钟*/
#define GPIO_SDIN_PORT                                  GPIOA  						 /*按键2使用的GPIO组*/
#define GPIO_SDIN                                       GPIO_Pin_7				     /*按键2连接的GPIO管脚号*/

#define RCC_LCD_DC                                      RCC_APB2Periph_GPIOC		 /*按键1使用的GPIO时钟*/
#define GPIO_LCD_DC_PORT                                GPIOC    					 /*按键1使用的GPIO组*/
#define GPIO_LCD_DC                                     GPIO_Pin_8					 /*按键1连接的GPIO管脚号*/

#define RCC_LCD_CE                                      RCC_APB2Periph_GPIOC	     /*按键2使用的GPIO时钟*/
#define GPIO_LCD_CE_PORT                                GPIOA  						 /*按键2使用的GPIO组*/
#define GPIO_LCD_CE                                     GPIO_Pin_11				     /*按键2连接的GPIO管脚号*/

#define RCC_LCD_RST                                     RCC_APB2Periph_GPIOC	     /*按键2使用的GPIO时钟*/
#define GPIO_LCD_RST_PORT                               GPIOA  						 /*按键2使用的GPIO组*/
#define GPIO_LCD_RST                                    GPIO_Pin_12				     /*按键2连接的GPIO管脚号*/



extern void LCD_init(void);
extern void LCD_clear(void);
extern void LCD_write_english_string(unsigned char X,unsigned char Y,char *s);
extern void LCD_write_chinese_string(unsigned char X, unsigned char Y,
                                     unsigned char ch_with,unsigned char num,
                                     unsigned char line,unsigned char row);
extern void chinese_string(unsigned char X, unsigned char Y, unsigned char T);
extern void LCD_write_char(unsigned char c);
extern void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                               unsigned char Pix_x,unsigned char Pix_y);
extern void LCD_write_byte(unsigned char dat, unsigned char dc);
extern void delay_1us(void);

#endif

