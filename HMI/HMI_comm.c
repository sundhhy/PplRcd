#include "HMI_comm.h"
#include <string.h>
#include "ExpFactory.h"
#include "HMIFactory.h"
#include "ModelFactory.h"


#include "utils/time.h"
#include "format.h"
#include "focus.h"

#include "chnInfoPic.h"

#include "os/os_depend.h"

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
//sheet			*g_arr_p_chnUtil[NUM_CHANNEL];
//sheet			*g_arr_p_chnAlarm[NUM_CHANNEL];

char		prn_buf[NUM_CHANNEL][8];


hmiAtt_t CmmHmiAtt = { 10,1, COLOUR_BLACK, 4, 2};

const char	arr_clrs[NUM_CHANNEL] = { 43, COLOUR_GREN, COLOUR_BLUE, COLOUR_YELLOW, \
	COLOUR_BABYBLUE, COLOUR_PURPLE};

ro_char news_cpic[] =  {"<cpic vx0=0 vx1=320 vy0=50 vy1=210>16</>" };

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

static ro_char code_bkPic[] =  {"<bpic vx0=0 vy0=0 m=0 >23</>" };
static ro_char code_title[] =  {"<text vx0=0 vy0=4 m=0 clr=white f=24> </>" };

static ro_char timeCode[] = { "<text vx0=220 vy0=0 bx=60  by=24 f=24 xali=r m=0 clr=yellow spr=/> </>" };



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

static void Build_ChnSheets(void);
static void Build_icoSheets(void);
static void Build_otherSheets(void);


static int CMM_Update_time(mdl_observer *self, void *p_srcMdl);


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

int	Operate_in_range(int	arg1, int op, int arg2, int rangel, int rangeh)
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

//超上下限的时候，不要反转
int	Operate_in_range_keep(int	arg1, int op, int arg2, int rangel, int rangeh)
{
	int	ret = 0;
	
	if(op == OP_ADD) {
		ret = arg1 + arg2;
		if(ret > rangeh) 
			ret = rangeh;
		
	} else if(op == OP_SUB) {
		ret = arg1 - arg2;
		if(ret < rangel)
			ret = rangel;
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
FUNCTION_SETTING(mdl_observer.update, CMM_Update_time);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_cmmHmi( HMI *self, void *arg)
{
	Focus_init();
	
	
	Build_ChnSheets();
	Build_icoSheets();
	Build_otherSheets();
	
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
	cmmHmi		*cthis = GetCmmHMI();
	Model		*p_time = Create_model("time");
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
	
	p_time->attach(p_time, &cthis->mdl_observer);
	g_p_shtTime->cnt.data = p_time->to_string(p_time, 0, NULL);
	g_p_shtTime->cnt.len = strlen(g_p_shtTime->cnt.data);
	
}

static int CMM_Update_time(mdl_observer *self, void *p_srcMdl)
{
	
	Model	*p_mdl = (Model *)p_srcMdl;
	
	if(Sheet_is_hide(g_p_shtTime))
		return RET_OK;
	if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1) <= 0)
		//180522 时间本来就是周期性更新，失败了一次也无所谓。之前这返回错误，可能是导致屏幕显示不全的原因
			//因为这让模型不断的去重新执行notify，导致屏幕频繁的刷新
		return RET_OK;		
	
	g_p_shtTime->cnt.data = p_mdl->to_string(p_mdl, 0, NULL);
	g_p_shtTime->cnt.len = strlen( g_p_shtTime->cnt.data);
	Sheet_slide(g_p_shtTime);
	
	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	return RET_OK;
	
}

static void Build_ChnSheets(void)
{
	int 		i = 0;
	shtctl 		*p_shtctl = NULL;
//	Expr 		*p_exp ;
//	Model			*p_mdl = NULL;
//	char		mdl_code[16] = {0};
	p_shtctl = GetShtctl();
	
//	Bulid_ChnData(g_arr_p_chnData, (void *)MAIN_hmi_code_data, MainHmi_Data_update);
	
//	p_exp = ExpCreate( "text");
	for(i = 0; i < NUM_CHANNEL; i++) {
		g_arr_p_chnData[i] = Sheet_alloc( p_shtctl);
//		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_data, g_arr_p_chnData[i]) ;
//		sprintf(mdl_code,"chn_%d", i);

//		g_arr_p_chnData[i]->p_mdl = Create_model(mdl_code);
//		g_arr_p_chnData[i]->update = MainHmi_Data_update;
		g_arr_p_chnData[i]->id = i;
//		p_mdl = g_arr_p_chnData[i]->p_mdl;
//		p_mdl->attach(p_mdl, (mdl_observer *)g_arr_p_chnData[i]);
		
//		g_arr_p_chnUtil[i] = Sheet_alloc( p_shtctl);
//		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_unit, g_arr_p_chnUtil[i]) ;
//		g_arr_p_chnUtil[i]->id = i;
//		g_arr_p_chnUtil[i]->p_mdl = Create_model(mdl_code);
//		g_arr_p_chnUtil[i]->update = MainHmi_Util_update;
//		g_arr_p_chnUtil[i]->cnt.subType = TEXT_ST_UNTIL;
//		p_mdl = g_arr_p_chnUtil[i]->p_mdl;
//		p_mdl->attach(p_mdl, (mdl_observer *)g_arr_p_chnUtil[i]);
		
//		g_arr_p_chnAlarm[i] = Sheet_alloc( p_shtctl);
//		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_alarm, g_arr_p_chnAlarm[i]) ;
//		g_arr_p_chnAlarm[i]->id = i;
//		g_arr_p_chnAlarm[i]->p_mdl = Create_model(mdl_code);
//		g_arr_p_chnAlarm[i]->update = MainHmi_Alarm_update;
//		p_mdl = g_arr_p_chnAlarm[i]->p_mdl;
//		p_mdl->attach(p_mdl, (mdl_observer *)g_arr_p_chnAlarm[i]);
		
		
		
	}
	
}







