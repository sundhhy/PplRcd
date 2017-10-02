#ifndef _INC_commHMI_H_
#define _INC_commHMI_H_
#include "HMI.h"
#include "ModelFactory.h"
#include "sheet.h"
#include "keyboardHMI.h"
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
#define NUM_CHANNEL			6
 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
CLASS( cmmHmi)
{
	Model			*p_mdlRtc;
//	shtctl			*p_shtctl;
//	sheet			*p_shtTime;
	EXTENDS( HMI);
	
	
	
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern	sheet		*g_p_shtTime, *g_p_ico_memu, *g_p_ico_bar, *g_p_ico_digital, *g_p_ico_trend;
extern  sheet		*g_arr_p_chnData[NUM_CHANNEL];
extern	sheet		*g_arr_p_chnUtil[NUM_CHANNEL];
extern	sheet		*g_arr_p_chnAlarm[NUM_CHANNEL];

extern	const char	arr_clrs[NUM_CHANNEL];
extern	hmiAtt_t	CmmHmiAtt;
extern keyboardHMI		*g_keyHmi;
extern sheet			*g_p_sht_title;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
cmmHmi *GetCmmHMI(void);

extern void Build_ChnSheets(void);
#endif
