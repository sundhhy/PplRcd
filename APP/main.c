/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include <stdio.h>
#include <string.h>
#include "stm32f10x_usart.h"

#include "hardwareConfig.h"
#include "mySystemClock.h"
#include "pinmux.h"
#include "power.h"
#include "intrInit.h"
#include "sys_cmd.h"
#include "Modbus_app.h"

#include "basis/macros.h"
#include "basis/sdhError.h"
#include "device.h"
#include "TDD.h"
#include "HMI/HMIFactory.h"
#include "HMI/Component_tips.h"

#include "Usb.h"
#include "usb_hardware_interface.h"

#include "control/CtlKey.h"
#include "control/CtlTimer.h"
#include "utils/time.h"
#include "utils/keyboard.h"
#include "utils/Storage.h"

#include "unit_test/unit_test.h"



#include "os/os_depend.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
//系统版本号
//主板本号自己制定
//系统编译时的月份和日作为子版本
short  g_majorVer = 1;		
short  g_childVer;
		
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define LCD_NOKIE		0

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static void ThrdKeyRun (void const *argument);                             // thread function
osThreadId tid_Thread_key;                                          // thread id
osThreadDef (ThrdKeyRun, osPriorityNormal, 1, 0);                   // thread object

static usb_op_t	usb_op;


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static int	Main_USB_event(int type);
static void Init_usb_when_idle(void *arg);
void 	Init_LCD(void);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

/*
 * main: initialize and start the system
 */

//extern cmd_run_t 	cmd_run;


int main (void) {
	Keyboard		*p_kb;
	Controller		*p_control;
	
	Model 			*p_mdl_time;
//	CMP_tips 		*p_tips;
	uint16_t	main_ms = 0;


//	uint8_t			old_sys_flag;
	
  // initialize peripherals here

	//BSP的初始化
	OpenPrpClock();
	Pin_init();
	NVIC_Configuration();
	
	PVD_Init();

	
	Init_LCD();
	
	osKernelInitialize (); 
	Init_Cmd_Thread();
	// initialize CMSIS-RTOS
	//各个外设驱动模块初始化
	System_init();
	
	
	//控制器初始化
//#if TDD_ON == 0

	
	Unit_test();

	//界面初始化
	HMI_Init();
	phn_sys.lcd_cmd_bytes  = 0;

	//按键初始化
	p_kb = GetKeyInsance();
	
	//借用一下内存:&phn_sys.lcd_cmd_bytes
	main_ms = CONF_KEYSCAN_CYCLEMS;
	p_kb->init( p_kb, &main_ms);

	//创建控制器
	p_control = SUPER_PTR( Get_CtlKey(), Controller);
	p_control->init(p_control, p_kb);
	
	p_control = SUPER_PTR(CtlTimer_new(), Controller);
	if(p_control == NULL) while(1);
	p_control->init(p_control, NULL);
//	

	
	p_mdl_time = Create_model("time");
	
	
	osKernelStart ();  
	
	
	phn_sys.usb_ready = 0;
	Cmd_Rgt_time_task(Init_usb_when_idle, NULL, 3);		//3秒之后初始化USB。

	MBA_Init();
//	
//	
//	
//	
	phn_sys.sys_flag |= SYSFLAG_POWEON;
//	p_tips = TIP_Get_Sington();
	
//	old_sys_flag = phn_sys.sys_flag;
//	p_tips->show_ico_tips(1, -1);	//提示出USB未初始化
	HMI_TIP_ICO(TIP_ICO_WARING, 1);
	main_ms = 0;
	while(1)
	{
		#define MAIN_DELAY_MS		20
		osDelay(MAIN_DELAY_MS);
		
		//main_ms 从65530 + 10的时候会溢出，就会变成4
		if(main_ms % 10)
			main_ms = MAIN_DELAY_MS;
		
		main_ms += MAIN_DELAY_MS;

		
		if(phn_sys.usb_ready)	//在USB初始化的时候进行界面切换会产生花屏，所以就等USB初始化之后再允许按键切屏
			p_kb->run( p_kb);
		
		if((main_ms % 100) == 0)	
		{		
			if(phn_sys.usb_ready)	//在USB初始化的时候进行界面切换会产生花屏，所以就等USB初始化之后再允许按键切屏
			{
				
				USB_Run(!phn_sys.usb_device);

			}
			LCD_Run();
			
		}
		
//		p_mdl_time->run(p_mdl_time);

		if((main_ms % 500) == 0)		
		{
			p_mdl_time->run(p_mdl_time);
			g_p_curHmi->hmi_run(g_p_curHmi);
		}
		
		STG_Run();


	}

  
}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static void Init_usb_when_idle(void *arg)
{
//	CMP_tips 		*p_tips;
//	p_tips = TIP_Get_Sington();
	UHI_Init(&usb_op);
	if(USB_Init(&usb_op) != RET_OK)
	{
		phn_sys.sys_flag |= SYSFLAG_ERR;
		HMI_TIP_ICO(TIP_ICO_ERR, 1);
	}
	else
	{
		
		USB_Rgt_event_hdl(Main_USB_event);
	}
	
//	p_tips->clear_ico_tips(1);
	HMI_TIP_ICO(TIP_ICO_WARING, 0);

	phn_sys.usb_ready = 1;

}

static void ThrdKeyRun (void const *argument) {
	Keyboard	*p_kb = ( Keyboard	*)argument ;
	while (1) { 
		delay_ms(CONF_KEYSCAN_CYCLEMS);		
		p_kb->run( p_kb);
		osThreadYield ();                                           // suspend thread
	}
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData( DEBUG_USART, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus( DEBUG_USART, USART_FLAG_TC) == RESET)
    {
    }

    return ch;
}

int fgetc(FILE *f /*stream*/)
{
	 /* Loop until rx not empty */
    while (USART_GetFlagStatus( DEBUG_USART, USART_FLAG_RXNE) == RESET)
    {
    }
	
	return USART_ReceiveData( DEBUG_USART);
}


void HardFault_Handler()
{
	while(1) {
	;
	}
	
}



static int	Main_USB_event(int type)
{
//	CMP_tips *p_tips = TIP_Get_Sington();
	if(type == et_ready)
	{
		
		phn_sys.usb_device = 1;
//		p_tips->show_ico_tips(0, -1);
		HMI_TIP_ICO(TIP_ICO_USB, 1);
		
	}
	else if(type == et_remove)
	{
		phn_sys.usb_device = 0;
//		p_tips->clear_ico_tips(0);
		HMI_TIP_ICO(TIP_ICO_USB, 0);
	}
	
	return 0;
}

void 	Init_LCD(void)
{
	I_dev_lcd 				*tdd_lcd;

	Dev_open(LCD_DEVID, (void *)&tdd_lcd);
	tdd_lcd->open();
	tdd_lcd->Clear(COLOUR_BLUE);
//	tdd_lcd->done();
//	Flush_LCD();
//	LCD_Run();	//立即执行lcd指令
}
