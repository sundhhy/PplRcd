#include "commHMI.h"
#include <string.h>
#include "ExpFactory.h"
#include "HMIFactory.h"

#include "utils/time.h"
#include "format.h"



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
sheet			*g_p_ico_memu;
sheet			*g_p_ico_bar;
sheet			*g_p_ico_digital;
sheet			*g_p_ico_trend;

sheet			*g_arr_p_chnUtil[NUM_CHANNEL];
sheet			*g_arr_p_chnAlarm[NUM_CHANNEL];

hmiAtt_t CmmHmiAtt = { 10,1, COLOUR_BLACK, 4, 2};

const char	arr_clrs[NUM_CHANNEL] = { 43, COLOUR_GREN, COLOUR_BLUE, COLOUR_YELLOW, \
	COLOUR_BABYBLUE, COLOUR_PURPLE};
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

const char ico_memu[] = { "<bu vx0=10 vy0=206 bx=33 by=33 bkc=black clr=black><pic  bx=32  by=32 >1</></bu>" };
//进入棒图图标
const char ico_bar[] = { "<bu vx0=50 vy0=206 bx=33 by=33 bkc=black clr=black><pic  bx=32  by=32 >2</></bu>" };
//进入数显画面图标
const char ico_digital[] = { "<bu vx0=90 vy0=206 bx=33 by=33bkc=black clr=black><pic  bx=32  by=32 >3</></bu>" };
//进入趋势画面图标
const char ico_trend[] = { "<bu vx0=130 vy0=206 bx=33 by=33 bkc=black clr=black><pic  bx=32  by=32 >4</></bu>" };


//每个通道的单位
static const char cmmhmi_code_unit[] = { "<text f=16 m=0 mdl=test aux=1>m3/h</>" };

//通道报警:HH HI LI LL
static const char cmmhmi_code_alarm[] = { "<text f=16 clr=red m=0 mdl=test aux=2> </>" };

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

static void Build_ChnSheets(void);
static void Build_icoSheets(void);
static void Build_otherSheets(void);

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
	HMI 			*p_hmi;
	menuHMI			*menuHmi ;
	barGhHMI		*barHmi ;
	shtctl *p_shtctl = NULL;
	Expr *p_exp ;
	
	
	Build_ChnSheets();
	Build_icoSheets();
	Build_otherSheets();
	
	//创建与公用图标相关的界面
	p_hmi = CreateHMI( HMI_MENU);
	p_hmi->init( p_hmi, NULL);
	menuHmi = SUB_PTR( p_hmi, HMI, menuHMI);
	
	p_hmi = CreateHMI( HMI_BAR);
	p_hmi->init( p_hmi, NULL);
	barHmi = SUB_PTR( p_hmi, HMI, barGhHMI);
	
	//初始化其他界面
	p_hmi = CreateHMI( HMI_DATA);
	p_hmi->init( p_hmi, NULL);
	
	p_hmi = CreateHMI( HMI_RLT_TREND);
	p_hmi->init( p_hmi, NULL);
	
	
	//将图标动作与相关界面处理绑定
	g_p_ico_memu->p_enterCmd = &menuHmi->shtCmd;
	g_p_ico_bar->p_enterCmd = &barHmi->shtCmd;
	
	return RET_OK;
}



static void Timer2str( struct  tm *p_tm, char *s, int n)
{
	
	
	snprintf( s, n, "%02d:%02d:%02d", p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
	
}

static void Build_ChnSheets(void)
{
	int 		i = 0;
	shtctl 		*p_shtctl = NULL;
	Expr 		*p_exp ;
		
	p_shtctl = GetShtctl();
	
	p_exp = ExpCreate( "text");
	for(i = 0; i < NUM_CHANNEL; i++) {
		g_arr_p_chnUtil[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)cmmhmi_code_unit, g_arr_p_chnUtil[i]) ;
		g_arr_p_chnAlarm[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)cmmhmi_code_alarm, g_arr_p_chnAlarm[i]) ;
		
	}
	
}

static void Build_icoSheets(void)
{
	shtctl 		*p_shtctl = NULL;
	Expr 		*p_exp ;
		
	p_shtctl = GetShtctl();
	
	//图标初始化
	p_exp = ExpCreate( "bu");

	
	//初始化公用图标
	g_p_ico_memu  = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)ico_memu, g_p_ico_memu) ;
	g_p_ico_memu->area.x1 = g_p_ico_memu->area.x0 + g_p_ico_memu->bxsize;
	g_p_ico_memu->area.y1 = g_p_ico_memu->area.y0 + g_p_ico_memu->bysize;
	FormatSheetSub( g_p_ico_memu);
	
	
	g_p_ico_bar  = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)ico_bar, g_p_ico_bar) ;
	g_p_ico_bar->area.x1 = g_p_ico_bar->area.x0 + g_p_ico_bar->bxsize;
	g_p_ico_bar->area.y1 = g_p_ico_bar->area.y0 + g_p_ico_bar->bysize;
	FormatSheetSub( g_p_ico_bar);
	
	
	g_p_ico_digital  = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)ico_digital, g_p_ico_digital) ;
	g_p_ico_digital->area.x1 = g_p_ico_digital->area.x0 + g_p_ico_bar->bxsize;
	g_p_ico_digital->area.y1 = g_p_ico_digital->area.y0 + g_p_ico_bar->bysize;
	FormatSheetSub( g_p_ico_digital);
	
	g_p_ico_trend  = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)ico_trend, g_p_ico_trend) ;
	g_p_ico_trend->area.x1 = g_p_ico_trend->area.x0 + g_p_ico_trend->bxsize;
	g_p_ico_trend->area.y1 = g_p_ico_trend->area.y0 + g_p_ico_trend->bysize;
	FormatSheetSub( g_p_ico_trend);
	
}

static void Build_otherSheets(void)
{
	shtctl 		*p_shtctl = NULL;
	Expr 		*p_exp ;
		
	p_shtctl = GetShtctl();
	
	
	
	//timer
	g_p_shtTime = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "text");
	p_exp->inptSht( p_exp, (void *)timeCode, g_p_shtTime) ;
	
	g_p_shtTime->p_mdl = ModelCreate("time");
	g_p_shtTime->p_mdl->attach( g_p_shtTime->p_mdl, (Observer *)g_p_shtTime);
	g_p_shtTime->p_mdl->getMdlData( g_p_shtTime->p_mdl, 0, &time);

	Timer2str( &time, s_timer, TIME_BUF_LEN);
	g_p_shtTime->cnt.data = s_timer;
	g_p_shtTime->cnt.len = strlen( s_timer);
	
}











