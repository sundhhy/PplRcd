/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
#include "TDD.h"

#if TDD_ON == 1

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




//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

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
#define 	USB_TFILE							"/CHN_0.CSV"

#define TDD_MDLCHN_SINGAL 			AI_Pt100
#define TDD_MDLCHN_NUM_CHN 			6

#define GITP_RISINGEDGE					0			
#define GITP_FAILINGEDGE				1	

//-------TDD_EFS----------------------------------
#define TEST_FILE_SIZE					64 * 1024
#define TEST_NUM_FILE						6	

//------TDD_W25Q---------------------------------
#define TEST_PAGES							16
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static I_dev_Char 			*I_uart2;
static I_dev_Char 			*I_uart3;
static KbTestOb 				*p_kbTestOb;
static I_dev_lcd 				*lcd;
static I_dev_Char 			*p_devGpio[5];
static Model						*p_mdl_test;

static Glyph 						*mytxt ;
static I_dev_lcd 				*tdd_lcd;
static flash_t					*tdd_fsh;
static uint16_t					*p_tdd_u16;
static uint32_t					*p_tdd_u32;

static uint16_t					arr_u16[2];
static char							lcd_buf[64];
static char							appBuf[512];
static char							udisk_buf[512];

static do_out_t					tdd_do;
static USB_file_info		usb_fin;

static int							tdd_fd;
static int							tdd_fd;
static int							tdd_i, tdd_j, tdd_k, tdd_len, tdd_count = 0;
static uint32_t					tdd_u32;
static uint16_t					tdd_u16;
static short						usb_cnt = 0;
static uint8_t					tdd_u8;
static char							line = 0;
static char							tdd_finish = 0;
static uint8_t					tdd_err;
static uint8_t 					keyval;


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static int		Usb_event(int type);
static int 		KeyEvent( char num, keyMsg_t arr_msg[]);
static void 	GpioIrqHdl( void *self, int type, int encode);

static void		Tdd_disp_text(char	*text, int	line, int	row);
static void		Tdd_disp_clean(void);
static void 	TDD_efs_erase(int erase_size, int lcd_line);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
void 	Tdd_Init(void)
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

void TDD_Time_sec(void)
{
	struct  tm	t = {0};
	uint32_t	s = 0;
	Tdd_disp_text("[TDD]秒值与时间的转换测试",0, 0);
	sprintf(appBuf, "18/01/28 09:46:05");
	Tdd_disp_text("测试时间: ",1, 0);
	Tdd_disp_text(appBuf, 1, 100);
	s = Str_time_2_u32(appBuf);
	sprintf(lcd_buf, "sec = %x", s);
	Tdd_disp_text(lcd_buf, 2, 0);
	Sec_2_tm(s, &t);
	
	sprintf(lcd_buf, "tm %02d/%02d/%02d %02d:%02d:%02d", t.tm_year,t.tm_mon, t.tm_mday, \
				t.tm_hour, t.tm_min, t.tm_sec);
	Tdd_disp_text(lcd_buf, 3, 0);
	while(1);
}


void TDD_Efs(void)
{
	
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
		sprintf(lcd_buf, "mod_chn_%d, %dKB", tdd_j, TEST_FILE_SIZE/1024);
		Tdd_disp_text(lcd_buf,2 + tdd_j, 0);

		sprintf(appBuf, "mod_chn_%d", tdd_j);
		tdd_fd = phn_sys.fs.fs_open(1, appBuf, "rw", TEST_FILE_SIZE);
		phn_sys.fs.fs_lseek(tdd_fd, WR_SEEK_SET, 0);
		p_tdd_u16 = (uint16_t *)appBuf;
		for(tdd_i = 0; tdd_i < sizeof(appBuf) ; tdd_i ++)
			appBuf[tdd_i] = tdd_j + 1;

		tdd_err = 0;
		for(tdd_count = 0; tdd_count < TEST_FILE_SIZE / sizeof(appBuf); tdd_count++)
			if(phn_sys.fs.fs_write(tdd_fd, (uint8_t *)appBuf, sizeof(appBuf)) < 0)
			{
				Tdd_disp_text("wr failed", 2 + tdd_j, 160);
				tdd_err = 1;
				break;
			}

		if(tdd_err == 0)
			Tdd_disp_text("wr ok", 2 + tdd_j, 160);
		phn_sys.fs.fs_close(tdd_fd);
	}
//	for(tdd_j = 0 ; tdd_j < TEST_NUM_FILE; tdd_j ++)
//	{
//		sprintf(appBuf, "mod_chn_%d", tdd_j);
//		tdd_fd = phn_sys.fs.fs_resize(-1, appBuf, 2 * 1024 * 1024);
//	}

	for(tdd_j = 0 ; tdd_j < TEST_NUM_FILE; tdd_j ++)
	{

		sprintf(appBuf, "mod_chn_%d", tdd_j);
		tdd_fd = phn_sys.fs.fs_open(1, appBuf, "rw", TEST_FILE_SIZE);

		tdd_err = 0;
		for(tdd_count = 0; tdd_count < TEST_FILE_SIZE / sizeof(appBuf); tdd_count++)
		{
			memset(appBuf, 0xcc, sizeof(appBuf));
			tdd_len = phn_sys.fs.fs_read(tdd_fd, (uint8_t *)appBuf, sizeof(appBuf));
			p_tdd_u16 = (uint16_t *)appBuf;
			for(tdd_k = 0; tdd_k < tdd_len ; tdd_k ++)
			{
				if(appBuf[tdd_k] != (tdd_j + 1))
				{
					Tdd_disp_text("chk err", 2 + tdd_j, 260);
					tdd_err = 1;
					break;
				}			
			}		
		}
		if(tdd_err == 0)
		Tdd_disp_text("chk ok", 2 + tdd_j, 260);
	}

	if(TEST_NUM_FILE != 6)
		while(1);		//擦除测试建立在测试文件有6个的前提之下的
	Tdd_disp_text("文件擦除测试", 8, 0);
	
	//在之前的读写测试基础上进行测试
	//取用中间的一个文件进行测试，这样既可以测试擦除功能
	//也可以测试对擦除对其他文件是否有影响
	//测试是是针对w25q进行的，因此扇区，块大小都是按照w25q进行的
	
	
	//测试小于一个扇区文件的擦除功能
	TDD_efs_erase(1025, 9);
	//测试一个扇区大小的擦除
	TDD_efs_erase(4096, 10);
	//测试1个块大小的擦除
	TDD_efs_erase(65536, 11);
	//测试组合擦除
	TDD_efs_erase(65536 + 4096 + 1025, 12);

	Tdd_disp_text("删除测试文件", 13, 0);	
	for(tdd_j = 0 ; tdd_j < TEST_NUM_FILE; tdd_j ++)
	{

		sprintf(appBuf, "mod_chn_%d", tdd_j);
		phn_sys.fs.fs_delete(-1, appBuf);
	}

	while(1);
	
}

void TDD_Fm25(void)
{
	
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
			if(tdd_fsh->fsh_read((uint8_t *)appBuf, tdd_i * tdd_fsh->fnf.page_size,tdd_fsh->fnf.page_size) != tdd_fsh->fnf.page_size)
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
	
}


void TDD_W25q(void)
{
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

	
//	if(tdd_i > tdd_j)
//	{
//		sprintf(lcd_buf, "check addr %d", tdd_i);
//		Tdd_disp_text(lcd_buf,1, 0);
//		Tdd_disp_text("成功", 1, 160);
//	}

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
	
}


void TDD_Mdl_chn(void)
{
	int16_t	set_val ;
	osKernelStart (); 	
	line = 0;
	Tdd_disp_text("通道采样测试",line++, 0);
	//检测通道是否正常
	for(tdd_i = 0; tdd_i < TDD_MDLCHN_NUM_CHN; tdd_i++)
	{
		sprintf(appBuf,"chn_%d", tdd_i);
		Tdd_disp_text(appBuf,line, 0);
		p_mdl_test = Create_model(appBuf);
		tdd_u8 = tdd_i;
		p_mdl_test->init(p_mdl_test, &tdd_u8);
		//		if(p_mdl_test->self_check(p_mdl_test) == RET_OK)
		{
			//			Tdd_disp_text("自检成功!",line++, 200);

			//设置信号类型为pt100
			tdd_u8 = TDD_MDLCHN_SINGAL;
			
			p_mdl_test->setMdlData(p_mdl_test,AUX_SIGNALTYPE, &tdd_u8);
			if(p_mdl_test->setMdlData(p_mdl_test,AUX_SIGNALTYPE, &tdd_u8) != RET_OK)
			{
				Tdd_disp_text("设置信号类型失败",line, 60);
				
			}
			
			//设置 k b small_signal
			set_val = 100;
			p_mdl_test->setMdlData(p_mdl_test,chnaux_k, &set_val);
			set_val = 0;
			p_mdl_test->setMdlData(p_mdl_test,chnaux_b, &set_val);
			set_val = 0;
			p_mdl_test->setMdlData(p_mdl_test,chnaux_small_signal, &set_val);
			
			tdd_u16 = 0x1;
			p_mdl_test->setMdlData(p_mdl_test,chnaux_lower_limit, &tdd_u16);
			tdd_u16 = 0x5000;
			p_mdl_test->setMdlData(p_mdl_test,chnaux_upper_limit, &tdd_u16);

			p_mdl_test->getMdlData(p_mdl_test,chnaux_lower_limit, NULL);
			p_mdl_test->getMdlData(p_mdl_test,chnaux_upper_limit, NULL);

			p_mdl_test->to_string(p_mdl_test, chnaux_lower_limit, lcd_buf);
			Tdd_disp_text(lcd_buf,line, 60);

			p_mdl_test->to_string(p_mdl_test, chnaux_upper_limit, lcd_buf);
			Tdd_disp_text(lcd_buf,line, 160);

			p_mdl_test->getMdlData(p_mdl_test,AUX_SIGNALTYPE, NULL);
			p_mdl_test->to_string(p_mdl_test, AUX_SIGNALTYPE, appBuf);
			Tdd_disp_text(appBuf,line++, 220);

		}
		//		else 
		//		{
		//			Tdd_disp_text("自检失败!",line++, 200);
		//		}
		osDelay(100);
	}

	Tdd_disp_text("通道采样",7, 100);
	while(1)
	{
		line = 8;
		tdd_count ++;

		for(tdd_i = 0; tdd_i < TDD_MDLCHN_NUM_CHN; tdd_i++)
		{
			sprintf(appBuf,"chn_%d", tdd_i);
			
			
			p_mdl_test = Create_model(appBuf);
			p_mdl_test->run(p_mdl_test);

			sprintf(appBuf,"[%04d] chn_%d", tdd_count, tdd_i);
			Tdd_disp_text(appBuf,line, 0);

			if(p_mdl_test->getMdlData(p_mdl_test, AUX_DATA, &tdd_j) == RET_OK)
			{
				sprintf(appBuf,"%05d", tdd_j);
				Tdd_disp_text(appBuf,line, 120);
			}
			else
			{
				Tdd_disp_text("err",line, 120);
			}

//			tdd_do.do_chn = tdd_i;
//			tdd_do.val = tdd_count & 1;
//			p_mdl_test->setMdlData(p_mdl_test, DO_output, &tdd_do);

			if(tdd_i == 0)
			{
				sprintf(appBuf,"CET:%05d", phn_sys.code_end_temperature);
				Tdd_disp_text(appBuf,line, 180);

			}
			line ++;
		}
		osDelay(1000);
	}
}

void TDD_Uart_3(void)
{
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
	while(1)
	{
		LCD_Run();
	}
}

void TDD_Uart_2(void)
{
	line = 0;
	Tdd_disp_text("串口2测试",line++, 0);
	Dev_open( DEVID_UART2, ( void *)&I_uart2);

	if( I_uart2->test(I_uart3, appBuf, 64) == RET_OK)
	{
		sprintf( appBuf, "succeed!");
		Tdd_disp_text(appBuf,line++, 0);
	}
	else 
	{
		sprintf( appBuf, "failed!");
		Tdd_disp_text(appBuf,line++, 0);
	}
	while(1)
	{
		LCD_Run();
	}
}

void TDD_Smart_bus(void)
{
	uint8_t tmp_u8;
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
	
}


void TDD_Usb(void)
{
	assert(USB_Init(NULL) == RET_OK);
	memset(udisk_buf, '8', 512); 

	USB_Rgt_event_hdl(Usb_event);
	Tdd_disp_text("U盘测试",line++, 0);
	while(1)
	{
		//		osDelay(100);

		USB_Run(0);
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
}

void TDD_Sheet(void)
{
	short 	hmi_count, count;
	HMI 		*p_mainHmi;
	
	p_mainHmi = Create_HMI( HMI_MAIN);
	p_mainHmi->init( p_mainHmi, NULL);
	
	p_mainHmi->show( p_mainHmi);
	Set_flag_show(&p_mainHmi->flag, 1); 

	osKernelStart ();                         // start thread execution 
	count = 0;
	while(1)
	{


		if(count == 10) 
		{
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

}

void TDD_Keyboard(void)
{
	Keyboard	*p_kb;
	
	mytxt = ( Glyph *)Get_GhTxt();
	Dev_open( LCD_DEVID, (void *)&lcd);
	lcd->Clear( COLOUR_BLACK);
	p_kbTestOb = KbTestOb_new();
	if(p_kbTestOb == NULL)
		while(1);
	
	p_kbTestOb->setKeyHdl( p_kbTestOb, KeyEvent);
	tdd_u8 = p_kb->addOb( p_kb, ( keyObservice *)p_kbTestOb);
	osKernelStart ();                         // start thread execution 
	while(1)
	{
		p_kb->run( p_kb);
		delay_ms( 200);
	}
}

void TDD_Gpio(void)
{
	for( tdd_u8 = 0; tdd_u8 < 5; tdd_u8 ++)
	{
		Dev_open( DEVID_GPIO_D0 + tdd_u8, ( void *)&p_devGpio[tdd_u8]);
		p_devGpio[tdd_u8]->ioctol( p_devGpio[tdd_u8], DEVCMD_SET_IRQHDL, GpioIrqHdl, (void *)p_devGpio[tdd_u8]);
		p_devGpio[tdd_u8]->ioctol( p_devGpio[tdd_u8], DEVGPIOCMD_SET_ENCODE, tdd_u8);
	}
	while(1)
	{
		printf("\n key val: 0x%x\n", keyval);
	}
}

void TDD_Mvc(void)
{

	
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{




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



static  void GpioIrqHdl( void *self, int type, int encode)
{
	if( type == GITP_FAILINGEDGE)
		keyval |= ( 1 << encode);
	else if( type == GITP_RISINGEDGE)
		keyval &= ~( 1 << encode);
	
}









static void	Tdd_disp_text(char	*text, int	line, int	row)
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
static void	Tdd_disp_clean()
{
	tdd_lcd->Clear( COLOUR_BLACK);
	Flush_LCD();
	LCD_Run();
}

static int	Usb_event(int type)
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


static void TDD_efs_erase(int erase_size, int lcd_line)
{
	uint8_t			*p_efs_u8 = (uint8_t *)appBuf;
	
	//	与测试文件相邻的左右两边的文件
	uint8_t			ngh_file[2] = {0, 2};;		
	
	if(erase_size > TEST_FILE_SIZE)
	{
		phn_sys.fs.fs_delete(-1, "mod_chn_2");
		ngh_file[1] = 3;
	}
	if(erase_size > 2 * TEST_FILE_SIZE)
	{
		phn_sys.fs.fs_delete(-1, "mod_chn_3");
		ngh_file[1] = 4;
	}
	if(erase_size > 3 * TEST_FILE_SIZE)
	{
		phn_sys.fs.fs_delete(-1, "mod_chn_4");
		ngh_file[1] = 5;
	}
	if(erase_size > 4 * TEST_FILE_SIZE)
	{
		phn_sys.fs.fs_delete(-1, "mod_chn_5");
		ngh_file[1] = 0xff;
	}
	
	tdd_fd = phn_sys.fs.fs_resize(-1, "mod_chn_1", erase_size);	
	phn_sys.fs.fs_erase_file(tdd_fd, 0, 0);
	tdd_err = 0;
	for(tdd_i = 0; tdd_i < erase_size;)
	{
		memset(appBuf, 0xcc, sizeof(appBuf));
		tdd_len = phn_sys.fs.fs_read(tdd_fd, p_efs_u8, sizeof(appBuf));
		for(tdd_k = 0; tdd_k < tdd_len ; tdd_k ++)
		{
			if(p_efs_u8[tdd_k] != 0xff)
			{
				tdd_err = 1;
				break;
			}			
		}
		tdd_i += tdd_len;	
		
		
	}
	if(tdd_err == 0)
	{
		sprintf(lcd_buf, "er %d ok!", erase_size); 		
	}
	else
	{
		sprintf(lcd_buf, "er %d err!", erase_size); 
	}
	Tdd_disp_text(lcd_buf, lcd_line, 0);
	
	//检查对相邻文件是否产生了破坏
	for(tdd_i = 0; tdd_i < 2; tdd_i++)
	{
		tdd_err = 0;
		tdd_j = ngh_file[tdd_i];
		if(tdd_j > 6)
			continue;
		sprintf(appBuf, "mod_chn_%d", tdd_j);
		tdd_fd = phn_sys.fs.fs_open(1, appBuf, "rw", TEST_FILE_SIZE);
		tdd_err = 0;
		for(tdd_count = 0; tdd_count < TEST_FILE_SIZE / sizeof(appBuf); tdd_count++)
		{
			memset(appBuf, 0xcc, sizeof(appBuf));
			tdd_len = phn_sys.fs.fs_read(tdd_fd, (uint8_t *)appBuf, sizeof(appBuf));
			p_tdd_u16 = (uint16_t *)appBuf;
			for(tdd_k = 0; tdd_k < tdd_len ; tdd_k ++)
			{
				if(appBuf[tdd_k] != (tdd_j + 1))
				{
					tdd_err = 1;
					break;
				}			
			}		
		}
		if(tdd_err == 0)
		{
			sprintf(lcd_buf, "ngh %d ok!", tdd_j); 		
		}
		else
		{
			sprintf(lcd_buf, "ngh %d err!", tdd_j); 
		}
		Tdd_disp_text(lcd_buf, lcd_line, (tdd_i + 1) *100);
	}
	
	
}

#endif //TDD_ON == 1

