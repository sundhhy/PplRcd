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
static int I2C_wait_EV(I2C_TypeDef* I2Cx, uint32_t ev);

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

int Read_IIC(int No, void *buf, uint8_t slave_addr, uint8_t reg_addr, uint16_t rd_len)
{
	I2C_TypeDef				*i2c_reg;
	int 					ret = RET_OK;	
	uint8_t					*p_u8 = (uint8_t *)buf;
	uint16_t				bytes_to_read = rd_len;
	uint32_t				safe_count = 10000;
	
	if(No >= NUM_IICS)
		return ERR_BAD_PARAMETER;
	
	if(No == 0)
		i2c_reg = I2C1;
	else if(No == 1)
		i2c_reg = I2C2;
	
	
	while(I2C_GetFlagStatus(i2c_reg, I2C_FLAG_BUSY))
	{
		if(safe_count)
			safe_count--; 
		else
		{
			
			return ERR_DEV_TIMEOUT;
		}
		
	}


	I2C_GenerateSTART(i2c_reg, ENABLE);

	/* Test on EV5 and clear it */
	if( I2C_wait_EV(i2c_reg, I2C_EVENT_MASTER_MODE_SELECT) < 0)
	{
		ret = ERR_DEV_TIMEOUT;	
		goto err_exit;
	}

	/* Send EEPROM address for write */
	I2C_Send7bitAddress(i2c_reg, slave_addr, I2C_Direction_Transmitter);

	/* Test on EV6 and clear it */
	if(I2C_wait_EV(i2c_reg, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) < 0)
	{
		ret = ERR_DEV_TIMEOUT;	
		
		goto err_exit;
	}

	/* Clear EV6 by setting again the PE bit */
//	I2C_Cmd(i2c_reg, ENABLE);

	/* Send the EEPROM's internal address to write to */
	I2C_SendData(i2c_reg, reg_addr);  

	/* Test on EV8 and clear it */
	if(I2C_wait_EV(i2c_reg, I2C_EVENT_MASTER_BYTE_TRANSMITTED) < 0)
	{
		ret = ERR_DEV_TIMEOUT;	
		goto err_exit;
	}
  
  /* Send STRAT condition a second time */  
    I2C_GenerateSTART(i2c_reg, ENABLE);
	
	/* Test on EV5 and clear it */
	if( I2C_wait_EV(i2c_reg, I2C_EVENT_MASTER_MODE_SELECT) < 0)
	{
		ret = ERR_DEV_TIMEOUT;	
		goto err_exit;
	}

	I2C_Send7bitAddress(i2c_reg, slave_addr, I2C_Direction_Receiver);
  
	/* Test on EV6 and clear it */
	if( I2C_wait_EV(i2c_reg, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) < 0)
	{
		ret = ERR_DEV_TIMEOUT;	
		goto err_exit;
	}
	
  /* While there is data to be read */
 //180121 这里会进入死循环，导致系统卡死，因此要进行超时退出
	safe_count = 1000;
  while(bytes_to_read)  
  {
    if(bytes_to_read == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(i2c_reg, DISABLE);
      
      /* Send STOP Condition */
      I2C_GenerateSTOP(i2c_reg, ENABLE);
		
		if(safe_count)
			safe_count --;
		else
		{
			
			ret = ERR_DEV_TIMEOUT;
			goto err_exit;
		}
		
	
    }

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(i2c_reg, I2C_EVENT_MASTER_BYTE_RECEIVED))  
    {      
      /* Read a byte from the EEPROM */
      *p_u8 = I2C_ReceiveData(i2c_reg);

      /* Point to the next location where the byte read will be saved */
      p_u8++; 
      
      /* Decrement the read bytes counter */
      bytes_to_read--;        
    }   
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(i2c_reg, ENABLE);
  return (rd_len - bytes_to_read);
  
err_exit:

	/* Send STOP condition */
	I2C_GenerateSTOP(i2c_reg, ENABLE);
	I2C_DeInit(i2c_reg);
	Init_IIC(No, arr_p_conf[No]);
	return ret;
	
}

int Write_IIC(int No, void *buf, uint8_t slave_addr, uint8_t reg_addr, uint16_t wr_len)
{
	
	I2C_TypeDef					*i2c_reg;
	int 						ret = RET_OK;
	uint8_t						*p_u8 = (uint8_t *)buf;
	uint16_t					bytes_to_write = wr_len;
	if(No >= NUM_IICS)
		return ERR_BAD_PARAMETER;
	
	if(No == 0)
		i2c_reg = I2C1;
	else if(No == 1)
		i2c_reg = I2C2;
	
	while(I2C_GetFlagStatus(i2c_reg, I2C_FLAG_BUSY)) ;
	
	I2C_GenerateSTART(i2c_reg, ENABLE);
	
	//等待EV5 然后清除它
	if( I2C_wait_EV(i2c_reg, I2C_EVENT_MASTER_MODE_SELECT) < 0)
	{
		ret = ERR_DEV_TIMEOUT;	
		goto err_exit;
	}
	
	if(arr_p_conf[No]->addr_bits == 7)
	{
		I2C_Send7bitAddress(i2c_reg, slave_addr, I2C_Direction_Transmitter);
	}
	else
	{
		I2C_Send7bitAddress(i2c_reg, slave_addr, I2C_Direction_Transmitter);
	}
	
	//等待EV6 然后清除它
	if( I2C_wait_EV(i2c_reg, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) < 0)
	{
		ret = ERR_DEV_TIMEOUT;	
		goto err_exit;
	}
	
	I2C_SendData(i2c_reg, reg_addr); 
	
	/* Test on EV8 and clear it */
	if( I2C_wait_EV(i2c_reg, I2C_EVENT_MASTER_BYTE_TRANSMITTED) < 0)
	{
		ret = ERR_DEV_TIMEOUT;	
		goto err_exit;
	}
	
	 /* While there is data to be written */
	bytes_to_write--;
	while(1)  
	{
		/* Send the current byte */
		I2C_SendData(i2c_reg, *p_u8); 

		/* Point to the next byte to be written */
		p_u8++; 

		/* Test on EV8 and clear it */
		if( I2C_wait_EV(i2c_reg, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == 0)
		{
//			ret = ERR_DEV_TIMEOUT;	
			if(bytes_to_write)
				bytes_to_write--;
			else
				goto exit;
		}
		else
		{
			goto exit;
		}
		
			
	}

	exit:
	I2C_GenerateSTOP(i2c_reg, ENABLE);
	return (wr_len - bytes_to_write);
	err_exit:

	/* Send STOP condition */
	I2C_GenerateSTOP(i2c_reg, ENABLE);
	return ret;
	

}




//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


static int I2C_wait_EV(I2C_TypeDef* I2Cx, uint32_t ev)
{
	int safe_count = 100000;
	

	
	while(! I2C_CheckEvent(I2Cx, ev))
	{
		if(safe_count)
			safe_count --;
		else
			break;
		
	}
	
//return 0;
	
	if(safe_count)
		return 0;
	else 
		return -1;
	
}
