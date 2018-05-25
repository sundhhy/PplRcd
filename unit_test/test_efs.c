//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
/*
测试文件系统的功能：
1 文件擦除是否正确：
	测试是否会破坏相邻文件；
	测试擦擦是否把整个文件都进行了擦除，没有任何遗漏。

*/

#include "cfg_test.h"
#include "unit_test.h"
#if UNT_EFS == 1
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "test_w25q.h"

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
#define DATA_FILE_SIZE					2 * 1024 * 1024
#define NUM_DATA_FILE						6	
#define FILL_DATA								0x5a
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
//static flash_t					*tdd_fsh;
static int 							row_num;
static uint8_t*						my_buf;
static int 							my_buf_size;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static int TEFS_Fill_data(int fd, uint8_t *buf, uint32_t size);
static int TEFS_erase_area(char n, int *all_fd, uint32_t erase_size);
static void TEFS_Erase(int *arr_fd, uint32_t size);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//


int Init_test(char	*test_buf, int size)
{
	int i;
	
	my_buf = (uint8_t *)test_buf;
	my_buf_size = size;
	
	row_num = 0;
//	UNT_Disp_text("文件系统测试",0, 0);
	
//	//创建需要测试的文件,按照实际会用到的文件进行创建,但是文件的大小并不一定准确
//	phn_sys.fs.fs_open(0, "sys.cfg", "rw", 256);
//	STRG_SYS.fs.fs_open(STRG_RCD_FSH_NUM, "alm_lost_pwr", "rw", 1024);
//	STRG_SYS.fs.fs_open(STRG_RCD_FSH_NUM, "SDH_LOG.CSV", "rw", 3600);
	for(i = 0 ; i < NUM_DATA_FILE; i ++)
	{

		sprintf(test_buf, "mod_chn_%d", i);
		phn_sys.fs.fs_open(1, test_buf, "rw", DATA_FILE_SIZE);
		
		
	}
	
	
	
	
	return 0;
}

//测试文件擦除对相邻文件的影响及擦除是否完全
void Run_test(void)
{
	int fd[NUM_DATA_FILE];
	char	tmp_buf[32];
	short 	i;
	
	for(i = 0; i < NUM_DATA_FILE; i++)
	{
		sprintf(tmp_buf, "mod_chn_%d", i);
		fd[i] = phn_sys.fs.fs_open(1, tmp_buf, "rw", DATA_FILE_SIZE);
	}
	row_num = 1;
	
	UNT_Disp_text("文件擦除范围准确性测试",0, 0);
	
	//先对文件写入非0xff的内容
	memset(my_buf, FILL_DATA, my_buf_size);
	for(i = 0; i < NUM_DATA_FILE; i++)
	{
		sprintf(tmp_buf, "mod_chn_%d", i);
		
		if(TEFS_Fill_data(fd[i], my_buf, my_buf_size))
		{
			sprintf(tmp_buf,"填充文件%d 失败", i);
		}
		else
		{
			sprintf(tmp_buf,"填充文件%d 成功", i);
		}
		
		UNT_Disp_text(tmp_buf,1 + NUM_DATA_FILE, 0);
		
	}
	
	UNT_Delay_ms(5000);
	UNT_Clean_lcd();

	
	
	//擦除1个字节
	TEFS_Erase(fd, 1);
	UNT_Delay_ms(5000);
	UNT_Clean_lcd();
	
	
	//擦除一半的字节
	TEFS_Erase(fd, DATA_FILE_SIZE / 2);
	UNT_Delay_ms(5000);
	UNT_Clean_lcd();
	
	
	//擦除全部的字节
	TEFS_Erase(fd, DATA_FILE_SIZE);
	
	
	
	UNT_Delay_ms(5000);
	
	
}


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

static void TEFS_Erase(int *arr_fd, uint32_t size)
{
	int i;
	char tmp_buf[48];
	
	sprintf(tmp_buf,"擦除字节:%d", size);
	UNT_Disp_text("擦除一个字节测试", 0, 0);
	
	for(i = 0; i < NUM_DATA_FILE; i++)
	{
		if(TEFS_erase_area(i, arr_fd,size))
		{
			sprintf(tmp_buf,"%d 失败", i);
			UNT_Disp_text(tmp_buf,1 + NUM_DATA_FILE, 120);
		}
		else
		{
			
			sprintf(tmp_buf,"%d 成功", i);
			UNT_Disp_text(tmp_buf,1 + NUM_DATA_FILE, 120);
		}
		
		
		memset(my_buf, FILL_DATA, my_buf_size);
		if(TEFS_Fill_data(arr_fd[i], my_buf, my_buf_size))
		{
			sprintf(tmp_buf,"填充文件%d 失败", i);
		}
		else
		{
			sprintf(tmp_buf,"填充文件%d 成功", i);
		}
		
		UNT_Disp_text(tmp_buf,i + 1 + NUM_DATA_FILE, 0);
		
	}
	
	
}


static int TEFS_Fill_data(int fd, uint8_t* buf, uint32_t size)
{
	uint32_t addr;
	int ret = 0;


	phn_sys.fs.fs_lseek(fd, WR_SEEK_SET, 0);
	for(addr = 0; addr < DATA_FILE_SIZE; addr += size)
	{
		
		if(phn_sys.fs.fs_raw_write(fd, my_buf, size) != size)
		{
			ret = 1;
			break;
		}
		
	}

	return ret;
}

//检查某一区域内的数据是否与指定的值相等
//成功返回0
//读取错误返回1 验证错误返回2 
static int TEFS_Check_area_val(int fd, uint32_t start_addr, uint32_t end_addr, uint8_t val)
{
	uint32_t addr;
	char tmp_buf[48];
	int j;
	
	
	phn_sys.fs.fs_lseek(fd, RD_SEEK_SET, start_addr);
	
	for(addr = start_addr; addr < end_addr; addr += my_buf_size)
	{
		
		if(phn_sys.fs.fs_raw_read(fd, (uint8_t *)my_buf, my_buf_size) != my_buf_size)
		{
			
			sprintf(tmp_buf,"RDE %d", addr);
			UNT_Disp_text(tmp_buf, row_num, 0);
			return 1;
		}
		
		
		for(j = 0; j < my_buf_size; j++)
		{
			if(my_buf[j] == val)
				continue;
			
			
			sprintf(tmp_buf,"[%d] %xh != %xh", addr + j, my_buf[j], val);
			UNT_Disp_text(tmp_buf,row_num , 120);
			return 2;
			
		}
	}
	
	return 0;
}

static int TEFS_erase_area(char n, int *all_fd, uint32_t erase_size)
{
	uint8_t			buf[8];
//	int					size = 8;
	int					ret;
//	int 				i;
	
	//	与测试文件相邻的左右两边的文件
	int			ngh_fd[2] = {0, 2};		
	
	if(n == 0)
	{
		ngh_fd[0] = -1;
		ngh_fd[1] = all_fd[1];
		
	}
	else if(n == (NUM_DATA_FILE - 1))
	{
		
		ngh_fd[0] = all_fd[n - 1];
		ngh_fd[1] = -1;
	}
	else
	{
		
		ngh_fd[0] = all_fd[n - 1];
		ngh_fd[1] = all_fd[n + 1];
		
	}
	
	if(erase_size < DATA_FILE_SIZE - 2)		//文件内部判断是否越界即可
	{
		phn_sys.fs.fs_erase_file(all_fd[n], 2, erase_size);		
		
		//检查擦除部分是不是0xff
		//前后边界都要检查一下
		row_num = 1 + n;
		ret = TEFS_Check_area_val(all_fd[n], 2, 2 + erase_size, 0xff);
		if(ret != 0)
			return ret;
		
		//检查前边界
		phn_sys.fs.fs_lseek(all_fd[n], RD_SEEK_SET, 0);
		phn_sys.fs.fs_raw_read(all_fd[n], buf, 2);
		if(buf[0] != FILL_DATA)
			return 1;
		if(buf[1] != FILL_DATA)
			return 1;
		
		//检查后边界
		phn_sys.fs.fs_lseek(all_fd[n], RD_SEEK_SET, 2 + erase_size);
		phn_sys.fs.fs_raw_read(all_fd[n], buf, 2);
		if(buf[0] != FILL_DATA)
			return 1;
		if(buf[1] != FILL_DATA)
			return 1;
		
		
		return 0;
		
	}

	phn_sys.fs.fs_erase_file(all_fd[n], 0, DATA_FILE_SIZE);	

	row_num = 1 + n;	
	ret = TEFS_Check_area_val(all_fd[n], 0, DATA_FILE_SIZE, 0xff);
	if(ret != 0)
		return ret;
		
	//检查对相邻文件是否产生了破坏


		//检查前一个文件的尾部
		if(ngh_fd[0] >= 0)
		{
			
			phn_sys.fs.fs_lseek(all_fd[n], RD_SEEK_SET, DATA_FILE_SIZE - 2);
			phn_sys.fs.fs_raw_read(all_fd[n], buf, 2);
			if(buf[0] != FILL_DATA)
				return 1;
			if(buf[1] != FILL_DATA)
				return 1;
			
			
		}
			
		
		//检查后一个文件的头部
		{
			
			phn_sys.fs.fs_lseek(all_fd[n], RD_SEEK_SET, 0);
			phn_sys.fs.fs_raw_read(all_fd[n], buf, 2);
			if(buf[0] != FILL_DATA)
				return 1;
			if(buf[1] != FILL_DATA)
				return 1;
			
		}
	
		return 0;
			
	
}

#endif
