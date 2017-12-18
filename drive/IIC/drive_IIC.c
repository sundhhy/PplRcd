//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "stdint.h"
#include "IIC/drive_IIC.h"
#include "basis/sdhError.h"
#include <stdarg.h>
#include <string.h>
#include "sdhDef.h"
#include "mem/CiiMem.h"
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

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int	Init_IIC(int No, spi_conf_t *c)
{
//	int 							ret = RET_OK;
//	SPI_TypeDef				*spi_reg = NULL;
//	SPI_InitTypeDef 	spi_init;
//	
//	if(No >= NUM_SPIS)
//		return ERR_BAD_PARAMETER;
//	
//	if(No == 0)
//		spi_reg = SPI1;
//	else if(No == 1)
//		spi_reg = SPI2;
//	else if(No == 2)
//		spi_reg = SPI3;
//	
//	SPI_StructInit(&spi_init);
//	
//	if(c->work_mode == 0)
//	{
//		spi_init.SPI_Mode = SPI_Mode_Master;
//	} 
//	else 
//	{
//		spi_init.SPI_Mode = SPI_Mode_Slave;
//	}

//	Spi_mode_CP(c->mode, &spi_init);
//	if(c->nss == 0)
//		spi_init.SPI_NSS = SPI_NSS_Soft;
//	else
//		spi_init.SPI_NSS = SPI_NSS_Hard;
//	
//	if(c->datasize_bit == 8)
//		spi_init.SPI_DataSize = SPI_DataSize_8b;
//	else
//		spi_init.SPI_DataSize = SPI_DataSize_16b;
//	
//	Spi_baud_Prescaler(c->baud, &spi_init);
//	
//	SPI_Init(spi_reg, &spi_init);
//	SPI_I2S_ITConfig(spi_reg, SPI_I2S_IT_OVR, ENABLE);
//	SPI_Cmd(spi_reg, ENABLE);
//	
//	return ret;
}

int Read_IIC(int No, void *buf, int rd_len)
{
//	SPI_TypeDef	*spi_reg = NULL;
//	int 				i = 0;
//	int 				ret = 0;
//	uint8_t			*data_u8 = (uint8_t			*)buf;
//	uint16_t		*data_u16 = (uint16_t			*)buf;
//	if(No >= NUM_SPIS)
//		return ERR_BAD_PARAMETER;
//	
//	if(No == 0)
//		spi_reg = SPI1;
//	else if(No == 1)
//		spi_reg = SPI2;
//	else if(No == 2)
//		spi_reg = SPI3;
//	
//	
//	
//	
//	if(data_size == 8) 
//	{
//		for( i = 0; i < rd_len; i++)
//		{

//			ret = spi_read_word(spi_reg);
//			if( ret < 0)
//				break;
//			data_u8[i] = ret;

//			
//			
//		}
//	} 
//	else if(data_size == 16) 
//	{
//		rd_len = rd_len >> 1;
//		for( i = 0; i < rd_len; i++)
//		{

//			ret = spi_read_word(spi_reg);
//			if(ret < 0) 
//			{
//				
//				break;
//				
//			}
//			data_u16[i] = ret;

//			
//			
//		}
//		i = i * 2;
//	} 
//	return i;	
	
}

int Write_IIC(int No, void *buf, int len)
{
//	SPI_TypeDef	*spi_reg = NULL;
//	int 				i = 0;
//	int 				ret = 0;
//	uint8_t			*data_u8 = (uint8_t			*)buf;
//	uint16_t		*data_u16 = (uint16_t			*)buf;
//	
//	if(No >= NUM_SPIS)
//		return ERR_BAD_PARAMETER;
//	
//	if(No == 0)
//		spi_reg = SPI1;
//	else if(No == 1)
//		spi_reg = SPI2;
//	else if(No == 2)
//		spi_reg = SPI3;
//	
//	if(data_size == 8)
//	{
//		for(i = 0; i < len; i++) {
//			ret = spi_write_word(spi_reg, data_u8[i]);
//			if(ret < 0)
//				break;
//			
//		}
//		
//		
//	}
//	else if(data_size == 16) 
//	{
//		len = len >> 1;
//		for(i = 0; i < len; i++) {
//			ret = spi_write_word(spi_reg, data_u16[i]);
//			if(ret < 0) 
//			{
//			
//				break;
//				
//			}
//			
//		}
//		i = i * 2;
//	}
//	
//	return i;
//}




//void SPI1_IRQHandler(void)
//{

//	if( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_OVR))
//	{
//		//依次读取SPI_DR和SPI_SR来清除OVR
//		SPI_I2S_ReceiveData(SPI1);
//		SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_OVR);
//	}
	
}


//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


