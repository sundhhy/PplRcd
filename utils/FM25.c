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
int FM25_Write_Sector_Data(uint8_t *pBuffer, uint16_t Sector_Num);
int FM25_Read_Sector_Data(uint8_t *pBuffer, uint16_t Sector_Num);
int FM25_Erase_Sector(uint32_t Sector_Number);
int FM25_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum);
int FM25_rd_data(uint8_t *pBuffer, uint32_t rd_add, uint32_t len);


static void FM25_cmd_addr(uint8_t cmd, uint16_t addr);
static int FM25_wr_enable(void);

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

	phn_sys.arr_fsh[FM25_SPI_NO].fsh_ers_sector = FM25_Erase_Sector;
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_wr_sector = FM25_Write_Sector_Data;
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_rd_sector = FM25_Read_Sector_Data;
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_write = FM25_Write;
	phn_sys.arr_fsh[FM25_SPI_NO].fsh_read = FM25_rd_data;
	
	
	FM25_info(&phn_sys.arr_fsh[FM25_SPI_NO].fnf);
		
	return RET_OK;


}


int FM25_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum)
{
	int ret = 0;
	
	FM25_wr_enable();
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
	//FM25L64 4KB = 512 * 8
	info->page_size = 512;
	info->total_pagenum = 8;
	info->block_pagenum = 0;
	info->sector_pagenum = 0;
}

void FM25_WP(int protect)
{
	uint8_t		fm25_cmd;
	
	
	FM25_Enable_CS;
	fm25_cmd = FM25CL64_WRSR;
	FM25_SPI_WRITE(&fm25_cmd, 1);
	if(protect)
	{
		fm25_cmd = FM25CL64_PROTECT;
	}
	else
	{
		fm25_cmd = FM25CL64_UNPROTECT;
	}
	FM25_SPI_WRITE(&fm25_cmd, 1);
	FM25_Disable_CS;

	
}

int FM25_Erase_Sector(uint32_t Sector_Number)
{

	return RET_OK;
	
}





int FM25_Write_Sector_Data(uint8_t *pBuffer, uint16_t Sector_Num)
{
	return 0;
   
}


int FM25_Read_Sector_Data(uint8_t *pBuffer, uint16_t Sector_Num)
{
	
	return 0;
}





//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


static int FM25_Read_status(void)
{
	uint8_t tmp_u8 = FM25CL64_RDSR;
  FM25_Enable_CS;
	
	if(FM25_SPI_WRITE(&tmp_u8, 1) != 1)
		return -1;
	if(FM25_SPI_READ(&tmp_u8, 1) != 1)
		return -1;
	FM25_Disable_CS;
  
  return tmp_u8;
}

static int FM25_wr_enable(void)
{
	int ret = ERR_DEV_FAILED;
	uint8_t cmd = FM25CL64_WREN;
	
	ret = FM25_Read_status();
	
	FM25_Enable_CS;
	if(FM25_SPI_WRITE(&cmd, 1) == 1)
		ret = RET_OK;
	FM25_Disable_CS;
	
	ret = FM25_Read_status();
	if(ret & FM25_STATUS_WEL)
		return RET_OK;
	return ERR_DEV_FAILED;
}

static void FM25_cmd_addr(uint8_t cmd, uint16_t addr)
{
	uint8_t	tmp_u8;
	
	tmp_u8 = FM25CL64_WRITE;
	FM25_SPI_WRITE(&tmp_u8, 1);
	
	tmp_u8 = (addr & 0xff00) >> 8;
	FM25_SPI_WRITE(&tmp_u8, 1);
	
	tmp_u8 = addr & 0x00ff;
	FM25_SPI_WRITE(&tmp_u8, 1);
	

}



