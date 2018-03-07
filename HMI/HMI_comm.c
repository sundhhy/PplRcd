#include "HMI_comm.h"
#include <string.h>
#include "ExpFactory.h"
#include "HMIFactory.h"

#include "utils/time.h"
#include "format.h"
#include "focus.h"

#include "chnInfoPic.h"


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
sheet			*g_p_sht_bkpic;
sheet			*g_p_sht_title;
sheet			*g_p_shtTime;
sheet			*g_p_cpic;
sheet			*g_p_text;
sheet			*g_p_boxlist;

//sheet			*g_p_ico_memu;
//sheet			*g_p_ico_bar;
//sheet			*g_p_ico_digital;
//sheet			*g_p_ico_trend;
//sheet			*g_p_ico_pgup;
//sheet			*g_p_ico_pgdn;
//sheet			*g_p_ico_search;
//sheet			*g_p_ico_eraseTool;

sheet			*g_arr_p_chnData[NUM_CHANNEL];
sheet			*g_arr_p_chnUtil[NUM_CHANNEL];
sheet			*g_arr_p_chnAlarm[NUM_CHANNEL];

char		prn_buf[BARHMI_NUM_BARS][8];


hmiAtt_t CmmHmiAtt = { 10,1, COLOUR_BLACK, 4, 2};

const char	arr_clrs[NUM_CHANNEL] = { 43, COLOUR_GREN, COLOUR_BLUE, COLOUR_YELLOW, \
	COLOUR_BABYBLUE, COLOUR_PURPLE};

ro_char news_cpic[] =  {"<cpic vx0=0 vx1=320 vy0=50 vy1=210>16</>" };

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------
keyboardHMI		*g_keyHmi;
//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define TIME_BUF_LEN		16

static ro_char code_bkPic[] =  {"<bpic vx0=0 vy0=0 m=0 >23</>" };
static ro_char code_title[] =  {"<text vx0=0 vy0=4 m=0 clr=white f=24> </>" };

static ro_char timeCode[] = { "<time vx0=220 vy0=0 bx=60  by=24 f=24 xali=r m=0 clr=yellow spr=/> </time>" };

////进入主菜单
//static ro_char ico_memu[] = { "<bu vx0=10 vy0=212 bx=49 by=25 bkc=black clr=black><pic bx=48  by=24 >20</></bu>" };
////进入棒图图标
//static ro_char ico_bar[] = { "<bu vx0=80 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >21</></bu>" };
////进入数显画面图标
//static ro_char ico_digital[] = { "<bu vx0=160 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >22</></bu>" };
////进入趋势画面图标
//static ro_char ico_trend[] = { "<bu vx0=240 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >23</></bu>" };

//static ro_char ico_pgup[] = { "<bu vx0=80 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >25</></bu>" };
//static ro_char ico_pgdn[] = { "<bu vx0=160 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >26</></bu>" };
//static ro_char ico_eraseTool[] = {"<bu vx0=240 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >27</></bu>"};
//static ro_char ico_search[] = {"<bu vx0=240 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >24</></bu>"};




//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//只会从头向后分配，而且不考虑空洞
//用完之后不必回收，在执行HMI_Ram_init 的时候会把内存全部回收
typedef struct {
	uint16_t		used_bytes;		//已经被使用
	uint16_t		free_bytes;
	uint8_t			vram_buf[1448];		//NUM_CHANNEL * (CURVE_POINT + 1)
}hmi_ram_mgr_t;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static cmmHmi *singalCmmHmi;
static hmi_ram_mgr_t	hmi_ram;
//static char s_timer[TIME_BUF_LEN];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_cmmHmi( HMI *self, void *arg);

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
		if(singalCmmHmi  == NULL) while(1);
		
	}
	
	return singalCmmHmi;
	
}

int Stripe_clean_clr(int row)
{
	if(row & 1) {
		
		return STRIPE_CLR_2;
	} else {
		
		return STRIPE_CLR_1;
	}
	
}

int Stripe_vy(int row) 
{
	int y;
	if(row == 0)
		y = STRIPE_VY0 ;
	else 
		y = STRIPE_VY1 + (row - 1)* STRIPE_SIZE_Y;
	return y;
}

int	Operate_in_tange(int	arg1, int op, int arg2, int rangel, int rangeh)
{
	int	ret = 0;
	
	if(op == OP_ADD) {
		ret = arg1 + arg2;
		if(ret > rangeh) 
			ret = rangel;
		
	} else if(op == OP_SUB) {
		ret = arg1 - arg2;
		if(ret < rangel)
			ret = rangeh;
	}
	return ret;
}



//一个简单的内存分配
//把用于曲线显示的大量内存，用于其他界面上的显存
//每次要分配内存之前都要重新初始化下
//因此这种内存在切换界面之后，之前的内存就会被回收
//界面在使用这种显存的时候，都要事先进行初始化
void HMI_Ram_init(void)
{
	hmi_ram_mgr_t *p_ram = &hmi_ram;
	
	p_ram->free_bytes = sizeof(p_ram->vram_buf);
	p_ram->used_bytes = 0;
//	int i = 0;
//	
//	for(i = 0; i < NUM_CHANNEL; i++) {
//		
//		p_vram->free_idx[i] = 0;
//		
//	}
	
//	p_vram->free_idx[0] = sizeof(vram_mgr_t);
	
	
	
}

uint16_t HMI_Ram_free_bytes(void)
{
	hmi_ram_mgr_t *p_ram = &hmi_ram;
	return p_ram->free_bytes;
}

//分配算法是最简单的，第一个匹配地址

void *HMI_Ram_alloc(int bytes)
{
	hmi_ram_mgr_t *p_ram = &hmi_ram;
	void	*p;
	
	if(bytes < 0 )
		return NULL;
	if(p_ram->free_bytes < bytes)
		return NULL;
	p = p_ram->vram_buf + p_ram->used_bytes;
	p_ram->used_bytes += bytes;
	p_ram->free_bytes -= bytes;
//	int i = 0;
	
//	for(i = 0; i < NUM_CHANNEL; i++) {
//		
//		if((CURVE_POINT - p_vram->free_idx[i]) >=  bytes) {
//			p = g_curve[i].points + p_vram->free_idx[i];
//			p_vram->free_idx[i] += bytes;
//			return p;
//				
//		}
//		
//	}
	
	return p;
	
	
	
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
//	menuHMI			*menuHmi ;
//	barGhHMI		*barHmi ;
//	dataHMI			*dataHmi;
//	RLT_trendHMI	*rltHmi;
//	shtctl *p_shtctl = NULL;
//	Expr *p_exp ;
	
	Focus_init();
	
	
	Build_ChnSheets();
	Build_icoSheets();
	Build_otherSheets();
	
	//注意：按键一定要放在最开始初始化，因为其他界面会依赖按键的一些操作
	p_hmi = CreateHMI(HMI_KYBRD);
	p_hmi->init(p_hmi, NULL);
	g_keyHmi = SUB_PTR(p_hmi, HMI, keyboardHMI);
	
	//创建与公用图标相关的界面
	p_hmi = CreateHMI(HMI_MENU);
	p_hmi->init( p_hmi, NULL);
//	menuHmi = SUB_PTR(p_hmi, HMI, menuHMI);
	
	p_hmi = CreateHMI(HMI_DATA);
	p_hmi->init(p_hmi, NULL);
//	dataHmi = SUB_PTR(p_hmi, HMI, dataHMI);
	
	p_hmi = CreateHMI(HMI_BAR);
	p_hmi->init( p_hmi, NULL);
//	barHmi = SUB_PTR(p_hmi, HMI, barGhHMI);
	
	p_hmi = CreateHMI(HMI_RLT_TREND);
	p_hmi->init(p_hmi, NULL);
//	rltHmi = SUB_PTR(p_hmi, HMI, RLT_trendHMI);
	
	//将图标动作与相关界面处理绑定
//	g_p_ico_memu->p_enterCmd = &menuHmi->shtCmd;
//	g_p_ico_digital->p_enterCmd = &dataHmi->shtCmd;
//	g_p_ico_bar->p_enterCmd = &barHmi->shtCmd;
//	g_p_ico_trend->p_enterCmd = &rltHmi->shtCmd;
	
	
	
	
	//初始化其他界面
	
	
	
	
	p_hmi = CreateHMI(HMI_NWS);
	p_hmi->init(p_hmi, NULL);
	
//	p_hmi = CreateHMI(HMI_NEWS_ALARM);
//	p_hmi->init(p_hmi, NULL);
//	
//	p_hmi = CreateHMI(HMI_NEWS_POWER_DOWN);
//	p_hmi->init(p_hmi, NULL);
//	
//	p_hmi = CreateHMI(HMI_HISTORY);
//	p_hmi->init(p_hmi, NULL);
	
	p_hmi = CreateHMI(HMI_ACCM);
	p_hmi->init(p_hmi, NULL);
	
	p_hmi = CreateHMI(HMI_SETUP);
	p_hmi->init(p_hmi, NULL);
	
	
	p_hmi = CreateHMI(HMI_SETTING);
	p_hmi->init(p_hmi, NULL);
	
	p_hmi = CreateHMI(HMI_WINDOWS);
	p_hmi->init(p_hmi, NULL);
	return RET_OK;
}







static void Build_icoSheets(void)
{
//	shtctl 		*p_shtctl = NULL;
//	Expr 		*p_exp ;
//		
//	p_shtctl = GetShtctl();
//	
//	//图标初始化
//	p_exp = ExpCreate( "bu");

//	
//	//初始化公用图标
//	g_p_ico_memu  = Sheet_alloc(p_shtctl);
//	p_exp->inptSht(p_exp, (void *)ico_memu, g_p_ico_memu) ;
//	g_p_ico_memu->area.x1 = g_p_ico_memu->area.x0 + g_p_ico_memu->bxsize;
//	g_p_ico_memu->area.y1 = g_p_ico_memu->area.y0 + g_p_ico_memu->bysize;
//	g_p_ico_memu->id = ICO_ID_MENU;
//	FormatSheetSub(g_p_ico_memu);
//	
//	
//	g_p_ico_bar  = Sheet_alloc(p_shtctl);
//	p_exp->inptSht(p_exp, (void *)ico_bar, g_p_ico_bar) ;
//	g_p_ico_bar->area.x1 = g_p_ico_bar->area.x0 + g_p_ico_bar->bxsize;
//	g_p_ico_bar->area.y1 = g_p_ico_bar->area.y0 + g_p_ico_bar->bysize;
//	FormatSheetSub(g_p_ico_bar);
//	
//	
//	g_p_ico_digital  = Sheet_alloc(p_shtctl);
//	p_exp->inptSht(p_exp, (void *)ico_digital, g_p_ico_digital) ;
//	g_p_ico_digital->area.x1 = g_p_ico_digital->area.x0 + g_p_ico_bar->bxsize;
//	g_p_ico_digital->area.y1 = g_p_ico_digital->area.y0 + g_p_ico_bar->bysize;
//	FormatSheetSub(g_p_ico_digital);
//	
//	g_p_ico_trend  = Sheet_alloc(p_shtctl);
//	p_exp->inptSht(p_exp, (void *)ico_trend, g_p_ico_trend) ;
//	g_p_ico_trend->area.x1 = g_p_ico_trend->area.x0 + g_p_ico_trend->bxsize;
//	g_p_ico_trend->area.y1 = g_p_ico_trend->area.y0 + g_p_ico_trend->bysize;
//	FormatSheetSub(g_p_ico_trend);
//	
//	g_p_ico_pgup  = Sheet_alloc(p_shtctl);
//	p_exp->inptSht(p_exp, (void *)ico_pgup, g_p_ico_pgup) ;
//	g_p_ico_pgup->area.x1 = g_p_ico_pgup->area.x0 + g_p_ico_pgup->bxsize;
//	g_p_ico_pgup->area.y1 = g_p_ico_pgup->area.y0 + g_p_ico_pgup->bysize;
//	g_p_ico_pgup->id = ICO_ID_PGUP;
//	FormatSheetSub(g_p_ico_pgup);
//	
//	g_p_ico_pgdn  = Sheet_alloc( p_shtctl);
//	p_exp->inptSht(p_exp, (void *)ico_pgdn, g_p_ico_pgdn) ;
//	g_p_ico_pgdn->area.x1 = g_p_ico_pgdn->area.x0 + g_p_ico_pgdn->bxsize;
//	g_p_ico_pgdn->area.y1 = g_p_ico_pgdn->area.y0 + g_p_ico_pgdn->bysize;
//	g_p_ico_pgdn->id = ICO_ID_PGDN;
//	FormatSheetSub(g_p_ico_pgdn);
//	
//	
//	g_p_ico_eraseTool  = Sheet_alloc(p_shtctl);
//	p_exp->inptSht(p_exp, (void *)ico_eraseTool, g_p_ico_eraseTool) ;
//	g_p_ico_eraseTool->area.x1 = g_p_ico_eraseTool->area.x0 + g_p_ico_eraseTool->bxsize;
//	g_p_ico_eraseTool->area.y1 = g_p_ico_eraseTool->area.y0 + g_p_ico_eraseTool->bysize;
//	g_p_ico_eraseTool->id = ICO_ID_ERASETOOL;
//	FormatSheetSub(g_p_ico_eraseTool);
//	
//	g_p_ico_search  = Sheet_alloc(p_shtctl);
//	p_exp->inptSht(p_exp, (void *)ico_eraseTool, g_p_ico_search) ;
//	g_p_ico_search->area.x1 = g_p_ico_search->area.x0 + g_p_ico_search->bxsize;
//	g_p_ico_search->area.y1 = g_p_ico_search->area.y0 + g_p_ico_search->bysize;
//	g_p_ico_search->id = ICO_ID_ERASETOOL;
//	FormatSheetSub(g_p_ico_search);
	
}

static void Build_otherSheets(void)
{
	shtctl 		*p_shtctl = NULL;
	Expr 		*p_exp ;
		
	p_shtctl = GetShtctl();
	
	g_p_cpic = Sheet_alloc( p_shtctl);
	g_p_text = Sheet_alloc( p_shtctl);
	g_p_boxlist = Sheet_alloc( p_shtctl);
	g_p_boxlist->id = SHEET_BOXLIST;
	g_p_text->id = SHEET_G_TEXT;
	
	g_p_sht_bkpic = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)code_bkPic, g_p_sht_bkpic) ;
	
	
	
	//title
	g_p_sht_title = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "text");
	p_exp->inptSht( p_exp, (void *)code_title, g_p_sht_title) ;
	
	//timer
	g_p_shtTime = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "text");
	p_exp->inptSht( p_exp, (void *)timeCode, g_p_shtTime) ;
	
	g_p_shtTime->p_mdl = ModelCreate("time");
	g_p_shtTime->p_mdl->attach( g_p_shtTime->p_mdl, (Observer *)g_p_shtTime);
	g_p_shtTime->cnt.data = g_p_shtTime->p_mdl->to_string(g_p_shtTime->p_mdl, 0, NULL);
	g_p_shtTime->cnt.len = strlen(g_p_shtTime->cnt.data);
	
}











