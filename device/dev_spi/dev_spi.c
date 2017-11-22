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
static int Read_defSpi(I_dev_Char *self, void *buf, int rdLen);
static int Write_devSpi(I_dev_Char *self, void *buf, int wrLen);
static int Ioctol_devSpi(I_dev_Char *self, int cmd, ...);
static int Test_devSpi(I_dev_Char *self, void *testBuf, int len);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

Dev_spi *Get_DevSpi(int minor)
{
    if(minor >= NUM_SPIS)
        return NULL;


}


CTOR( Dev_spi)
FUNCTION_SETTING( I_dev_Char.open, Open_devSpi);
FUNCTION_SETTING( I_dev_Char.close, Close_devSpi);
FUNCTION_SETTING( I_dev_Char.read, Read_defSpi);
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

}

static int Close_devSpi(I_dev_Char *self)
{

}

static int Read_defSpi(I_dev_Char *self, void *buf, int rdLen)
{

}

static int Write_devSpi(I_dev_Char *self, void *buf, int wrLen)
{


}

static int Ioctol_devSpi(I_dev_Char *self, int cmd, ...)
{


}
static int Test_devSpi(I_dev_Char *self, void *testBuf, int len)
{


}

