#ifndef _INC_HMIfactory_H_
#define _INC_HMIfactory_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
#include "HMI_comm.h"
#include "HMI_main.h"
#include "HMI_keyboard.h"
#include "HMI_menu.h"
#include "HMI_barGraph.h"
#include "HMI_data.h"
#include "HMI_curve.h"
#include "HMI_news_select.h"
//#include "News_Alarm_HMI.h"
//#include "News_Power_down_HMI.h"
//#include "History_HMI.h"
#include "HMI_Accumulate.h"
#include "HMI_setup.h"
#include "HMI_striped_background.h"
#include "HMI_windows.h"

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
//#define HMI_NEWS_ALARM			9
//#define HMI_NEWS_POWER_DOWN		10
//#define HMI_HISTORY				11
#define HMI_ACCM				12
#define HMI_SETUP				13
#define HMI_STRIPED_BKG				14
#define HMI_WINDOWS				15
 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// global variable declarations


//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
HMI *Create_HMI( int type);

#endif
