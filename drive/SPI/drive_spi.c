//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "stdint.h"
#include "SPI/drive_spi.h"
#include "basis/sdhError.h"
#include <stdarg.h>
#include <string.h>
#include "sdhDef.h"
#include "mem/CiiMem.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
//#define SPI_CLK			72000000
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
#define SAFE_COUNT_VAL		2000
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void Spi_mode_CP(int mode, SPI_InitTypeDef *spi_def);
static void Spi_baud_Prescaler(int baudrate, SPI_InitTypeDef *spi_def);
static int spi_read_word(SPI_TypeDef	*spi_reg);
static int spi_write_word(SPI_TypeDef	*spi_reg, uint16_t val);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int	Init_spi(int No, spi_conf_t *c)
{
	int 							ret = RET_OK;
	SPI_TypeDef				*spi_reg = NULL;
	SPI_InitTypeDef 	spi_init;
	
	if(No >= NUM_SPIS)
		return ERR_BAD_PARAMETER;
	
	if(No == 0)
		spi_reg = SPI1;
	else if(No == 1)
		spi_reg = SPI2;
	else if(No == 2)
		spi_reg = SPI3;
	
	SPI_StructInit(&spi_init);
	
	if(c->work_mode == 0)
	{
		spi_init.SPI_Mode = SPI_Mode_Master;
	} 
	else 
	{
		spi_init.SPI_Mode = SPI_Mode_Slave;
	}

	Spi_mode_CP(c->mode, &spi_init);
	if(c->nss == 0)
		spi_init.SPI_NSS = SPI_NSS_Soft;
	else
		spi_init.SPI_NSS = SPI_NSS_Hard;
	
	if(c->datasize_bit == 8)
		spi_init.SPI_DataSize = SPI_DataSize_8b;
	else
		spi_init.SPI_DataSize = SPI_DataSize_16b;
	
	Spi_baud_Prescaler(c->baud, &spi_init);
	
	SPI_Init(spi_reg, &spi_init);
	SPI_I2S_ITConfig(spi_reg, SPI_I2S_IT_OVR, ENABLE);
	SPI_Cmd(spi_reg, ENABLE);
	
	return ret;
}

int Read_spi(int No, int data_size, void *buf, int rd_len)
{
	SPI_TypeDef	*spi_reg = NULL;
	int 				i = 0;
	int 				ret = 0;
	uint8_t			*data_u8 = (uint8_t			*)buf;
	uint16_t		*data_u16 = (uint16_t			*)buf;
	if(No >= NUM_SPIS)
		return ERR_BAD_PARAMETER;
	
	if(No == 0)
		spi_reg = SPI1;
	else if(No == 1)
		spi_reg = SPI2;
	else if(No == 2)
		spi_reg = SPI3;
	
	
	
	
	if(data_size == 8) 
	{
		for( i = 0; i < rd_len; i++)
		{

			ret = spi_read_word(spi_reg);
			if( ret < 0)
				break;
			data_u8[i] = ret;
//			if(i > 254)
//			{
//				data_u16 = NULL;
//				
//			}

			
			
		}
	} 
	else if(data_size == 16) 
	{
		rd_len = rd_len >> 1;
		for( i = 0; i < rd_len; i++)
		{

			ret = spi_read_word(spi_reg);
			if(ret < 0) 
			{
				
				break;
				
			}
			data_u16[i] = ret;

			
			
		}
		i = i * 2;
	} 
	return i;	
	
}

int Write_spi(int No, int data_size, void *buf, int len)
{
	SPI_TypeDef	*spi_reg = NULL;
	int 				i = 0;
	int 				ret = 0;
	uint8_t			*data_u8 = (uint8_t			*)buf;
	uint16_t		*data_u16 = (uint16_t			*)buf;
	
	if(No >= NUM_SPIS)
		return ERR_BAD_PARAMETER;
	
	if(No == 0)
		spi_reg = SPI1;
	else if(No == 1)
		spi_reg = SPI2;
	else if(No == 2)
		spi_reg = SPI3;
	
	if(data_size == 8)
	{
		for(i = 0; i < len; i++) {
			ret = spi_write_word(spi_reg, data_u8[i]);
			if(ret < 0)
				break;
			
//			if(i >254)
//				data_u16 = NULL;
			
		}
		
		
	}
	else if(data_size == 16) 
	{
		len = len >> 1;
		for(i = 0; i < len; i++) {
			ret = spi_write_word(spi_reg, data_u16[i]);
			if(ret < 0) 
			{
			
				break;
				
			}
			
		}
		i = i * 2;
	}
	
	return i;
}




void SPI1_IRQHandler(void)
{

	if( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_OVR))
	{
		//依次读取SPI_DR和SPI_SR来清除OVR
		SPI_I2S_ReceiveData(SPI1);
		SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_OVR);
	}
	
}

void SPI2_IRQHandler(void)
{

	if( SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_OVR))
	{
		//依次读取SPI_DR和SPI_SR来清除OVR
		SPI_I2S_ReceiveData(SPI2);
		SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_OVR);
	}
	
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static void Spi_mode_CP(int mode, SPI_InitTypeDef *spi_def)
{
	
		   /*
        Mode 0 CPOL=0, CPHA=0 
        Mode 1 CPOL=0, CPHA=1
        Mode 2 CPOL=1, CPHA=0 
        Mode 3 CPOL=1, CPHA=1
    */
	switch(mode) 
	{
		case 0:		
			spi_def->SPI_CPOL = SPI_CPOL_Low;
			spi_def->SPI_CPHA = SPI_CPHA_1Edge;
			break;
		case 1:		
			spi_def->SPI_CPOL = SPI_CPOL_Low;
			spi_def->SPI_CPHA = SPI_CPHA_2Edge;
			break;
		case 2:		
			spi_def->SPI_CPOL = SPI_CPOL_High;
			spi_def->SPI_CPHA = SPI_CPHA_1Edge;
			break;
		case 3:		
			spi_def->SPI_CPOL = SPI_CPOL_High;
			spi_def->SPI_CPHA = SPI_CPHA_2Edge;
			break;
	}
	
}

static void Spi_baud_Prescaler(int baudrate, SPI_InitTypeDef *spi_def)
{
//	uint16_t	bpr[8] = {SPI_BaudRatePrescaler_2, SPI_BaudRatePrescaler_4,SPI_BaudRatePrescaler_8, \
//		SPI_BaudRatePrescaler_16, SPI_BaudRatePrescaler_32, SPI_BaudRatePrescaler_64, \
//		SPI_BaudRatePrescaler_128, SPI_BaudRatePrescaler_256};
//	int div = SPI_CLK/baudrate;
//		
//	
//	
//	spi_def->SPI_BaudRatePrescaler = bpr[div];
}

static int spi_read_word(SPI_TypeDef	*spi_reg)
{
	
	int	safe_count = SAFE_COUNT_VAL;	
	
	
	/*! Loop while DR register in not emplty */
	while( SPI_I2S_GetFlagStatus(spi_reg, SPI_I2S_FLAG_TXE) == RESET)
	{
		
		if( safe_count)
		{
			safe_count --;
			
		}
		else
		{
			return ERR_DEV_TIMEOUT;
		}

	}
	 
	/*!Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(spi_reg, 0);
 
	/*! Wait to receive a byte */
	safe_count =  SAFE_COUNT_VAL;
	while (SPI_I2S_GetFlagStatus(spi_reg, SPI_I2S_FLAG_RXNE) == RESET)
	{
		
		if( safe_count)
		{
			safe_count --;
			
		}
		else
		{
			return ERR_DEV_TIMEOUT;
		}
		

		
	}
	 
//	/*! Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(spi_reg);
	
}

static int spi_write_word(SPI_TypeDef	*spi_reg, uint16_t val)
{
	
	int	safe_count = SAFE_COUNT_VAL;
//	uint16_t	tmp = 0;
	
	while( SPI_I2S_GetFlagStatus(spi_reg, SPI_I2S_FLAG_BSY))
	{

		if( safe_count)
		{
			safe_count --;
			
		}
		else
		{
			return ERR_DEV_TIMEOUT;
		}

	}
	
	

	//等待spi的发送缓冲区为空闲的时候开始发送
	safe_count = SAFE_COUNT_VAL;	
	while(SPI_I2S_GetFlagStatus(spi_reg, SPI_I2S_FLAG_TXE) != SET)
	{
		if( safe_count)
		{
			safe_count --;
			
		}
		else
		{
			return ERR_DEV_TIMEOUT;
		}
	}
	
	
	
	SPI_I2S_SendData(spi_reg, val);
	safe_count = SAFE_COUNT_VAL;
	while( SPI_I2S_GetFlagStatus(spi_reg, SPI_I2S_FLAG_RXNE) != SET)
	{
		if( safe_count)
		{
			safe_count --;
			
		}
		else
		{
			return ERR_DEV_TIMEOUT;
		}
	}
	SPI_I2S_ReceiveData(spi_reg); 
 
	
	return RET_OK;
	
	
}

