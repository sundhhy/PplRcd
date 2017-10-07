#ifndef _INC_dataHMI_H_
#define _INC_dataHMI_H_
#include "HMI.h"
#include "commHMI.h"
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define BARHMI_NUM_BARS			NUM_CHANNEL
 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
CLASS( dataHMI)
{
	EXTENDS( HMI);
	IMPLEMENTS( shtCmd);
	IMPLEMENTS( Observer);
	
//	sheet  		*arr_p_sht_data[BARHMI_NUM_BARS];
//	sheet  		**arr_p_sht_unit;
//	sheet  		**arr_p_sht_alarm;
	char		flags;
	char 		none[3];
	
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern HMI *g_p_dataHmi;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
dataHMI *Get_dataHMI(void);

#endif