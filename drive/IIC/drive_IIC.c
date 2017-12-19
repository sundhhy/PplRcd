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
static iic_conf_t *arr_p_conf[NUM_IICS];

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void I2C_wait_standby_state(void);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int	Init_IIC(int No, iic_conf_t *c)
{
	
	I2C_TypeDef					*i2c_reg;
	I2C_InitTypeDef			i2c_init;
	int 							ret = RET_OK;	
	
	if(No >= NUM_IICS)
		return ERR_BAD_PARAMETER;
	
	if(No == 0)
		i2c_reg = I2C1;
	else if(No == 1)
		i2c_reg = I2C2;
	I2C_StructInit(&i2c_init);
	
	i2c_init.I2C_ClockSpeed = c->speed;
	if(c->duty_cycle)
		i2c_init.I2C_DutyCycle = I2C_DutyCycle_16_9;
	else
		i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
	
	i2c_init.I2C_OwnAddress1 = c->own_addr;
	if(c->ack_enbale)
		i2c_init.I2C_Ack = I2C_Ack_Enable;
	else
		i2c_init.I2C_Ack = I2C_Ack_Disable;
	
	if(c->addr_bits == 7)
		i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	else if(c->addr_bits == 10)
		i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
	else 
	{
		ret = ERR_BAD_PARAMETER;
		goto exit;
	}
	
	I2C_Init(i2c_reg, &i2c_init);
	I2C_Cmd(i2c_reg, ENABLE);
	
	arr_p_conf[No] = c;
	exit:
	return ret;

}

int Read_IIC(int No, void *buf, int rd_addr, int rd_len)
{
	I2C_TypeDef					*i2c_reg;
	I2C_InitTypeDef			i2c_init;
	int 							ret = RET_OK;	
	
	if(No >= NUM_IICS)
		return ERR_BAD_PARAMETER;
	
	if(No == 0)
		i2c_reg = I2C1;
	else if(No == 1)
		i2c_reg = I2C2;
	
	
//	while(
	

	
}

int Write_IIC(int No, void *buf, uint8_t slave_addr, uint8_t reg_addr, uint16_t len)
{
	
	I2C_TypeDef					*i2c_reg;
	I2C_InitTypeDef			i2c_init;
	int 							ret = RET_OK;	
	
	if(No >= NUM_IICS)
		return ERR_BAD_PARAMETER;
	
	if(No == 0)
		i2c_reg = I2C1;
	else if(No == 1)
		i2c_reg = I2C2;
	
	while(I2C_GetFlagStatus(i2c_reg, I2C_FLAG_BUSY)) ;
	
	I2C_GenerateSTART(i2c_reg, ENABLE);
	
	//等待EV5 然后清除它
	while(!I2C_CheckEvent(i2c_reg, I2C_EVENT_MASTER_MODE_SELECT));
	
	if(arr_p_conf[No]->addr_bits == 7)
	{
		I2C_Send7bitAddress(i2c_reg, wr_addr, I2C_Direction_Transmitter);
	}
	else
	{
		I2C_Send7bitAddress(i2c_reg, wr_addr, I2C_Direction_Transmitter);
	}
	
	//等待EV6 然后清除它
	while(!I2C_CheckEvent(i2c_reg, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

}




//void SPI1_IRQHandler(void)
//{

//	if( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_OVR))
//	{
//		//依次读取SPI_DR和SPI_SR来清除OVR
//		SPI_I2S_ReceiveData(SPI1);
//		SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_OVR);
//	}
	
//}


//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static void I2C_wait_standby_state(void)
{
	
}

