#ifndef _INC_HMIfactory_H_
#define _INC_HMIfactory_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
#include "commHMI.h"
#include "mainHMI.h"
#include "keyboardHMI.h"
#include "menuHMI.h"
#include "barGraphHMI.h"
#include "dataHMI.h"
#include "real_time_trendHmi.h"
#include "news_select_HMI.h"
#include "News_Alarm_HMI.h"
#include "News_Power_down_HMI.h"
//#include "History_HMI.h"
#include "Accumulate_HMI.h"
#include "Setup_HMI.h"
#include "Setting_HMI.h"


//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define HMI_MAIN				1
#define HMI_CMM					2
#define HMI_KYBRD				3
#define HMI_MENU				4
#define HMI_BAR					5
#define HMI_DATA				6
#define HMI_RLT_TREND			7
#define HMI_NWS					8
#define HMI_NEWS_ALARM			9
#define HMI_NEWS_POWER_DOWN		10
//#define HMI_HISTORY				11
#define HMI_ACCM				12
#define HMI_SETUP				13
#define HMI_SETTING				14

 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// global variable declarations


//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
HMI *CreateHMI( int type);

#endif
