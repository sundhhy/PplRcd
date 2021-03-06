//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_dev_IIC_H__
#define __INC_dev_IIC_H__

#include "basis/sdhError.h"
#include "lw_oopc.h"



#include "dev_char.h"

//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

typedef struct {
	uint8_t			slave_addr;
	uint8_t			reg_addr;
	uint8_t			none[2];
}I2C_slave_t;

CLASS(Dev_IIC)
{
	IMPLEMENTS( I_dev_Char);
	void			*p_IIC;
	I2C_slave_t		*p_i2c_slaver;
	uint8_t		minor;
	uint8_t		none[3];
	
};


//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

Dev_IIC *Get_Dev_IIC(int minor);

#endif

