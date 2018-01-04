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
//#include "nokia_5110.h"
#include "device.h"

#include "TDD.h"
#include "HMI/HMIFactory.h"
#include "Usb.h"

#include "control/CtlKey.h"
#include "control/CtlTimer.h"
#include "utils/time.h"
#include "utils/keyboard.h"

#include "os/os_depend.h"

#if TDD_ON == 1
#include "ModelFactory.h"
#include "Gh_txt.h"

#endif
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
char		lcd_buf[64];
char		appBuf[512];
int			tdd_i, tdd_j, tdd_k, tdd_len, tdd_count = 0;
uint32_t		tdd_u32;
uint16_t		tdd_u16;

uint8_t		tdd_u8;
char			line = 0;
char			tdd_finish = 0;
uint8_t		tdd_err;
Glyph 		*mytxt ;
I_dev_lcd 	*tdd_lcd;
flash_t		*tdd_fsh;
uint16_t	*p_tdd_u16;
uint32_t	*p_tdd_u32;

void 	Tdd_init(void);
void	Tdd_disp_text(char	*text, int	line, int	row);
void	Tdd_disp_clean();

# if TDD_EFS == 1
int		tdd_fd;
#	endif

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
	Controller	*p_control;
//	Controller	*p_ctlTime;
//	Model 		*mTime;
	HMI 		*p_mainHmi;
//	int			ret = 0;
	short			count = 0;
	short			hmi_count = 0;
#if TDD_ON == 1
	uint8_t			tmp_u8 = 0;
#endif
	osKernelInitialize ();                    // initialize CMSIS-RTOS

  // initialize peripherals here
	
	//BSP的初始化
	OpenPrpClock();
	Pin_init();
	NVIC_Configuration();
	
	//各个外设驱动模块初始化
	System_init();

//	ret = USB_Init(NULL);
//	assert(ret == RET_OK);
	
	InitTimer( TIM2, 1000);
	clean_time_flags();
	

	
//	p_mdl_test =  ModelCreate("test");
//	p_mdl_test->init( p_mdl_test, NULL);
//	mTime = ModelCreate("time");
	//控制器初始化
	
	
		//按键初始化
	p_kb = GetKeyInsance();
	count = CONF_KEYSCAN_CYCLEMS;
	p_kb->init( p_kb, &count);
	tid_Thread_key = osThreadCreate (osThread(ThrdKeyRun), p_kb);
	p_control = SUPER_PTR( Get_CtlKey(), Controller);
	p_control->init(p_control, p_kb);

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
	
	
	p_control = SUPER_PTR(CtlTimer_new(), Controller);
	p_control->init(p_control, NULL);
	
	// create 'thread' functions that start executing,
	// example: tid_name = osThreadCreate (osThread(name), NULL);

	osKernelStart ();                         // start thread execution 
	count = 0;
	while(1)
	{
		
		
		if(count == 10) {
			count = 0;
			
//			p_mdl_test->getMdlData( p_mdl_test, 10000, NULL);
//			mTime->getMdlData( mTime, 0, NULL);
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
#elif TDD_EFS == 1

#define TEST_FILE_SIZE		64 * 1024
#define TEST_NUM_FILE		6	
	Tdd_disp_text("[TDD] Easy file system ",0, 0);
	
	Tdd_disp_text("test sys.cfg 256B",1, 0);
	tdd_j = phn_sys.fs.fs_open(0, "sys.cfg", "rw", 256);
	for(tdd_i = 0; tdd_i < sizeof(appBuf); tdd_i ++)
		appBuf[tdd_i] = tdd_i + 1;
	phn_sys.fs.fs_write(tdd_j, (uint8_t *)appBuf, 256);
	memset(appBuf, 0, sizeof(appBuf));
	phn_sys.fs.fs_read(tdd_j, (uint8_t *)appBuf,256);
	for(tdd_i = 0; tdd_i < 256; tdd_i ++)
	{
		if(appBuf[tdd_i] != ((tdd_i + 1) & 0xff))
			break;
		
	}
	if(tdd_i == 256)
		Tdd_disp_text("成功", 1, 260);
	else
		Tdd_disp_text("失败", 1, 260);
	
	for(tdd_j = 0 ; tdd_j < TEST_NUM_FILE; tdd_j ++)
	{
		sprintf(lcd_buf, "mod_chn_%d, 2MB", tdd_j);
		Tdd_disp_text(lcd_buf,2 + tdd_j, 0);
		
		sprintf(appBuf, "mod_chn_%d", tdd_j);
		tdd_fd = phn_sys.fs.fs_open(1, appBuf, "rw", TEST_FILE_SIZE);
		p_tdd_u16 = (uint16_t *)appBuf;
		for(tdd_i = 0; tdd_i < sizeof(appBuf) / 2; tdd_i ++)
			p_tdd_u16[tdd_i] = tdd_i + tdd_j * 256;
		
		for(tdd_count = 0; tdd_count < TEST_FILE_SIZE / sizeof(appBuf); tdd_count++)
			phn_sys.fs.fs_write(tdd_fd, (uint8_t *)appBuf, sizeof(appBuf));
		Tdd_disp_text("wr ok", 2 + tdd_j, 160);
	}
	for(tdd_j = 0 ; tdd_j < TEST_NUM_FILE; tdd_j ++)
	{
		sprintf(appBuf, "mod_chn_%d", tdd_j);
		tdd_fd = phn_sys.fs.fs_resize(tdd_fd, 2 * 1024 * 1024);
	}
	
	for(tdd_j = 0 ; tdd_j < TEST_NUM_FILE; tdd_j ++)
	{
		
		sprintf(appBuf, "mod_chn_%d", tdd_j);
		tdd_fd = phn_sys.fs.fs_open(1, appBuf, "rw", TEST_FILE_SIZE);
		
		
		for(tdd_count = 0; tdd_count < TEST_FILE_SIZE / sizeof(appBuf); tdd_count++)
		{
			memset(appBuf, 0xcc, sizeof(appBuf));
			tdd_len = phn_sys.fs.fs_read(tdd_fd, (uint8_t *)appBuf, sizeof(appBuf));
			p_tdd_u16 = (uint16_t *)appBuf;
			for(tdd_k = 0; tdd_k < tdd_len / 2; tdd_k ++)
			{
				if(p_tdd_u16[tdd_k] != (tdd_j * 256 + tdd_k))
				{
					Tdd_disp_text("chk err", 2 + tdd_j, 260);
					break;
				}			
			}		
		}
		if(tdd_k == sizeof(appBuf) / 2)
			Tdd_disp_text("chk ok", 2 + tdd_j, 260);
	}
		
	
	while(1);
#elif TDD_FM25 == 1
	tdd_fsh = &phn_sys.arr_fsh[FSH_FM25_NUM];
	osKernelStart (); 	
	line = 0;
	Tdd_disp_text("FM25 读写测试",0, 0);
	
	//单个字节读写测试
	tdd_j = tdd_fsh->fnf.total_pagenum * tdd_fsh->fnf.page_size;
	for(tdd_i = 0; tdd_i <= tdd_j; tdd_i += 2)
	{
		//对每个寄存器写入地址一样的值
		tdd_u16 = tdd_i;
		tdd_fsh->fsh_write((uint8_t *)&tdd_u16, tdd_i, 2);
		tdd_u16 = ~tdd_i;
		tdd_fsh->fsh_read((uint8_t *)&tdd_u16, tdd_i, 2);
		sprintf(lcd_buf, "check addr %d", tdd_i);
		Tdd_disp_text(lcd_buf,1, 0);
		if(tdd_u16 == tdd_i)
		{
			Tdd_disp_text("成功", 1, 160);
			
		}
		else
		{
			Tdd_disp_text("失败", 1, 160);
			break;
		}
	}
	
	tdd_err = 0;
	for(tdd_j = 0; tdd_j <= 0xff; tdd_j ++)
	{
		line = 1;
		sprintf(lcd_buf,"读写值=[%d]", tdd_j);
		Tdd_disp_text(lcd_buf, 3, 0);
		
		
	
		
		//写入测试数据
		memset(appBuf, tdd_j, sizeof(appBuf));
		for(tdd_i = 0; tdd_i < tdd_fsh->fnf.total_pagenum; tdd_i++)
		{
			sprintf(lcd_buf,"写页面%d", tdd_i);
			Tdd_disp_text(lcd_buf, 4, 0);
			
			 if( tdd_fsh->fsh_write((uint8_t *)appBuf, tdd_i * tdd_fsh->fnf.page_size,tdd_fsh->fnf.page_size) != tdd_fsh->fnf.page_size)
			 {
				Tdd_disp_text("失败",4, 160);
				 tdd_err = 1;
			 }
			 else
			 {
				 Tdd_disp_text("成功",4, 160);
			 }
			 osDelay(10);
			 
			 
			
		}
		
		//读取数据并与写入数据进行对比
		for(tdd_i = 0; tdd_i < tdd_fsh->fnf.total_pagenum; tdd_i++)
		{
			sprintf(lcd_buf,"读页面%d", tdd_i);
			Tdd_disp_text(lcd_buf, 5, 0);
			memset(appBuf, ~tdd_j, sizeof(appBuf));
			if( tdd_fsh->fsh_read((uint8_t *)appBuf, tdd_i * tdd_fsh->fnf.page_size,tdd_fsh->fnf.page_size) != tdd_fsh->fnf.page_size)
			{
				Tdd_disp_text("失败", 5, 160);
				tdd_err = 1;
				break;
			}
			 else
			 {
				 Tdd_disp_text("成功", 5, 160);
			 }
			
			osDelay(10);
			
			//逐个字节比较读取与写入的值是否一样
			 sprintf(lcd_buf,"检查页面%d", tdd_i);
			Tdd_disp_text(lcd_buf, 6, 0);
			for(tdd_count = 0; tdd_count < tdd_fsh->fnf.page_size; tdd_count ++)
			{
				if(appBuf[tdd_count] != tdd_j)
				{
					
					sprintf(lcd_buf,"Pg[%d][%d]:%02xh!=%02xh", tdd_i, tdd_count, appBuf[tdd_count], tdd_j);
					Tdd_disp_text(lcd_buf, 6, 160);
					tdd_err = 1;
					break;
				}
				
			}
			
			
			
			
		}
//		Tdd_disp_text("done!",line, 280);
		line ++;
	}
	
	Tdd_disp_text("FM25 读写测试结束",7, 0);
	if(tdd_err)
	{
		Tdd_disp_text("失败",7, 160);
	}
	else
	{
		Tdd_disp_text("成功",7, 160);
	}
	while(1);

#elif TDD_W25Q == 1
	#define TEST_PAGES			16
	tdd_fsh = &phn_sys.arr_fsh[FSH_W25Q_NUM];
	osKernelStart (); 	
	line = 0;
	
	tdd_fsh->fsh_info(&tdd_fsh->fnf);
	Tdd_disp_text("W25Q 读写测试",line++, 0);
	tdd_err = 0;
	
	
	//单个字节读写测试
	tdd_j = tdd_fsh->fnf.total_pagenum * tdd_fsh->fnf.page_size;
	
	tdd_fsh->fsh_ersse(FSH_OPT_CHIP, 0);
	tdd_u8 = 0;
	
	//字节读写测试，非常耗时，运行一次就够了
//	for(tdd_i = 65536; tdd_i <= tdd_j; tdd_i += 4)
//	{
//		//对每个寄存器写入地址一样的值
//		tdd_u32 = tdd_i;
//		tdd_fsh->fsh_write((uint8_t *)&tdd_u32, tdd_i, 4);
//		tdd_u32 = ~tdd_i;
//		tdd_fsh->fsh_read((uint8_t *)&tdd_u32, tdd_i, 4);
//		
//		if(tdd_u32 != tdd_i)
//		{
//			Tdd_disp_text("失败", 1, 160);
//			
//			break;
//		}
//		if(tdd_i % 10000 == 0)
//		{
//			sprintf(lcd_buf, "check addr %d", tdd_i);
//			Tdd_disp_text(lcd_buf,1, 0);
//			
//		}
//	}
	
	if(tdd_i > tdd_j)
	{
		sprintf(lcd_buf, "check addr %d", tdd_i);
		Tdd_disp_text(lcd_buf,1, 0);
		Tdd_disp_text("成功", 1, 160);
	}

	for(tdd_j = 0; tdd_j <= 0xff; tdd_j ++)
	{
		line = 1;
		sprintf(lcd_buf,"读写值=[%d]", tdd_j);
		Tdd_disp_text(lcd_buf,2, 0);
		
		Tdd_disp_text("擦除W25Q",3, 0);
		
		
		if(tdd_fsh->fsh_ersse(FSH_OPT_SECTOR, 0) == RET_OK)
			Tdd_disp_text("成功",3, 160);
		else
			Tdd_disp_text("失败",3, 160);
			
		
		
		//写入测试数据
		memset(appBuf, tdd_j, sizeof(appBuf));
		for(tdd_i = 0; tdd_i < TEST_PAGES; tdd_i++)
		{
			sprintf(lcd_buf,"写页%d", tdd_i);
			Tdd_disp_text(lcd_buf, 4, 0);
			if( tdd_fsh->fsh_write((uint8_t *)appBuf, tdd_i * tdd_fsh->fnf.page_size,tdd_fsh->fnf.page_size) != tdd_fsh->fnf.page_size)
			{
				
				tdd_err = 1;
				Tdd_disp_text("失败",4, 160);
				goto w25q_err;
			}
			else
				Tdd_disp_text("成功",4, 160);
			 
			 
			
		}
		
		//读取数据并与写入数据进行对比
		for(tdd_i = 0; tdd_i < TEST_PAGES; tdd_i++)
		{
			
			sprintf(lcd_buf,"读页%d", tdd_i);
			Tdd_disp_text(lcd_buf, 5, 0);
			memset(appBuf, ~tdd_j, sizeof(appBuf));
			
			if( tdd_fsh->fsh_read((uint8_t *)appBuf, tdd_i * tdd_fsh->fnf.page_size,tdd_fsh->fnf.page_size) != tdd_fsh->fnf.page_size)
			{
					
					tdd_err = 1;
					Tdd_disp_text("失败",5, 160);
					goto w25q_err;
			}
			else
				Tdd_disp_text("成功",5, 160);
			
			//逐个字节比较读取与写入的值是否一样
			sprintf(lcd_buf,"检查页%d", tdd_i);
			Tdd_disp_text(lcd_buf,6, 0);
			for(tdd_count = 0; tdd_count < tdd_fsh->fnf.page_size; tdd_count ++)
			{
				if(appBuf[tdd_count] != tdd_j)
				{
					
					sprintf(appBuf,"P[%d][%d]: %02xh!=%02xh", tdd_i, tdd_count, appBuf[tdd_count], tdd_j);
						Tdd_disp_text(appBuf,6, 160);
						tdd_err = 1;
						break;
				}
				
			}
			
			w25q_err:
			if(tdd_err)
			{
				Tdd_disp_text("失败",2, 160);
			}
			else
			{
				Tdd_disp_text("成功",2, 160);
			}
			
			
		}
//		Tdd_disp_text("done!",line, 280);
	}
	
	Tdd_disp_text("W25Q 读写测试结束",7, 0);
	if(tdd_err)
	{
		Tdd_disp_text("失败",7, 160);
	}
	else
	{
		Tdd_disp_text("成功",7, 160);
	}
	while(1);	
#elif TDD_MODCHANNEL == 1
	osKernelStart (); 	
	line = 0;
	Tdd_disp_text("通道采样测试",line++, 0);
	//检测通道是否正常
	for(tdd_i = 0; tdd_i < NUM_CHANNEL; tdd_i++)
	{
		sprintf(appBuf,"chn_%d", tdd_i);
		Tdd_disp_text(appBuf,line, 0);
		p_mdl_test = ModelCreate(appBuf);
		if(p_mdl_test->self_check(p_mdl_test) == RET_OK)
		{
			//设置信号类型为pt100
			tdd_u8 = AI_Pt100;
			if(p_mdl_test->setMdlData(p_mdl_test,AUX_SIGNALTYPE, &tdd_u8) == RET_OK)
			{
				Tdd_disp_text("设置信号类型成功",line, 60);
				
			}
			p_mdl_test->to_string(p_mdl_test, AUX_SIGNALTYPE, appBuf);
			Tdd_disp_text(appBuf,line++, 220);
			
		}
		else 
		{
			Tdd_disp_text("自检失败!",line++, 200);
		}
		osDelay(100);
	}
	
	Tdd_disp_text("通道采样",7, 100);
	while(1)
	{
		line = 8;
		tdd_count ++;
		
		for(tdd_i = 0; tdd_i < NUM_CHANNEL; tdd_i++)
		{
			
			sprintf(appBuf,"chn_%d", tdd_i);
			p_mdl_test = ModelCreate(appBuf);
			p_mdl_test->run(p_mdl_test);
			
			sprintf(appBuf,"[%04d] chn_%d", tdd_count, tdd_i);
			Tdd_disp_text(appBuf,line, 0);
			
			tdd_j = p_mdl_test->getMdlData(p_mdl_test, AUX_SIGNALTYPE, NULL);
			p_mdl_test->to_string(p_mdl_test, AUX_SIGNALTYPE, appBuf);
			Tdd_disp_text(appBuf,line, 150);
			tdd_j = p_mdl_test->getMdlData(p_mdl_test, AUX_DATA, NULL);
			
			
			sprintf(appBuf,"%xh", tdd_j);
			Tdd_disp_text(appBuf,line++, 250);
			
			
			
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
	osKernelStart (); 
	Tdd_disp_text("SmartBus 测试",line++, 0);
	Dev_open(DEVID_UART3, ( void *)&I_uart3);
	tdd_j = 0;
	while(1)
	{
		line = 2;
		
		sprintf(appBuf,"[%04d]:发送查询通道%d", tdd_count++,tdd_j);
		Tdd_disp_text(appBuf,line, 0);
		tdd_i = SmBus_Query(SMBUS_MAKE_CHN(SMBUS_CHN_AI, tdd_j), (uint8_t *)appBuf, 64);
		if(I_uart3->write(I_uart3, appBuf, tdd_i) == RET_OK)
		{
			
			Tdd_disp_text("成功",line++, 260);
		}
		else 
		{
			Tdd_disp_text("失败",line++, 260);
			
		}
//		LCD_Run();
		Tdd_disp_text("等待返回...",line++, 0);
		tdd_i = I_uart3->read(I_uart3, appBuf, 64);
		
		if(tdd_i)
		{
			Tdd_disp_text("解析报文",line, 0);
			SmBus_decode(SMBUS_CMD_QUERY, (uint8_t *)appBuf, &tmp_u8, 1);
			if(tdd_j == tmp_u8)
				Tdd_disp_text("成功",line++, 160);
			else
			{
				
				sprintf(appBuf,"错误的通道%d", tmp_u8);
				Tdd_disp_text(appBuf,line++, 160);
			}
				
		}
		else 
		{
			Tdd_disp_text("失败",line++, 260);
			
		}
//		Tdd_disp_clean();
		tdd_j ++;
		if(tdd_j >= NUM_CHANNEL)
			tdd_j = 0;
		osDelay(1000);
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
//			mTime->getMdlData(mTime, 0, NULL);
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
//	Model 		*mTime  = ModelCreate("time");
	while (1) { 

		delay_ms(CONF_KEYSCAN_CYCLEMS);		
		p_kb->run( p_kb);
//		mTime->getMdlData(mTime, 0, NULL);
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
	tdd_lcd->open();
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
	LCD_Run();
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

