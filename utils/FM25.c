/*************************************************
Copyright (C), 
File name: FM25.c
Author:		sundh
Version:	v1.0
Date: 		17-12-25
Description: 


*************************************************/



//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "FM25.h"
#include <string.h>
#include "sdhdef.h"
#include "os/os_depend.h"
#include "device.h"

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
static I_dev_Char *p_fm25_spi;

//----------------- 移植时需要修改的接口 -----------------
#define FM25_SPI_DEVID				DEVID_SPI2
#define FM25_SPI_NO					FSH_FM25_NUM

#define FM25_DELAY_MS(ms)			delay_ms(ms)	
#define FM25_Enable_CS          	GPIO_ResetBits(PORT_FM25_nCS, PIN_FM25_nCS)
#define FM25_Disable_CS         	GPIO_SetBits(PORT_FM25_nCS, PIN_FM25_nCS)      	
#define FM25_SPI_WRITE(buf, len)		p_fm25_spi->write(p_fm25_spi, buf, len)
#define FM25_SPI_READ(buf, len)			p_fm25_spi->read(p_fm25_spi, buf, len)


//-----------------------------------------------------------

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

 
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------



void FM25_WP(int protect);
void FM25_info(fsh_info_t *info);
void FM25_Erase_addr(uint32_t s, uint32_t size);
int FM25_Erase(int opt, uint32_t Sector_Number);
int FM25_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum);
int FM25_rd_data(uint8_t *pBuffer, uint32_t rd_add, uint32_t len);
void FM25_Flush(void);
int	FM25_lock(uint32_t ms);
int	FM25_unlock(void);

static void FM25_cmd_addr(uint8_t cmd, uint16_t addr);
static int FM25_wr_enable(void);
static int FM25_Write_status(uint8_t s);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int FM25_init(void)
{
	if(phn_sys.arr_fsh[FM25_SPI_NO].fnf.fnf_flag == 0)
	{
		
			
		Dev_open(FM25_SPI_DEVID, (void *)&p_fm25_spi);
		FM25_Disable_CS;
		
		phn_sys.arr_fsh[FM25_SPI_NO].fnf.fnf_flag = 1;
		
		
	}
	
	
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_info = FM25_info;
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_wp = FM25_WP;

	phn_sys.arr_fsh[FM25_SPI_NO].fsh_ersse = FM25_Erase;
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_ersse_addr = FM25_Erase_addr;

	phn_sys.arr_fsh[FM25_SPI_NO].fsh_write = FM25_Write;
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_raw_write = FM25_Write;
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_raw_read = FM25_rd_data;
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_read = FM25_rd_data;
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_flush = FM25_Flush;
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_lock = FM25_lock;
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_unlock = FM25_unlock;
	
	
	FM25_info(&phn_sys.arr_fsh[FM25_SPI_NO].fnf);
		
	return RET_OK;


}


int FM25_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum)
{
	int ret = 0;
	
//	FM25_Write_status(0x80);
	
	if(FM25_wr_enable() != RET_OK)
		return 0;
	FM25_Enable_CS;
	
	FM25_cmd_addr(FM25CL64_WRITE,  WriteAddr);
	ret = FM25_SPI_WRITE(pBuffer, WriteBytesNum);
	FM25_Disable_CS;
	
	return ret;
 
}

int FM25_rd_data(uint8_t *pBuffer, uint32_t rd_add, uint32_t len)
{
	int ret = 0;
	FM25_Enable_CS;
	FM25_cmd_addr(FM25CL64_READ,  rd_add);
	ret = FM25_SPI_READ(pBuffer, len);
	FM25_Disable_CS;
	
	return ret;
	
}

///这个函数要在w25q_init成功之后调用才有用
void FM25_info(fsh_info_t *info)
{
	//FM25L64 4Kb = 512 * 8
#if FM25_DEVTYPE == FM25L04B
	info->page_size = 64;
	info->total_pagenum = 8;
#else
	info->page_size = 64;
	info->total_pagenum = 128;
#endif
	info->num_sct = 0;
	info->num_blk = 0;

}

void FM25_WP(int protect)
{
	uint8_t		fm25_cmd;
	
	
	
	if(protect)
	{
		
		fm25_cmd = FM25CL64_PROTECT;
	}
	else
	{
		fm25_cmd = FM25CL64_UNPROTECT;
	}
	FM25_Write_status(fm25_cmd);

	
}

void FM25_Flush(void)
{
	
}

void FM25_Erase_addr(uint32_t s, uint32_t size)
{
	short		wr_size = 	size;		
	uint8_t		retry = 5;
	uint8_t		tmp = 0xff;


	while( wr_size > 0)
	{
		if(FM25_Write(&tmp, s, 1)== 1)
		{
			s ++;
			wr_size --;
			retry = 5;
			continue;
		}
		
		if(retry)
		{
			retry --;
		}
		else
		{
			s ++;
			wr_size --;
			retry = 5;
			
		}
		
	
	}
		
	
}

int FM25_Erase(int opt, uint32_t num)
{

	int ret = ERR_DEV_FAILED;
	fsh_info_t 	info;
	uint32_t 		addr = 0;
	uint32_t		total;
	uint8_t		tmp_buf[4];

	switch(opt)
	{
		
		case FSH_OPT_SECTOR:
			
			break;
		case FSH_OPT_BLOCK:
			
			break;
		case FSH_OPT_CHIP:
			FM25_info(&info);
			memset(tmp_buf, 0xff, 4);
			ret = RET_OK;
			total = info.total_pagenum * info.page_size / 4;
			for(addr = 0; addr < total; addr += 4)
			{
				if( FM25_Write(tmp_buf, 4 * addr, 4)!= 4)
					ret = ERR_DEV_FAILED;
				
			}
			
			break;
		default:
			break;
		
	}
	
	return ret;
	
}











//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


//static int FM25_Read_status(void)
//{
//	uint8_t tmp_u8 = FM25CL64_RDSR;
//  FM25_Enable_CS;
//	
//	if(FM25_SPI_WRITE(&tmp_u8, 1) != 1)
//		return -1;
//	if(FM25_SPI_READ(&tmp_u8, 1) != 1)
//		return -1;
//	FM25_Disable_CS;
//  
//  return tmp_u8;
//}

int	FM25_lock(uint32_t ms)
{
	return RET_OK;
	
}
int	FM25_unlock(void)
{
	return RET_OK;
	
}

static int FM25_Write_status(uint8_t s)
{
	uint8_t		cmd[2] = {FM25CL64_WRSR, 0};
	uint8_t		n = 0;
	
	cmd[1] = s;
	FM25_Enable_CS;
	n = FM25_SPI_WRITE(cmd, 2);
	FM25_Disable_CS;
	
//	FM25_Read_status();
	
	if(n == 2)
		return RET_OK;
	else
		return ERR_DEV_FAILED;

}

static int FM25_wr_enable(void)
{
	int ret = ERR_DEV_FAILED;
	uint8_t cmd = FM25CL64_WREN;
	
//	ret = FM25_Read_status();
	
	FM25_Enable_CS;
	if(FM25_SPI_WRITE(&cmd, 1) == 1)
		ret = RET_OK;
	FM25_Disable_CS;
	
//	ret = FM25_Read_status();
//	if(ret & FM25_STATUS_WEL)
	return ret;
//	return ERR_DEV_FAILED;
}

static void FM25_cmd_addr(uint8_t cmd, uint16_t addr)
{
	uint8_t	tmp_u8;
	
#if FM25_DEVTYPE == FM25L04B
	//在读写的时候，地址是9bit，其中最高的Bit放在命令的Bit3传输		
	tmp_u8 = cmd;
	if(addr & 0x100)
		tmp_u8 |= 0x8;
	FM25_SPI_WRITE(&tmp_u8, 1);
	
	tmp_u8 = addr & 0x00ff;
	FM25_SPI_WRITE(&tmp_u8, 1);
	
#elif FM25_DEVTYPE == FM25L64B
	// op-code (8-bit) | address(16 -bit ,高3位被忽视)
	tmp_u8 = cmd;
	FM25_SPI_WRITE(&tmp_u8, 1);
	
	
	tmp_u8 = (addr & 0xff00) >> 8;
	FM25_SPI_WRITE(&tmp_u8, 1);
	
	tmp_u8 = addr & 0x00ff;
	FM25_SPI_WRITE(&tmp_u8, 1);
#endif
}



