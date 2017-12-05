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
#include "Usb.h"

#include "control/CtlKey.h"
#include "control/CtlTimer.h"
#include "utils/time.h"
#include "utils/keyboard.h"

#include "os/os_depend.h"
#if TDD_KEYBOARD == 1

#include "Gh_txt.h"
#include "glyph.h"

#endif

# if TDD_SMART_BUS == 1
#include "Communication/smart_bus.h"
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
char		appBuf[ 64];
int			tdd_i, tdd_j;
char			line = 0;
char			tdd_finish = 0;

Glyph 		*mytxt ;
I_dev_lcd 	*tdd_lcd;

void 	Tdd_init(void);
void	Tdd_disp_text(char	*text, int	line, int	row);
void	Tdd_disp_clean();
# if TDD_SMART_BUS == 1

#	endif
#	if TDD_USB == 1
#define 	USB_TFILE	"/TDD3.TXT"
int						tdd_fd;
char					udisk_buf[512];
short						usb_cnt = 0;
USB_file_info	usb_fin;
	
int	Usb_event(int type);
#	endif

#	if TDD_KEYBOARD == 1
static int KeyEvent( char num, keyMsg_t arr_msg[]);

KbTestOb 	*p_kbTestOb;
I_dev_lcd 	*lcd;
	
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

#	if TDD_DEV_UART3 == 1 || TDD_SMART_BUS == 1

I_dev_Char *I_uart3;

#	endif

#endif		//TDD_ON








int main (void) {
//	USART_InitTypeDef USART_InitStructure;
	Keyboard	*p_kb;
	Controller	*p_ctlkey;
	Controller	*p_ctlTime;
	Model 		*mTime;
	Model 		*p_mdl_test;
	HMI 			*p_mainHmi;
	int			ret = 0;
	short			count = 0;
	short			hmi_count = 0;
#if TDD_ON == 1
	uint8_t			tmp_u8 = 0;
#endif
	osKernelInitialize ();                    // initialize CMSIS-RTOS

  // initialize peripherals here
	
	//BSP的初始化
	System_init();
	OpenPrpClock();
	Pin_init();
	NVIC_Configuration();
	
	//各个外设驱动模块初始化
	
	ret = USB_Init(NULL);
	assert(ret == RET_OK);
	
	InitTimer( TIM2, 1000);
	clean_time_flags();
	
	mTime = ModelCreate("time");
	mTime->init( mTime, NULL);
	
	p_mdl_test =  ModelCreate("test");
	p_mdl_test->init( p_mdl_test, NULL);
	
	//控制器初始化
	
	p_ctlTime = SUPER_PTR(CtlTimer_new(), Controller);
	p_ctlTime->init(p_ctlTime, NULL);
		//按键初始化
	p_kb = GetKeyInsance();
	count = CONF_KEYSCAN_CYCLEMS;
	p_kb->init( p_kb, &count);
	tid_Thread_key = osThreadCreate (osThread(ThrdKeyRun), p_kb);
	p_ctlkey = SUPER_PTR( Get_CtlKey(), Controller);
	p_ctlkey->init( p_ctlkey, p_kb);

	if (!tid_Thread_key) return(-1);
#if TDD_ON == 1
	Tdd_init();
#endif
	
	
#if TDD_ON == 0
	//界面初始化
	p_mainHmi = CreateHMI( HMI_MAIN);
	p_mainHmi->init( p_mainHmi, NULL);
	p_mainHmi->show( p_mainHmi);
	Set_flag_show(&p_mainHmi->flag, 1); 
	
	// create 'thread' functions that start executing,
	// example: tid_name = osThreadCreate (osThread(name), NULL);

	osKernelStart ();                         // start thread execution 
	count = 0;
	while(1)
	{
		
		
		if(count == 10) {
			count = 0;
			
			p_mdl_test->getMdlData( p_mdl_test, 10000, NULL);
			mTime->getMdlData( mTime, 0, NULL);
		}
		if(hmi_count == 100)
		{
			hmi_count = 0;
//			g_p_curHmi->show(g_p_curHmi);
			
		}
		LCD_Run();
		osDelay(100);
		hmi_count ++;
		count ++;
		
	}
	
#elif TDD_MODCHANNEL == 1	
	line = 0;
	Tdd_disp_text("通道采样测试",line++, 0);
	//检测通道是否正常
	for(tdd_i = 0; tdd_i < NUM_CHANNEL; tdd_i++)
	{
		sprintf(appBuf,"chn_%d", tdd_i);
		Tdd_disp_text(appBuf,line, 0);
		p_mdl_test = ModelCreate(appBuf);
		p_mdl_test->init(p_mdl_test, NULL);
		if(p_mdl_test->self_check(p_mdl_test) == RET_OK)
		{
			Tdd_disp_text("自检成功!",line++, 200);
		}
		else 
		{
			Tdd_disp_text("自检失败!",line++, 200);
		}
	}
	
	Tdd_disp_text("采样测试...",line++, 0);
	while(1)
	{
		line = 4;
		for(tdd_i = 0; tdd_i < NUM_CHANNEL; tdd_i++)
		{
			sprintf(appBuf,"chn_%d", tdd_i);
			Tdd_disp_text("采样测试...",line, 0);
			p_mdl_test = ModelCreate(appBuf);
			if(p_mdl_test->getMdlData(p_mdl_test, AUX_DATA, &tdd_j) == RET_OK)
			{
				sprintf(appBuf,"%d", tdd_j);
				Tdd_disp_text(appBuf,line++, 200);
			}
			else
			{
				
				Tdd_disp_text("失败",line++, 200);
			}
		}
		osDelay(1000);
		
	}
#elif TDD_DEV_UART3 == 1
	line = 0;
	Tdd_disp_text("串口3测试",line++, 0);
	Dev_open(DEVID_UART3, ( void *)&I_uart3);
	
	if( I_uart3->test(I_uart3, appBuf, 64) == RET_OK)
	{
		sprintf( appBuf, "succeed!");
		Tdd_disp_text(appBuf,line++, 0);
		
	}
	else 
	{
		sprintf( appBuf, "failed!");
		Tdd_disp_text(appBuf,line++, 0);
	}
	LCD_Run();
	while(1)
	{
		
		LCD_Run();
		
	}
# elif TDD_SMART_BUS == 1
	
	line = 0;
	Tdd_disp_text("SmartBus 测试",line++, 0);
	LCD_Run();
	Dev_open(DEVID_UART3, ( void *)&I_uart3);
	while(1)
	{
		Tdd_disp_text("发送查询命令",line, 0);
		tdd_i = SmBus_build_query((uint8_t *)appBuf, 64, SMBUS_CHN_AI, 0);
		tdd_j = I_uart3->write(I_uart3, appBuf, tdd_i);
		if(tdd_j == tdd_i)
		{
			
			Tdd_disp_text("成功",line++, 260);
		}
		else 
		{
			Tdd_disp_text("失败",line++, 260);
			
		}
		LCD_Run();
		Tdd_disp_text("等待返回...",line++, 0);
		tdd_i = I_uart3->read(I_uart3, appBuf, 64);
		
		if(tdd_i)
		{
			Tdd_disp_text("解析报文",line, 0);
			SmBus_decode(SMBUS_CMD_QUERY, (uint8_t *)appBuf, &tmp_u8, 1);
			Tdd_disp_text("成功",line++, 260);
		}
		else 
		{
			Tdd_disp_text("失败",line++, 260);
			
		}
	}
	
# elif TDD_USB == 1
	
	
	
	memset(udisk_buf, '8', 512); 
	
	USB_Rgt_event_hdl(Usb_event);
	Tdd_disp_text("U盘测试",line++, 0);
	while(1)
	{
//		osDelay(100);
		
		USB_Run(NULL);
		if(tdd_finish)
		{
			continue;
		}
		if(usb_cnt == 0)
			continue;
		
		
		sprintf(appBuf, "创建文件:%s", USB_TFILE); 
		Tdd_disp_text(appBuf,2, 0);
		tdd_fd = USB_Create_file(USB_TFILE, USB_FM_READ | USB_FM_WRITE | USB_FM_COVER);
		if(tdd_fd > 0)
		{
			Tdd_disp_text("成功! ",2, 260);
			
		}
		else if(tdd_fd == ERR_ALREADY_EXIST)
		{
			
			Tdd_disp_text("已存在",2, 260);
		}
		else 
		{
			Tdd_disp_text("失败! ",2, 260);
			
		}
		line = 3;
		sprintf(appBuf, "写文件:写入8M ... "); 
		
		Tdd_disp_text(appBuf,line, 0);
		for(tdd_i = 0; tdd_i < 5000; tdd_i ++)
		{

			if(tdd_i % 1000 == 0)
			{
				sprintf(appBuf, "%03d", tdd_i); 
				Tdd_disp_text(appBuf,line, 260);
			}
			sprintf(udisk_buf, "写文件测试:[%03d]\r\n", tdd_i);
			USB_Write_file(tdd_fd, udisk_buf, strlen(udisk_buf));
//			
		}
		
//		Tdd_disp_text("完成! ", line, 260);
//		USB_Get_file_info(USB_TFILE, &usb_fin);
//		USB_flush_file(tdd_fd);
//		USB_Get_file_info_f(tdd_fd, &usb_fin);
//		USB_Colse_file(tdd_fd);

//		tdd_fd = 0;
//		
//		tdd_fd = USB_Open_file(USB_TFILE, USB_FM_READ);
//		
//		for(tdd_i = 0; tdd_i < 255; tdd_i ++)
//		{
//			memset(udisk_buf, 0, 512); 
//			USB_Read_file(tdd_fd, udisk_buf, 512);
//			
//		}
		
		line = 4;
		if(tdd_fd > 0)
		{
			Tdd_disp_text("关闭文件",line, 0);
			if(USB_Colse_file(tdd_fd) == RET_OK)
			{
				
				Tdd_disp_text("成功!",line, 260);
				tdd_finish = 1;
			}
			else
			{
				
				Tdd_disp_text("失败",line, 260);
			}
		}
	}
	
	
	
	
	
#else
#if TDD_SHEET == 1
	p_mainHmi->show( p_mainHmi);
	Set_flag_show(&p_mainHmi->flag, 1); 
	
	osKernelStart ();                         // start thread execution 
	count = 0;
	while(1)
	{
		
		
		if(count == 10) {
			count = 0;
			
			p_mdl_test->getMdlData( p_mdl_test, 10000, NULL);
			mTime->getMdlData(mTime, 0, NULL);
		}
		if(hmi_count == 50)
		{
			hmi_count = 0;
//			g_p_curHmi->show(g_p_curHmi);
			
		}
		LCD_Run();
		osDelay(100);
		hmi_count ++;
		count ++;
		
	}
	
#	elif TDD_KEYBOARD == 1
	mytxt = ( Glyph *)Get_GhTxt();
	Dev_open( LCD_DEVID, (void *)&lcd);
	lcd->Clear( COLOUR_BLACK);
	p_kbTestOb = KbTestOb_new();
	p_kbTestOb->setKeyHdl( p_kbTestOb, KeyEvent);
	u8_tmp = p_kb->addOb( p_kb, ( keyObservice *)p_kbTestOb);
	osKernelStart ();                         // start thread execution 
	while(1)
	{
		
		p_kb->run( p_kb);
		delay_ms( 200);
	}
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
  


	
}

void ThrdKeyRun (void const *argument) {
	Keyboard	*p_kb = ( Keyboard	*)argument ;
	while (1) { 

		delay_ms(CONF_KEYSCAN_CYCLEMS);		
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
	vArea_t area  = {0};
	
	lcd->Clear( COLOUR_BLACK);
	cnt.colour = COLOUR_WHITE;
	cnt.font = FONT_16;
	sprintf(buf, "key num %d ", num);
	
	cnt.data = buf;
	cnt.len = strlen( buf);
	area.x0 = 0;
	area.y0 += 0;
	mytxt->vdraw( mytxt, &cnt, &area);
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
		area.x0 = 0;
		area.y0 += 24;
		mytxt->vdraw( mytxt, &cnt, &area);
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

#if TDD_ON == 1
void 	Tdd_init(void)
{
	mytxt = ( Glyph *)Get_GhTxt();
	Dev_open( LCD_DEVID, (void *)&tdd_lcd);
	tdd_lcd->Clear( COLOUR_BLACK);
	Flush_LCD();
	LCD_Run();	//立即执行lcd指令
}
void	Tdd_disp_text(char	*text, int	line, int	row)
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
void	Tdd_disp_clean()
{
	tdd_lcd->Clear( COLOUR_BLACK);
	Flush_LCD();
}
#	if TDD_USB == 1
int	Usb_event(int type)
{
	if(type == et_ready)
	{
		usb_cnt = 1;
		tdd_finish = 0;
		Tdd_disp_text("发现U盘",1, 0);
		
	}
	else if(type == et_remove)
	{
		usb_cnt = 0;
		Tdd_disp_text("拔除U盘",1, 0);
		
	}
	
	return 0;
}

#	endif
#endif

