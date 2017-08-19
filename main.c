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
#include "intrInit.h"

#include "basis/macros.h"
#include "basis/sdhError.h"
#include "nokia_5110.h"
#include "device.h"

#include "TDD.h"
#include "ModelFactory.h"
#include "HMI/HMIFactory.h"

#include "control/CtlKey.h"
#include "utils/time.h"
#include "utils/keyboard.h"

#include "os/os_depend.h"
#if TDD_KEYBOARD == 1

#include "Gh_txt.h"
#include "glyph.h"

#endif

//@Deprecated
#include "ViewFactory.h"

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
void ThrdKeyRun (void const *argument);                             // thread function
osThreadId tid_Thread_key;                                          // thread id
osThreadDef (ThrdKeyRun, osPriorityNormal, 1, 0);                   // thread object

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------



//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{







/*
 * main: initialize and start the system
 */

//tdd data
#if TDD_ON == 1
char	appBuf[ 64];
uint8_t		u8_tmp;

#	if TDD_KEYBOARD == 1
static int KeyEvent( char num, keyMsg_t arr_msg[]);

KbTestOb *p_kbTestOb;
Glyph *mytxt ;
I_dev_lcd *lcd;
	
#	endif

#	if TDD_GPIO == 1

#define GITP_RISINGEDGE		0			
#define GITP_FAILINGEDGE	1	
I_dev_Char *p_devGpio[5];
uint8_t 	keyval;

static  void GpioIrqHdl( void *self, int type, int encode);
#	endif

#	if TDD_MVC == 1
struct  tm	rtcTm;
Model *mTime;
View	*vTest;
#	endif

#	if TDD_DEV_UART2 == 1

I_dev_Char *I_uart2;

#	endif

#endif







int main (void) {
//	USART_InitTypeDef USART_InitStructure;
	Keyboard	*p_kb;
	Controller	*p_ctlkey;

	Model 		*mTime;
	HMI 		*p_mainHmi;
	
	osKernelInitialize ();                    // initialize CMSIS-RTOS

  // initialize peripherals here
	OpenPrpClock();
	Pin_init();
	NVIC_Configuration();
	
	InitTimer( TIM2, 1000);
	clean_time_flags();
	
	mTime = ModelCreate("time");
	mTime->init( mTime, NULL);
	
	p_kb = GetKeyInsance();
	p_kb->init( p_kb, NULL);
	tid_Thread_key = osThreadCreate (osThread(ThrdKeyRun), p_kb);
	p_ctlkey = SUPER_PTR( Get_CtlKey(), Controller);
	p_ctlkey->init( p_ctlkey, p_kb);

	if (!tid_Thread_key) return(-1);
	
	p_mainHmi = CreateHMI( HMI_MAIN);
	p_mainHmi->init( p_mainHmi, NULL);
	
#if TDD_ON == 0

//app code
#else
#	if TDD_SHEET == 1
	p_mainHmi->show( p_mainHmi);

#	elif TDD_KEYBOARD == 1
	mytxt = ( Glyph *)Get_GhTxt();
	Dev_open( LCD_DEVID, (void *)&lcd);
	lcd->Clear( COLOUR_BLACK);
	p_kbTestOb = KbTestOb_new();
	p_kbTestOb->setKeyHdl( p_kbTestOb, KeyEvent);
	u8_tmp = p_kb->addOb( p_kb, ( keyObservice *)p_kbTestOb);
	

#	elif TDD_GPIO == 1
	
	for( u8_tmp = 0; u8_tmp < 5; u8_tmp ++)
	{
		Dev_open( DEVID_GPIO_D0 + u8_tmp, ( void *)&p_devGpio[u8_tmp]);
		p_devGpio[u8_tmp]->ioctol( p_devGpio[u8_tmp], DEVCMD_SET_IRQHDL, GpioIrqHdl, (void *)p_devGpio[u8_tmp]);
		p_devGpio[u8_tmp]->ioctol( p_devGpio[u8_tmp], DEVGPIOCMD_SET_ENCODE, u8_tmp);
	}
	while(1)
	{
		
		printf("\n key val: 0x%x\n", keyval);
	}
#	elif LCD_NOKIA == 1
	LCD_init();
    LCD_clear();
	sprintf( appBuf, "VER : %x %x", g_majorVer, g_childVer);
	LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_LOGO) *6)/2,2, appBuf);
#	elif TDD_MVC == 1
	
	mTime = ModelCreate("time");
	vTest = ViewCreate("test");
	
	mTime->init( mTime, NULL);
	mTime->getMdlData( mTime, 0, &rtcTm);
	
	vTest->init( vTest, NULL);
	vTest->myModel = mTime;
	
	
#	elif TDD_LCD == 1

	View_test();

#	elif TDD_DEV_UART2 == 1	
	
//#	if LCD_NOKIA == 1
//	sprintf( appBuf, "uart 2 testing ...");
//	LCD_clear();
//	LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_LOGO) *6)/2,1, appBuf);
//#	endif	
	Dev_open( DEVID_UART2, ( void *)&I_uart2);
	
	if( I_uart2->test(I_uart2, appBuf, 64) == RET_OK)
	{
		sprintf( appBuf, "succeed!");
		
	}
	else 
	{
		sprintf( appBuf, "failed!");
	}
	
	
	
//#		if LCD_NOKIA == 1
//	LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_LOGO) *6)/2,23, appBuf);
//#		endif
	
#	elif TDD_MVC == 1
	while(1)
	{
		osDelay(1000);
		vTest->show( vTest, NULL);
		
	}	

#	endif
	
#endif		//TDD_ON == 0
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);

  osKernelStart ();                         // start thread execution 

#if TDD_ON == 0
	
#else
#	if TDD_KEYBOARD == 1
	
	while(1)
	{
		
		p_kb->run( p_kb);
		delay_ms( 200);
	}

#	endif	
	
	
#endif
	
}

void ThrdKeyRun (void const *argument) {
	Keyboard	*p_kb = ( Keyboard	*)argument ;
	while (1) {    
		p_kb->run( p_kb);	
		osThreadYield ();                                           // suspend thread
	}
}

#	if TDD_KEYBOARD == 1
static int KeyEvent( char num, keyMsg_t arr_msg[])
{
	int i = 0;
	char buf[32];
	dspContent_t cnt;
	dspArea_t area  = {0};
	
	lcd->Clear( COLOUR_BLACK);
	cnt.colour = COLOUR_WHITE;
	cnt.font = FONT_16;
	sprintf(buf, "key num %d ", num);
	
	cnt.data = buf;
	cnt.len = strlen( buf);
	area.useArea.x1 = 0;
	area.useArea.y1 += 0;
	mytxt->draw( mytxt, &cnt, &area);
	for( i = 0; i < num; i++)
	{
		memset( buf, 0, sizeof( buf));
		memset( appBuf, 0, sizeof( appBuf));
		Keycode2Str( arr_msg[i].keyCode, 32, buf);
		strcat( appBuf, buf);
		Keyevnet2Str( arr_msg[i].eventCode, 32, buf);
		strcat( appBuf, " : ");
		strcat( appBuf, buf);
		
		cnt.data = appBuf;
		cnt.len = strlen( appBuf);
		area.useArea.x1 = 0;
		area.useArea.y1 += 24;
		mytxt->draw( mytxt, &cnt, &area);
//		sprintf( appBuf, "%s %s", 
	}
	
	
	return RET_OK;
	
	
	
}
#endif

#if TDD_GPIO == 1
static  void GpioIrqHdl( void *self, int type, int encode)
{
	if( type == GITP_FAILINGEDGE)
		keyval |= ( 1 << encode);
	else if( type == GITP_RISINGEDGE)
		keyval &= ~( 1 << encode);
	
}
#endif


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

