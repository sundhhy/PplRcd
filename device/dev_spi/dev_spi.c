//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "dev_spi.h"
#include "basis/sdhDebug.h"
#include "basis/macros.h"
#include "sdhDef.h"
#include <stdarg.h>
#include <stdint.h>
#include "dev_cmd.h"
#include "os/os_depend.h"
#include "hardwareConfig.h"
#include "SPI/drive_spi.h"
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

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static Dev_spi *d_spi[NUM_SPIS] = {NULL};

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static int Open_devSpi(I_dev_Char *self, void *conf);
static int Close_devSpi(I_dev_Char *self);
static int Read_devSpi(I_dev_Char *self, void *buf, int rd_len);
static int Write_devSpi(I_dev_Char *self, void *buf, int wr_len);
static int Ioctol_devSpi(I_dev_Char *self, int cmd, ...);
static int Test_devSpi(I_dev_Char *self, void *testBuf, int len);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

Dev_spi *Get_DevSpi(int minor)
{
	I_dev_Char *devChar;
	if(minor >= NUM_SPIS)
			return NULL;
	if(d_spi[minor] == NULL) 
	{
		d_spi[minor] = Dev_spi_new();
		if(d_spi[ minor]  == NULL) while(1);
		d_spi[minor]->minor = minor;
		devChar = SUPER_PTR(d_spi[minor], I_dev_Char);
		
		devChar->open(devChar, &arr_conf_spi[minor]);
			
		
	}
	
	return d_spi[minor];

}


CTOR( Dev_spi)
FUNCTION_SETTING( I_dev_Char.open, Open_devSpi);
FUNCTION_SETTING( I_dev_Char.close, Close_devSpi);
FUNCTION_SETTING( I_dev_Char.read, Read_devSpi);
FUNCTION_SETTING( I_dev_Char.write, Write_devSpi);
FUNCTION_SETTING( I_dev_Char.ioctol, Ioctol_devSpi);
FUNCTION_SETTING( I_dev_Char.test, Test_devSpi);
END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static int Open_devSpi(I_dev_Char *self, void *conf)
{
	Dev_spi		*cthis = SUB_PTR( self, I_dev_Char, Dev_spi);
	
	return Init_spi(cthis->minor, conf);
}

static int Close_devSpi(I_dev_Char *self)
{

	return RET_OK;
}

static int Read_devSpi(I_dev_Char *self, void *buf, int rd_len)
{
	Dev_spi		*cthis = SUB_PTR( self, I_dev_Char, Dev_spi);
	int				len = 0;
	int				ret;
	while(len < rd_len) 
	{
		ret = Read_spi(cthis->minor, arr_conf_spi[cthis->minor].datasize_bit, buf, rd_len);
		if(ret >0)
			len += ret;
		
	}
	
	return len;
}

static int Write_devSpi(I_dev_Char *self, void *buf, int wr_len)
{
	Dev_spi		*cthis = SUB_PTR( self, I_dev_Char, Dev_spi);
	int				len = 0;
	int 			ret = 0;
	while(len < wr_len)
	{
		ret = Write_spi(cthis->minor, arr_conf_spi[cthis->minor].datasize_bit, buf, wr_len);
		if(ret >0)
			len += ret;
		
		
	}
	
	return len;

}

static int Ioctol_devSpi(I_dev_Char *self, int cmd, ...)
{

	return RET_OK;
}
static int Test_devSpi(I_dev_Char *self, void *testBuf, int len)
{

	
	return RET_OK;

}

