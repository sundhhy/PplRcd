#include "ModelTime.h"
#include <string.h>
#include "utils/rtc_pcf8563.h"
#include "mem/CiiMem.h"
#include "sdhDef.h"

//#include "utils/rtc.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

const	char	g_moth_day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
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
static char s_timer[16];

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
/* Cycle/Sync Callback functions */
static char* MdlTime_to_string( Model *self, IN int aux, void *arg);
static int MdlTime_set_by_string( Model *self, IN int aux, void *arg);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int MdlTime_init( Model *self, IN void *arg)
{
	UtlRtc *rtc = ( UtlRtc *)Pcf8563_new();
	struct  tm	*tm = CALLOC( 1, sizeof( *tm));
//	self->dataSource = Pcf8563_new();
	rtc->init( rtc, NULL);
	
	rtc->get( rtc, tm);
	
	
	
	self->dataSource = rtc;
	self->coreData = tm;
	self->crDt_len = sizeof( *tm);
	
	return RET_OK;
}

int MdlTime_getData(  Model *self, IN int aux, void *arg) 
{
	struct  tm	*tm = ( struct  tm	*) self->coreData;
	struct  tm	*tm2 = NULL;
	tm->tm_sec ++;
	if( tm->tm_sec >59)
	{
		tm->tm_min ++;
		tm->tm_sec = 0;
		
	}
	
	if( tm->tm_min >59)
	{
		tm->tm_hour ++;
		tm->tm_min = 0;
		
	}
	
	if(arg != NULL) {
		tm2 = ( struct  tm	*) arg;
		tm2->tm_hour = tm->tm_hour;
		tm2->tm_min = tm->tm_min;
		tm2->tm_sec = tm->tm_sec;
	}
	
//	memcpy( self->coreData, arg, self->crDt_len);
	self->notify( self);
	return RET_OK;
}
CTOR( ModelTime)
SUPER_CTOR( Model);
FUNCTION_SETTING( Model.init, MdlTime_init);
FUNCTION_SETTING( Model.getMdlData, MdlTime_getData);
FUNCTION_SETTING( Model.set_by_string, MdlTime_set_by_string);

FUNCTION_SETTING( Model.to_string, MdlTime_to_string);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{




static char* MdlTime_to_string( Model *self, IN int aux, void *arg)
{
	struct  tm	*p_tm= (struct  tm	*) self->coreData;
	switch(aux) {
		case 0:
			snprintf(s_timer, 16, "%02d:%02d:%02d", p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
			return s_timer;
		case 1:
			if(arg == NULL)
				break;
			sprintf(arg, "%02d:%02d:%02d %02d:%02d:%02d", p_tm->tm_year, p_tm->tm_mon, p_tm->tm_mday, \
				p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
			return arg;
		default:break;
				

	}
	return s_timer;
	
}


static int MdlTime_set_by_string( Model *self, IN int aux, void *arg)
{
	struct  tm	t = {0};
	struct  tm	*p_tm= (struct  tm	*) self->coreData;
	short		i;
	short		val;
	char	s_data[4];
	switch(aux) {
		case 0:
			return RET_OK;
		case 1:
			//"%02d:%02d:%02d %02d:%02d:%02d", p_tm->tm_year, p_tm->tm_mon, p_tm->tm_mday, \
				p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec
			if(arg == NULL)
				break;
			for(i = 0; i < 6; i+=3) {
				
				memcpy(s_data, (char *)arg + i, 2);
				s_data[2] = 0;
				val = atoi(s_data);
				if(i == 0) {
					val += 2000;
					t.tm_year = val;
					
				} else if(i == 1) {
					if(val > 12)
						return -1;
					t.tm_mon = val;
					
				} else if(i == 2) {
					if(val > g_moth_day[t.tm_mon])
						return -1;
					t.tm_mday = val;
					
				} else if(i == 3) {
					if(val > 23)
						return -1;
					t.tm_hour = val;
					
				} else if(i == 4) {
					if(val > 59)
						return -1;
					t.tm_min = val;
					
				} else if(i == 5) {
					if(val > 59)
						return -1;
					t.tm_sec = val;
					
				}
				
				
			}
			
			p_tm->tm_year = t.tm_year;
			p_tm->tm_mon = t.tm_mon;
			p_tm->tm_mday = t.tm_mday;
			p_tm->tm_hour = t.tm_hour;
			p_tm->tm_min = t.tm_min;
			p_tm->tm_sec = t.tm_sec;
			
			return RET_OK;
		default:break;
				

	}
	return -1;
	
}
