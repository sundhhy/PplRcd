
#include "stdint.h"
#include "device_gpio.h"
#include "dev_cmd.h"

#include "basis/sdhError.h"
#include <stdarg.h>
#include <string.h>
#include "sdhDef.h"
#include "hardwareConfig.h"

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
//只有输入引脚需要用这个驱动模块，输出的GPIO直接用STM32的库API就行了
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
#define NUMPIN		8
const gpio_pins *arr_Gpiocfg[NUMPIN] ={ &pin_keyRight, &pin_keyLeft, &pin_keyUp, \
	&pin_keyDown, &pin_keyEnter, &pin_keyEsc, &pin_UsbInt, &pin_pwr};

const GPIO_TypeDef *arr_Gpiobase[NUMPIN] ={ GPIO_PORT_KEY_RIGHT, GPIO_PORT_KEY_LEFT, GPIO_PORT_KEY_UP, \
	GPIO_PORT_KEY_DOWN, GPIO_PORT_KEY_ENTER, GPIO_PORT_KEY_ESC, GPIO_PORT_USBINT, PORT_PWR};

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
devGpio *arr_gpioInstance[NUMPIN];

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
/* Cycle/Sync Callback functions */


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
devGpio *Get_DevGpio(int minor)
{
	devGpio *p_dev;
	
	if( minor >= NUMPIN)
		return NULL;
	if( arr_gpioInstance[ minor])
		return  arr_gpioInstance[ minor];
	
	p_dev = devGpio_new();
	if(p_dev == NULL) while(1);
	if( p_dev == NULL)
		return NULL;
	
	p_dev->dri = driveGpio_new();
	if(p_dev->dri == NULL) while(1);
	if( p_dev->dri == NULL)
		goto destoryQuit0;
	
	if( p_dev->dri->init( p_dev->dri, (void *)arr_Gpiobase[ minor], (void *)arr_Gpiocfg[ minor]) != RET_OK)
		goto destoryQuit1;
	
	return p_dev;
destoryQuit1:
	lw_oopc_free( p_dev->dri);	
destoryQuit0:
	
	lw_oopc_free( p_dev);
	return NULL;
	
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{






static int DevGpioOpen( I_dev_Char *self, void *conf)
{
	
	
	
	
	


	

	
	return ERR_OK;
	
}

static int DevGpioClose( I_dev_Char *self)
{
	
	return RET_OK; 
}

static int DevGpioRead( I_dev_Char *self, void *buf, int rdLen)
{
	char val;
	devGpio *cthis = ( devGpio *)self;
	
	if( cthis->dri->read( cthis->dri, &val) == RET_OK)
	{
		*(char *)buf = val;
		return RET_OK;
	}
	
	return ERR_FAIL; 
}

static int DevGpioWrite( I_dev_Char *self, void *buf, int wrLen)
{
	return RET_OK; 
}

static int DevGpioIoctol( I_dev_Char *self ,int cmd, ...)
{
	devGpio *cthis = ( devGpio *)self;
	irqHdl		hdl;
	void		*arg;
	int int_data;
	va_list arg_ptr; 
	va_start(arg_ptr, cmd); 
	
	switch( cmd)
	{
		case DEVCMD_SET_IRQHDL:
			hdl =  va_arg(arg_ptr, irqHdl);
			arg =  va_arg(arg_ptr, void *);
			va_end(arg_ptr); 
			cthis->dri->func_hdl = hdl;
			cthis->dri->hdl_arg = arg;
			break;
		case DEVGPIOCMD_SET_ENCODE:
			int_data = va_arg(arg_ptr, int);
			va_end(arg_ptr); 
			cthis->dri->setEncode( cthis->dri, int_data);
			break;
		
		case DEVCMD_DISABLE_IRQ:
			
			cthis->dri->control_irq( cthis->dri, 0);
			break;
		case DEVCMD_ENABLE_IRQ:
			cthis->dri->control_irq( cthis->dri, 1);
			break;
			
	}
	return RET_OK; 
}

static int DevGpioTest( I_dev_Char *self, void *testBuf, int len)
{
	
	return RET_OK; 

}




CTOR( devGpio)
FUNCTION_SETTING( I_dev_Char.open, DevGpioOpen);
FUNCTION_SETTING( I_dev_Char.close, DevGpioClose);
FUNCTION_SETTING( I_dev_Char.read, DevGpioRead);
FUNCTION_SETTING( I_dev_Char.write, DevGpioWrite);
FUNCTION_SETTING( I_dev_Char.ioctol, DevGpioIoctol);
FUNCTION_SETTING( I_dev_Char.test, DevGpioTest);
END_CTOR

