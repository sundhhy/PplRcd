
#include "stdint.h"
#include "GPIO/drive_gpio.h"
#include "stm32f10x_exti.h"

#include "basis/sdhError.h"
#include <stdarg.h>
#include <string.h>
#include "sdhDef.h"

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
#define EXTI_MAX		16
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
//用在中断程序中，找到对应的驱动
static driveGpio	*arr_gpio[ EXTI_MAX];
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
/* Cycle/Sync Callback functions */


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






static int GpioInit( driveGpio *self, void *p_base, void *cfg)
{
	gpio_pins	*p_gpio = ( gpio_pins *)cfg;
	EXTI_InitTypeDef		st_exti;
	arr_gpio[ p_gpio->extiLine] = self;
	self->p_cfg = cfg;
	self->p_gpioBase = p_base;
	
	if( p_gpio->direction == GPIO_DIR_OUT)
		return ERR_OK;
	
	
	EXTI_StructInit( &st_exti);
	
	
	st_exti.EXTI_Line = p_gpio->extiLine;
	st_exti.EXTI_LineCmd = ENABLE;
	
	switch( p_gpio->irqType)
	{
		
		case GPIO_IRQ_FAILING:
			st_exti.EXTI_Trigger = EXTI_Trigger_Falling;
			break;
		case GPIO_IRQ_RISING:
			st_exti.EXTI_Trigger = EXTI_Trigger_Rising;
			break;
		case GPIO_IRQ_BOTHEDGE:
			st_exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
			break;
		
	}
	
	
	EXTI_Init( &st_exti);

	
	return ERR_OK;
	
}


static int GpioDeInit( driveGpio *self)
{
	
	
	self->func_hdl = NULL;
	
	return ERR_OK;
	
}

/*!
**
**
** @param data 
** @param size 
** @return
**/
static int GpioWrite( driveGpio *self, int n_val)
{
//	gpio_pins	*p_gpio = ( gpio_pins *)self->p_cfg;

	return ERR_OK;
}



/*!
**
**
** @param data 
** @param size 
** @return
**/
static int GpioRead( driveGpio *self,int *p_n_val)
{
	gpio_pins	*p_gpio = ( gpio_pins *)self->p_cfg;

	return ERR_OK;
}



/*!
**
**
** @param size
**
** @return
**/
static int  GpioIoctol( driveGpio *self, int cmd, ...)
{
//	int int_data;
//	va_list arg_ptr; 
//	va_start(arg_ptr, cmd); 
	
	
	
	return RET_OK;
}




static int GpioTest( driveGpio *self, void *buf, int size)
{
	
	
}


static void GpioSetIrqHdl( driveGpio *self, irqHdl hdl)
{
	
	self->func_hdl = hdl;
	
	
}

static void GpioSetEncode( driveGpio *self, int e)
{
	self->encode[0] = e;
	
}




CTOR( driveGpio)
FUNCTION_SETTING( init, GpioInit);
FUNCTION_SETTING( deInit, GpioDeInit);
FUNCTION_SETTING( read, GpioRead);
FUNCTION_SETTING( write, GpioWrite);
FUNCTION_SETTING( ioctol, GpioIoctol);
FUNCTION_SETTING( setIrqHdl, GpioSetIrqHdl);
FUNCTION_SETTING( setEncode, GpioSetEncode);

FUNCTION_SETTING( test, GpioTest);
END_CTOR

