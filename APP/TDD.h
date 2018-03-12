//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_TDD_H_
#define __INC_TDD_H_
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define TDD_ON							0	//打开TDD，这个配置会让程序申请一些测试用到的公共资源
#define	TDD_TIME_SEC					0   //180128 秒只与时间之间的转换
#define TDD_EFS							0		//171231
#define TDD_FM25						1		//171225
#define TDD_W25Q						0
#define TDD_MODCHANNEL 					0		
#define TDD_DEV_UART3 					0		
#define TDD_SMART_BUS					0
#define TDD_USB							0
#define TDD_SHEET						0
#define TDD_KEYBOARD					0
#define TDD_GPIO						0
#define TDD_LCD							0		
#define TDD_DEV_UART2 					0		//170430
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

void Tdd_Init(void);
void TDD_Time_sec(void);
void TDD_Efs(void);
void TDD_Fm25(void);
void TDD_W25q(void);
void TDD_Mvc(void);
void TDD_Gpio(void);
void TDD_Keyboard(void);
void TDD_Sheet(void);
void TDD_Usb(void);
void TDD_Smart_bus(void);
void TDD_Uart_2(void);
void TDD_Uart_3(void);
void TDD_Mdl_chn(void);




#endif
