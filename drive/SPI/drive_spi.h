//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_drive_spi_H__
#define __INC_drive_spi_H__
#include "stdint.h"
#include "stm32f10x_spi.h"
#include "lw_oopc.h"
#include "mem/Ping_PongBuf.h"
#include "dri_cmd.h"
#include "hardwareConfig.h"
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
CLASS( driveSpi)
{
    char                *rxCache;
    char                *txCache;
    void                *devBase;
    void	             *cfg;
    void                *device;
   
    int (*init)(driveSpi *self, void *dev, void *cfg);
    int (*deInit)(driveSpi *self);
    int (*read)(driveSpi *self, void *buf, int rdLen);
    int (*write)(driveSpi *self, void *buf, int wrLen);
    int (*ioctol)(driveSpi *self, int cmd, ...);
    int (*test)(driveSpi *self, void *testBuf, int bufSize);
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------


#endif

