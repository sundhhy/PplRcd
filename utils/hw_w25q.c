/*************************************************
Copyright (C), 
File name: w25q.c
Author:		sundh
Version:	v1.0
Date: 		14-02-27
Description: 
w25Q的驱动程序，实现W25Q的标准SPI操作模式
使用MCU的SPI0来与w25q通讯
w25q要提供很多复杂的接口。
w25q不实现驱动适配器接口。
为了尽量减少对flash的操作，所有对flash的写操作都以扇区为单位进行。对于读取提供了随机地址读的方法。
Others: 
Function List: 
1. w25q_init
初始化flash，并获取id和容量信息
History: 
1. Date:
Author:
Modification:
2. w25q_Erase_Sector
擦除flash中的选定的某一个扇区
History: 
1. Date:
Author:
Modification:
3. w25q_Write_Sector_Data
像一个指定的扇区写入数据
History: 
1. Date:
Author:
Modification:
4. w25q_Read_Sector_Data
读取一个扇区
History: 
1. Date:
Author:
Modification:
5. w25q_rd_data
读取随机地址
History: 
1. Date:
Author:
Modification:
*************************************************/



//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "hw_w25q.h"
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

static	I_dev_Char	*w25q_spi;

///移植时需要修改的接口
#define W25Q_SPI_DEVID				DEVID_SPI2

#define W25Q_DELAY_MS(ms)			delay_ms(ms)	
#define W25Q_Enable_CS          	GPIO_ResetBits(PORT_W25Q_nCS, PIN_W25Q_nCS)
#define W25Q_Disable_CS         	GPIO_SetBits(PORT_W25Q_nCS, PIN_W25Q_nCS)  

#define W25Q_Enable_WP          	GPIO_ResetBits(PORT_FSH_nWP, PIN_FSH_nWP)
#define W25Q_Disable_WP         	GPIO_SetBits(PORT_FSH_nWP, PIN_FSH_nWP) 

#define SPI_WRITE(data, len)		w25q_spi->write(w25q_spi, data, len)
#define SPI_READ(buf, len)			w25q_spi->read(w25q_spi, buf, len)
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static w25q_instance_t  W25Q_flash;
 
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static uint8_t	W25Q_tx_buf[16];
static int w25q_wr_enable(void);
static uint8_t w25q_ReadSR(void);
static int w25q_write_waitbusy(uint8_t *data, int len);
static int w25q_read_id(void);


void W25Q_WP(int protect);

void w25q_info(fsh_info_t *info);
int w25q_erase(uint32_t offset, uint32_t len);
int w25q_Write_Sector_Data(uint8_t *pBuffer, uint16_t Sector_Num);
int w25q_Read_Sector_Data(uint8_t *pBuffer, uint16_t Sector_Num);
int w25q_Read_page_Data(uint8_t *pBuffer, uint16_t num_page);
int w25q_Erase_Sector(uint32_t Sector_Number);
int w25q_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum);
int w25q_rd_data(uint8_t *pBuffer, uint32_t rd_add, uint32_t len);
int w25q_close(void);
int w25q_Erase_chip_c7(void);
int w25q_Erase_chip_60(void);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int w25q_init(void)
{
	if(W25Q_flash.w25q_flag == 0)
	{
		
			
		Dev_open(W25Q_SPI_DEVID, (void *)&w25q_spi);
		W25Q_Disable_CS;
		
		W25Q_tx_buf[0] = 0xff;
		W25Q_flash.w25q_flag = 1;
	}
	
	
//	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_init = w25q_init;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_info = w25q_info;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_wp = W25Q_WP;

	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_ers_sector = w25q_Erase_Sector;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_wr_sector = w25q_Write_Sector_Data;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_rd_sector = w25q_Read_Sector_Data;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_write = w25q_Write;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_read = w25q_rd_data;
	
	W25Q_Enable_WP;
	return w25q_read_id();


}

///这个函数要在w25q_init成功之后调用才有用
void w25q_info(fsh_info_t *info)
{
	w25qInfo_t	*w25qinfo = ( w25qInfo_t *)info;
	
	w25qinfo->page_size = PAGE_SIZE;
	w25qinfo->total_pagenum = W25Q_flash.page_num;
	w25qinfo->sector_pagenum = SECTOR_HAS_PAGES;
	
	w25qinfo->block_pagenum = BLOCK_HAS_SECTORS * SECTOR_HAS_PAGES;

}

void W25Q_WP(int protect)
{
	
	if(protect)
		W25Q_Enable_WP;
	else
		W25Q_Disable_WP;
}



int w25q_close(void)
{
	
	return ERR_OK;
	
}



int w25q_erase(uint32_t offset, uint32_t len)
{
	uint32_t start, end, erase_size;
	int ret = 0;
	
	erase_size = SECTOR_SIZE;
	W25Q_tx_buf[0] = W25Q_INSTR_Sector_Erase_4K;
	
	
	start = offset;
	end = start + len;
	
	while (offset < end) {
		W25Q_tx_buf[1] = offset >> 16;
		W25Q_tx_buf[2] = offset >> 8;
		W25Q_tx_buf[3] = offset >> 0;
		offset += erase_size;

		ret = w25q_write_waitbusy( W25Q_tx_buf, 4);
		if( ret != ERR_OK)
			goto exit;
		
		
	}
	return ERR_OK;
	exit:
	W25Q_Disable_CS;
	return ret;	
	
}

//将提供的扇区进行擦除操作。扇区号的范围是0 - 4096 （w25q128）

int w25q_Erase_Sector(uint32_t Sector_Number)
{

	uint8_t Block_Num = Sector_Number / BLOCK_HAS_SECTORS;
	if( Sector_Number > W25Q_flash.sector_num)
			return ERR_BAD_PARAMETER;
	
	Sector_Number %= BLOCK_HAS_SECTORS;
	W25Q_tx_buf[0] = W25Q_INSTR_Sector_Erase_4K;
	W25Q_tx_buf[1] = Block_Num;
	W25Q_tx_buf[2] = Sector_Number<<4;
	W25Q_tx_buf[3] = 0;
	return w25q_write_waitbusy( W25Q_tx_buf, 4);
	
}


int w25q_Erase_block(uint16_t block_Number)
{

	if( block_Number > W25Q_flash.block_num)
		return ERR_BAD_PARAMETER;
	
	W25Q_tx_buf[0] = W25Q_INSTR_BLOCK_Erase_64K;
	W25Q_tx_buf[1] = block_Number;
	W25Q_tx_buf[2] = 0;
	W25Q_tx_buf[3] = 0;
	return w25q_write_waitbusy( W25Q_tx_buf, 4);
	
}

int w25q_Erase_chip_c7(void)
{

	W25Q_tx_buf[0] = W25Q_INSTR_Chip_Erase_C7;
	
	return w25q_write_waitbusy( W25Q_tx_buf, 1);
	
}
int w25q_Erase_chip_60(void)
{

	W25Q_tx_buf[0] = W25Q_INSTR_Chip_Erase_60;
	
	return w25q_write_waitbusy( W25Q_tx_buf, 1);
	
}



int w25q_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum)
{

		short step = 0;
		short count = 100;
		int ret = -1;
		
		while(1)
		{
			switch( step)
			{
				case 0:
					if( w25q_wr_enable() != ERR_OK)
					{
						ret =  ERR_DRI_OPTFAIL;
						goto exit;
					}
					
					W25Q_Enable_CS;
					W25Q_tx_buf[0] = W25Q_INSTR_Page_Program;
					W25Q_tx_buf[1] = (uint8_t)((WriteAddr&0x00ff0000)>>16);
					W25Q_tx_buf[2] = (uint8_t)((WriteAddr&0x0000ff00)>>8);
					W25Q_tx_buf[3] = (uint8_t)WriteAddr;
					if(SPI_WRITE( W25Q_tx_buf, 4) != ERR_OK)
					{
						ret =  ERR_DRI_OPTFAIL;
						goto exit;
					}
					if(SPI_WRITE( pBuffer, WriteBytesNum) != ERR_OK)
					{
						ret =  ERR_DRI_OPTFAIL;
						goto exit;
					}
					W25Q_Disable_CS;
					
					step++;
					break;
				case 1:
					if( w25q_ReadSR() & W25Q_STATUS1_BUSYBIT)
					{
						W25Q_DELAY_MS(1);
						if( count)
							count --;
						else
						{
							ret =  ERR_DEV_TIMEOUT;
							goto exit;
						}
						break;
					}
					
					ret =  ERR_OK;
					goto exit;
			
				default:
					step = 0;
					break;
				
			}		//switch
			
			
		}		//while(1)
		
		exit:
		
		
		return ret;
}


int w25q_Write_Sector_Data(uint8_t *pBuffer, uint16_t Sector_Num)
{
	int wr_page = 0;
	int		ret;
	while(1)
	{
		ret = w25q_Write(pBuffer + wr_page*PAGE_SIZE, Sector_Num*SECTOR_SIZE + wr_page * PAGE_SIZE, PAGE_SIZE);
		if( ret != ERR_OK)
			return ret;
		wr_page ++;
		if( wr_page >= SECTOR_HAS_PAGES)
			return ERR_OK;
	}
   
}


int w25q_Read_Sector_Data(uint8_t *pBuffer, uint16_t Sector_Num)
{
	uint8_t Block_Num = Sector_Num / BLOCK_HAS_SECTORS;
	if( Sector_Num > W25Q_flash.sector_num)
		return ERR_BAD_PARAMETER;
	memset( pBuffer, 0xff, SECTOR_SIZE);
	
	W25Q_Enable_CS;
	W25Q_tx_buf[0] = W25Q_INSTR_READ_DATA;
	W25Q_tx_buf[1] = Block_Num;
	W25Q_tx_buf[2] = Sector_Num<<4;
	W25Q_tx_buf[3] = 0;
	if( SPI_WRITE(W25Q_tx_buf, 4) != ERR_OK)
		return ERR_DRI_OPTFAIL;
	
	if( SPI_READ(pBuffer, SECTOR_SIZE) != ERR_OK)
		return ERR_DRI_OPTFAIL;
	W25Q_Disable_CS;
	return ERR_OK;
}

int w25q_Read_page_Data(uint8_t *pBuffer, uint16_t num_page)
{
	return w25q_rd_data( pBuffer, num_page * PAGE_SIZE, PAGE_SIZE);
}

int w25q_rd_data(uint8_t *pBuffer, uint32_t rd_add, uint32_t len)
{
	if( len > PAGE_SIZE)
		return ERR_BAD_PARAMETER;
	W25Q_tx_buf[0] = W25Q_INSTR_READ_DATA;
	W25Q_tx_buf[1] = (uint8_t)((rd_add&0x00ff0000)>>16);
	W25Q_tx_buf[2] = (uint8_t)((rd_add&0x0000ff00)>>8);
	W25Q_tx_buf[3] = (uint8_t)rd_add;
	W25Q_Enable_CS;
	
	if( SPI_WRITE( W25Q_tx_buf, 4) != ERR_OK)
		return ERR_DRI_OPTFAIL;
	if( SPI_READ( pBuffer, len) != ERR_OK)
		return ERR_DRI_OPTFAIL;
	W25Q_Disable_CS;
	return ERR_OK;

}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static int w25q_wr_enable(void)
{
	int ret;
//	int count = 50;
	uint8_t cmd = W25Q_INSTR_WR_ENABLE;
	
	
	
	W25Q_Enable_CS;
	ret = SPI_WRITE( &cmd,1);
	W25Q_Disable_CS;
	return ret;
}


static uint8_t w25q_ReadSR(void)
{
    uint8_t cmd = 0;
	uint8_t	status;
    W25Q_Enable_CS;
	
	
	cmd = W25Q_INSTR_RD_Status_Reg1;
	
	if( SPI_WRITE( &cmd, 1) != ERR_OK)
		return 0xff;
	if( SPI_READ( &status, 1) != ERR_OK)
		return 0xff;
    W25Q_Disable_CS;
    return status;
}



static int w25q_write_waitbusy(uint8_t *data, int len)
{
	short step = 0;
	short count = 100;
	int ret = -1;
	
	
	while(1)
	{
		switch( step)
		{	
			case 0:
				
				if( w25q_wr_enable() != ERR_OK)
				{
					ret =  ERR_DRI_OPTFAIL;
					goto exit;
				}
				
				W25Q_Enable_CS;
				if( SPI_WRITE( data, len) != ERR_OK)
				{
					ret =  ERR_DRI_OPTFAIL;
					goto exit;
				}
				step ++;
				W25Q_Disable_CS;

//				W25Q_DELAY_MS(1);
				break;
			case 1:
				if( w25q_ReadSR() & W25Q_STATUS1_BUSYBIT)
				{
					W25Q_DELAY_MS(1);
					if( count)
						count --;
					else
					{
						ret =  ERR_DEV_TIMEOUT;
						goto exit;
					}
					break;
				}
				
				ret =  ERR_OK;
				goto exit;
			default:
				step = 0;
				break;
		}
	}
		
	exit:
	W25Q_Disable_CS;
	return ret;

}


static int w25q_read_id(void)
{
	uint8_t id[2];
	//read id
	W25Q_tx_buf[0] = 0x90;
	W25Q_tx_buf[1] = 0;
	W25Q_tx_buf[2] = 0;
	W25Q_tx_buf[3] = 0;
	W25Q_Enable_CS;
	
	SPI_WRITE( W25Q_tx_buf, 4);
	SPI_READ( id, 2);
	
	W25Q_Disable_CS;
	W25Q_flash.id[0] =  id[0];
	W25Q_flash.id[1] =  id[1];
	if( id[0] == 0xEF && id[1] == 0x17)		//w25Q128
	{
		W25Q_flash.page_num = 65536;
		W25Q_flash.sector_num = W25Q_flash.page_num/SECTOR_HAS_PAGES;
		W25Q_flash.block_num = W25Q_flash.sector_num/BLOCK_HAS_SECTORS;
		return ERR_OK;
	}
	
	if( id[0] == 0xEF && id[1] == 0x16)		//w25Q64
	{
		W25Q_flash.page_num = 32768;
		W25Q_flash.sector_num = W25Q_flash.page_num/SECTOR_HAS_PAGES;
		W25Q_flash.block_num = W25Q_flash.sector_num/BLOCK_HAS_SECTORS;
		return ERR_OK;
	}
	

	return ERR_FAIL;
	
	
}








