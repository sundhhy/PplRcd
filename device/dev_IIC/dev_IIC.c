//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "dev_IIC.h"
#include "basis/sdhDebug.h"
#include "basis/macros.h"
#include "sdhDef.h"
#include <stdarg.h>
#include <stdint.h>
#include "dev_cmd.h"
#include "os/os_depend.h"
#include "hardwareConfig.h"
#include "IIC/drive_IIC.h"
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
static Dev_IIC *d_IIC[NUM_IICS] = {NULL};

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static int Open_dev_IIC(I_dev_Char *self, void *conf);
static int Close_dev_IIC(I_dev_Char *self);
static int Read_dev_IIC(I_dev_Char *self, void *buf, int rd_len);
static int Write_dev_IIC(I_dev_Char *self, void *buf, int wr_len);
static int Ioctol_dev_IIC(I_dev_Char *self, int cmd, ...);
static int Test_dev_IIC(I_dev_Char *self, void *testBuf, int len);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

Dev_IIC *Get_Dev_IIC(int minor)
{
	I_dev_Char *devChar;
	if(minor >= NUM_IICS)
		return NULL;
	if(d_IIC[minor] == NULL) 
	{
		d_IIC[minor] = Dev_IIC_new();
		d_IIC[minor]->minor = minor;
		devChar = SUPER_PTR(d_IIC[minor], I_dev_Char);
		
		devChar->open(devChar, &arr_conf_IIC[minor]);
			
		
	}
	
	return d_IIC[minor];

}


CTOR( Dev_IIC)
FUNCTION_SETTING( I_dev_Char.open, Open_dev_IIC);
FUNCTION_SETTING( I_dev_Char.close, Close_dev_IIC);
FUNCTION_SETTING( I_dev_Char.read, Read_dev_IIC);
FUNCTION_SETTING( I_dev_Char.write, Write_dev_IIC);
FUNCTION_SETTING( I_dev_Char.ioctol, Ioctol_dev_IIC);
FUNCTION_SETTING( I_dev_Char.test, Test_dev_IIC);
END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static int Open_dev_IIC(I_dev_Char *self, void *conf)
{
	Dev_IIC		*cthis = SUB_PTR( self, I_dev_Char, Dev_IIC);
	
	return Init_IIC(cthis->minor, conf);
}

static int Close_dev_IIC(I_dev_Char *self)
{

	return RET_OK;
}

static int Read_dev_IIC(I_dev_Char *self, void *buf, int rd_len)
{
	Dev_IIC		*cthis = SUB_PTR( self, I_dev_Char, Dev_IIC);
	int				len = 0;
	int				ret;
	while(len < rd_len) 
	{
		ret = Read_IIC(cthis->minor, buf, rd_len);
		if(ret >0)
			len += ret;
	}
	
	return len;
}

static int Write_dev_IIC(I_dev_Char *self, void *buf, int wr_len)
{
	Dev_IIC		*cthis = SUB_PTR( self, I_dev_Char, Dev_IIC);
	int				len = 0;
	int 			ret = 0;
	while(len < wr_len)
	{
		ret = Write_IIC(cthis->minor, buf, wr_len);
		if(ret >0)
			len += ret;
		
		
	}
	
	return len;

}

static int Ioctol_dev_IIC(I_dev_Char *self, int cmd, ...)
{

	return RET_OK;
}
static int Test_dev_IIC(I_dev_Char *self, void *testBuf, int len)
{

	
	return RET_OK;

}

