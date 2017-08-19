#include "commHMI.h"
#include <string.h>
#include "ExpFactory.h"
#include "utils/time.h"



//提供 按键，事件，消息，窗口，报警，时间，复选框的图层
//这些图层可能会被其他界面所使用
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
sheet			*g_p_shtTime;

hmiAtt_t	CmmHmiAtt = { 10,1, COLOUR_BLACK, 4, 2};
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define TIME_BUF_LEN		16


const char timeCode[] = { "<time vx0=240 vy0=0 bx=60  by=24 f=24 xali=m bkc=black clr=yellow spr=/> </time>" };
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static cmmHmi *singalCmmHmi;
static char s_timer[TIME_BUF_LEN];
static struct  tm time;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_cmmHmi( HMI *self, void *arg);
static void Timer2str( struct  tm *p_tm, char *s, int n);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
cmmHmi *GetCmmHMI(void)
{
	if( singalCmmHmi == NULL)
	{
		singalCmmHmi = cmmHmi_new();
		
	}
	
	return singalCmmHmi;
	
}


CTOR( cmmHmi)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_cmmHmi);
//FUNCTION_SETTING( View.show, TestView_show);
END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_cmmHmi( HMI *self, void *arg)
{
//	cmmHmi	*cthis = SUB_PTR( self, HMI, cmmHmi);
	shtctl *p_shtctl = NULL;
	Expr *p_exp ;
	
	p_shtctl = GetShtctl();
	
	
	g_p_shtTime = Sheet_alloc( p_shtctl);
	
	
	p_exp = ExpCreate( "text");
	p_exp->inptSht( p_exp, (void *)timeCode, g_p_shtTime) ;
	
	g_p_shtTime->p_mdl = ModelCreate("time");
	g_p_shtTime->p_mdl->attach( g_p_shtTime->p_mdl, (Observer *)g_p_shtTime);
	g_p_shtTime->p_mdl->getMdlData( g_p_shtTime->p_mdl, 0, &time);

	Timer2str( &time, s_timer, TIME_BUF_LEN);
	g_p_shtTime->cnt.data = s_timer;
	g_p_shtTime->cnt.len = strlen( s_timer);
	
	
	return RET_OK;
}



static void Timer2str( struct  tm *p_tm, char *s, int n)
{
	
	
	snprintf( s, n, "%02d:%02d:%02d", p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
	
}













