//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

/*
此文件提供单元测试的一些基础功能，如显示功能，按键功能。
使各个单元测试专门实现自己需要实现的部分即可。
一次测试只支持一个单元测试。

*/

#include "cfg_test.h"

#include "unit_test.h"
#include "osObjects.h"                      // RTOS object definitions
#include <stdio.h>
#include <string.h>
#include "stm32f10x_usart.h"

#include "hardwareConfig.h"
#include "mySystemClock.h"
#include "pinmux.h"
#include "intrInit.h"
#include "basis/macros.h"
#include "basis/sdhError.h"
#include "device.h"
#include "HMI/HMIFactory.h"
#include "Usb.h"
#include "usb_hardware_interface.h"
#include "control/CtlKey.h"
#include "control/CtlTimer.h"
#include "utils/time.h"
#include "utils/keyboard.h"
#include "os/os_depend.h"
#include "ModelFactory.h"
#include "Gh_txt.h"
#include "Gh_txt.h"
#include "glyph.h"

#include "Communication/smart_bus.h"


//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
#if UNT_ON == 1

//static I_dev_Char 			*I_uart2;
//static I_dev_Char 			*I_uart3;
//static KbTestOb 				*p_kbTestOb;
//static I_dev_Char 			*p_devGpio[5];
//static Model						*p_mdl_test;

static Glyph 						*mytxt ;
static I_dev_lcd 				*tdd_lcd;

//static uint16_t					*p_tdd_u16;
//static uint32_t					*p_tdd_u32;

//static uint16_t					arr_u16[2];
//static char							lcd_buf[64];
static char							test_buf[512];
//static char							udisk_buf[512];

//static do_out_t					tdd_do;
//static USB_file_info		usb_fin;

//static int							tdd_fd;
//static int							tdd_fd;
//static int							tdd_i, tdd_j, tdd_k, tdd_len, tdd_count = 0;
//static uint32_t					tdd_u32;
//static uint16_t					tdd_u16;
//static short						usb_cnt = 0;
//static uint8_t					tdd_u8;
//static char							line = 0;
//static char							tdd_finish = 0;
//static uint8_t					tdd_err;
//static uint8_t 					keyval;


#endif

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void 	Init_unit_test(void);


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
void Unit_test(void)
{
#if UNT_ON == 1
	Init_unit_test();
	
	
	Init_test(test_buf, sizeof(test_buf));
	osKernelStart (); 

	
	Run_test();
	
	while(1);
#endif		
	
}


#if UNT_ON == 1
void	UNT_Delay_ms(int ms)
{
	delay_ms(ms);
	
	
}

void UNT_Clean_lcd(void)
{
	
	tdd_lcd->Clear( COLOUR_BLACK);
	LCD_Run();
}

void	UNT_Disp_text(char	*text, int	line, int	row)
{
	dspContent_t cnt ={0};
	vArea_t area  = {0};
	
	cnt.colour = COLOUR_WHITE;
	cnt.font = FONT_16;
	
	cnt.data = text;
	cnt.len = strlen( text);
	area.x0 = row;
	area.y0 = 16 * line;
	mytxt->vdraw( mytxt, &cnt, &area);
	Flush_LCD();
	LCD_Run();
}


#endif
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
//============================================================================//
#if UNT_ON == 1

static void 	Init_unit_test(void)
{
	
//	Keyboard	*p_kb;
//	p_kb = GetKeyInsance();
//	p_kb->init( p_kb, &count);
	
	mytxt = ( Glyph *)Get_GhTxt();
	Dev_open( LCD_DEVID, (void *)&tdd_lcd);
	tdd_lcd->open();
	tdd_lcd->Clear( COLOUR_BLACK);
	Flush_LCD();
	LCD_Run();	//立即执行lcd指令
}





#endif
