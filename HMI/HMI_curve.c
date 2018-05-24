#include "HMI_curve.h"
#include "HMI_menu.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include "chnInfoPic.h"
#include "ModelFactory.h"
#include "Component_curve.h"
#include "os/os_depend.h"
#include "utils/Storage.h"
#include "arithmetic/bit.h"

//柱状图在y坐标上，按100%显示的话是:71 -187 
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define CRV_MAX_PIXS			240  //曲线最大的像素点数量

#define HST_FLAG_DONE			1			//当前页面已经显示好了
#define HST_FLAG_LOOP			2	
#define HST_NEXT_PG				0
#define HST_PREV_PG				1

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI *g_p_RLT_trendHmi;

sheet  		*g_arr_p_check[NUM_CHANNEL]; 		//是否显示的指示图形
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------





//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define	RLTHMI_BKPICNUM		"14"
#define	RLTHMI_TITLE		"实时趋势"

#define HISTORY_TITLE		"历史趋势"

#define DEFAULT_MIN_DIV		1

#define CHNSHOW_ROW_SPACE		32
#define CTRV_OFFSET_Y			0		//各通道曲线在y轴上的偏移

static const char RT_hmi_code_chninfo[] =  {"<cpic vx0=260 vy0=30 vx1=320 vy1=240>23</>" };


static const char RT_hmi_code_div[] = { "<text vx0=8 vy0=36 f=16 m=0 clr=red> </>" };

static const char RT_hmi_code_data[] = { "<text f=16 vx0=285 m=0 aux=3>100</>" };
static const char RLT_hmi_code_chnshow[] ={ "<icon vx0=265 vy0=62 xn=5 yn=1 n=0>19</>" };

static const char RT_hmi_code_first_time[] = { "<text vx0=88 vy0=36 f=16 m=0 clr=red> </>" };


//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef void (*midv_change)(RLT_trendHMI *cthis, uint8_t new_mins);


struct {
	
	//每次显示完一页历史曲线的时候，记录下已经显示过的数量。这样在翻页的时候，就不必每次都重头开始寻找符合显示条件的记录了
	//注意，这样做的前提是假设存储器中的记录都是按照时间的先后顺序存放的。另外在往前翻页的时候，要把这个记录进行相应的回退，不然往前翻页就永远也不能显示了
	uint32_t			arr_hst_num[NUM_CHANNEL];		
	uint8_t				hst_flags;
	uint8_t				has_pgdn;		//执行过向下翻页
	uint8_t				rtv_reflush;	//为1时，实时曲线重绘。用于页面定期刷新
	
	uint8_t				init_att;	
	
	//这个参数是通过窗口设置的起始时间
	uint32_t			set_start_sec;			//设置的起始显示时间，为0时，把最早的历史曲线作为其实显示时间
	//每副画面上的实际起始时间，需要注意，这个值并不一定比set_start_sec大，因为用户可能向前翻页，使得本页实际起始时间超前于设置的起始时间，这是允许的
	//这个参数是可以通过上下翻页来改变的。并且如果第一次显示的时候为0 的话，可以用存储器的最早的记录时间来更新
	uint32_t			real_first_time_s;			
	char				set_first_tm_buf[32];
}hst_mgr;
					
	
	

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

//static curve_ctl_t *arr_p_crv[RLTHMI_NUM_CURVE];


static void RTV_midv_change(RLT_trendHMI *cthis, uint8_t new_mins);
static void HST_midv_change(RLT_trendHMI *cthis, uint8_t new_mins);

static midv_change	arr_mdiv_change[2] = {RTV_midv_change, HST_midv_change};

static sheet  		*p_curve_bkg;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_RT_trendHMI( HMI *self, void *arg);
static void RT_trendHmi_InitSheet( HMI *self, uint32_t att );
static void RT_trendHmi_HideSheet( HMI *self );
static void	RT_trendHmi_Show( HMI *self);


static void	RT_trendHmi_HitHandle( HMI *self, char kcd);

//焦点

static void	RLT_init_focus(HMI *self);
static void	RLT_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void	RLT_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);



//曲线
//static void HMI_CRV_Show_all_crvs( HMI *self);		//在切入本界面时，用于将初始的曲线画面

//每秒执行一次，在每次更新曲线时，是对全部的通道曲线进行更新
static void HMI_CRV_RTV_Run(HMI *self);
static void HMI_CRV_HST_Run(HMI *self);

//static void Bulid_rtCurveSheet( RLT_trendHMI *self);
static void RLT_Init_curve(HMI *self);
static void HST_Init(HMI *self);
static uint16_t HST_Num_rcds(uint8_t	mul);
static void HST_Flex(uint8_t new_mdiv, uint8_t old_mdiv);
static void HST_Move(RLT_trendHMI *cthis, uint8_t direction);


//数据
static void RLTHmi_Init_chnSht(void);

//键盘
//static int	RLT_div_input(void *self, void *data, int len);


static void HMI_CRV_Build_cmp(HMI *self);
static void RLT_btn_hdl(void *arg, uint8_t btn_id);

//历史曲线的起始时间
static void CRV_Set_first_time(HMI *self);
static int CRV_Win_cmd(void *p_rcv, int cmd,  void *arg);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

RLT_trendHMI *Get_RLT_trendHMI(void)
{
	static RLT_trendHMI *singal_RTTHmi = NULL;
	if( singal_RTTHmi == NULL)
	{
		singal_RTTHmi = RLT_trendHMI_new();
		if(singal_RTTHmi  == NULL) while(1);
		g_p_RLT_trendHmi = SUPER_PTR( singal_RTTHmi, HMI);
		
	}
	
	return singal_RTTHmi;
	
}

CTOR( RLT_trendHMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_RT_trendHMI);
FUNCTION_SETTING( HMI.initSheet, RT_trendHmi_InitSheet);
FUNCTION_SETTING( HMI.hide, RT_trendHmi_HideSheet);
FUNCTION_SETTING( HMI.show, RT_trendHmi_Show);
FUNCTION_SETTING( HMI.hmi_run, HMI_CRV_RTV_Run);

FUNCTION_SETTING( HMI.init_focus, RLT_init_focus);
FUNCTION_SETTING( HMI.clear_focus, RLT_clear_focus);
FUNCTION_SETTING( HMI.show_focus, RLT_show_focus);


FUNCTION_SETTING( HMI.hitHandle, RT_trendHmi_HitHandle);
FUNCTION_SETTING(HMI.build_component, HMI_CRV_Build_cmp);


//FUNCTION_SETTING(mdl_observer.update, RLT_trendHmi_MdlUpdata);		

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_RT_trendHMI( HMI *self, void *arg)
{
	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	Expr 						*p_exp ;
	shtctl 					*p_shtctl = NULL;
	short						i = 0;
	
	
	
//	Curve_init();
	
	p_shtctl = GetShtctl();
	
	
	
//	cthis->str_div[0] = DEFAULT_MIN_DIV;
	cthis->min_div = DEFAULT_MIN_DIV ;
	
	

//	Bulid_rtCurveSheet(cthis);
	
	
	//初始化通道勾选图形
	p_exp = ExpCreate( "pic");
	cthis->chn_show_map = 0xff;
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++) {
		g_arr_p_check[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)RLT_hmi_code_chnshow, g_arr_p_check[i]);
		g_arr_p_check[i]->area.y0 += i * CHNSHOW_ROW_SPACE;
		g_arr_p_check[i]->area.y1 += i * CHNSHOW_ROW_SPACE;
		g_arr_p_check[i]->id = SHTID_CHECK(i);
	}

	//初始化焦点
//	self->init_focus(self);
	
	
	self->flag = 0;
	return RET_OK;

}

static void HMI_CRV_Build_cmp(HMI *self)
{
	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	Button					*p = BTN_Get_Sington();
	Curve 					*p_crv = CRV_Get_Sington();
	curve_att_t			crv;
	short				i;
	short				num = 0;
	
	p->build_each_btn(0, BTN_TYPE_MENU, Main_btn_hdl, self);
//	p->build_each_btn(0, BTN_TYPE_PGUP, RLT_btn_hdl, self);
//	p->build_each_btn(1, BTN_TYPE_PGDN, RLT_btn_hdl, self);
//	p->build_each_btn(2, BTN_TYPE_LOOP, RLT_btn_hdl, self);
	
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++)
	{
		crv.crv_col = arr_clrs[i];
		crv.crv_direction = HMI_DIR_RIGHT;
		if(self->arg[0] == 0)
		{
			crv.crv_step_pix = 4 / cthis->min_div;
			num = cthis->min_div * 60;
		}
		else		//历史趋势就是像素倍数
		{
			crv.crv_step_pix = 16 / cthis->min_div;
			num = HST_Num_rcds(cthis->min_div);
		}
		
		//下面的尺寸要跟曲线的背景位置匹配
		crv.crv_x0 = 0;
		crv.crv_y0 = 50 + i * CTRV_OFFSET_Y;
		crv.crv_x1 = 240;
		crv.crv_y1 = 150 + i * CTRV_OFFSET_Y;		//210
		if(CTRV_OFFSET_Y == 0)
			crv.crv_y1 = 209;
		crv.crv_buf_size = CRV_MAX_PIXS + 1;
		
		
		if( num <= (CRV_MAX_PIXS + 1))
			crv.crv_max_num_data = num;
		else
			crv.crv_max_num_data = CRV_MAX_PIXS + 1;
		
		cthis->arr_crv_fd[i] = p_crv->alloc(&crv);
	}

}

static void RLT_btn_hdl(void *arg, uint8_t btn_id)
{
	HMI						*self	= (HMI *)arg;		
	RLT_trendHMI	*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	switch(btn_id)
	{
		case ICO_ID_LOOP:
			if(self->arg[0] == 1)
			{
				
				hst_mgr.hst_flags ^= HST_FLAG_LOOP;
				
			}
			break;
		case ICO_ID_PGDN:
			if(self->arg[0] == 1)
			{
				
				HST_Move(cthis, HST_NEXT_PG);
				
			}
			break;
		case ICO_ID_PGUP:
			if(self->arg[0] == 1)
			{
				
				HST_Move(cthis, HST_PREV_PG);
				
			}
			break;
		
			
	}
	
}



static void RT_trendHmi_InitSheet( HMI *self, uint32_t att )
{
	RLT_trendHMI	*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	int 			h = 0;
	int				i;
	
	p_shtctl = GetShtctl();
	
	p_exp = ExpCreate("pic");
	p_exp->inptSht(p_exp, (void *)RT_hmi_code_chninfo, g_p_cpic);
	
	p_exp = ExpCreate( "text");
	cthis->p_div = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)RT_hmi_code_div, cthis->p_div);
	sprintf(cthis->str_div, "%d", cthis->min_div);
	cthis->p_div->cnt.data = cthis->str_div;
	cthis->p_div->cnt.len = strlen(cthis->p_div->cnt.data);
	cthis->p_div->id = SHTID_RTL_MDIV;
	
	cthis->p_first_time = Sheet_alloc(p_shtctl);
	p_exp->inptSht(p_exp, (void *)RT_hmi_code_first_time, cthis->p_first_time);
	cthis->p_first_time->cnt.data = hst_mgr.set_first_tm_buf;
	cthis->p_first_time->id = SHT_FIRST_TIME;
	
	g_p_sht_bkpic->cnt.data = RLTHMI_BKPICNUM;

	if(self->arg[0] == 0) {
		g_p_sht_title->cnt.data = RLTHMI_TITLE;
		self->hmi_run = HMI_CRV_RTV_Run;
	} else if(self->arg[0] == 1) {
		g_p_sht_title->cnt.data = HISTORY_TITLE;
		self->hmi_run = HMI_CRV_HST_Run;
		
	} 
	g_p_sht_title->cnt.len = strlen(g_p_sht_title->cnt.data);

	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(p_curve_bkg, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
	Sheet_updown(cthis->p_div, h++);
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++) {
		Sheet_updown(g_arr_p_check[i], h++);
	}
	hst_mgr.init_att = att;
	RLTHmi_Init_chnSht();
	RLT_Init_curve(self);
	self->init_focus(self);
}

static void RT_trendHmi_HideSheet( HMI *self )
{
	RLT_trendHMI			*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	
	int i;
	
	
	for( i = phn_sys.sys_conf.num_chn - 1; i >= 0; i--) {
		Sheet_updown(g_arr_p_check[i], -1);
//		Sheet_updown(g_arr_p_chnData[i], -1);
	}
//	Sheet_updown( cthis->p_clean_chnifo, -1);
	Sheet_updown(cthis->p_div, -1);
//	Sheet_updown(g_p_ico_memu, -1);
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(p_curve_bkg, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	
	
//	self->clear_focus(self, 0, 0);
//	self->clear_focus( self, self->p_fcuu->focus_row, self->p_fcuu->focus_col);
	Sheet_free(cthis->p_first_time);
	Sheet_free(cthis->p_div);
	Focus_free(self->p_fcuu);
}	



static void	RLT_init_focus(HMI *self)
{
	short	col = 0, offset; 
	RLT_trendHMI *cthis = SUB_PTR(self, HMI, RLT_trendHMI);
	
	
	
	if(self->arg[0] == 1) {
		//历史曲线多一个设置起始时间的选项
		self->p_fcuu = Focus_alloc(1, phn_sys.sys_conf.num_chn + 2);
		
	} 
	else
	{
		self->p_fcuu = Focus_alloc(1, phn_sys.sys_conf.num_chn + 1);
		
	}
	
	
	
	Focus_Set_focus(self->p_fcuu, 0, 0);
	
	Focus_Set_sht(self->p_fcuu, 0, 0, cthis->p_div);
	offset = 1;
	
	if(self->arg[0] == 1) {
		Focus_Set_sht(self->p_fcuu, 0, 1, cthis->p_first_time);
		offset = 2;
	} 
	
	for(col = 0; col < phn_sys.sys_conf.num_chn; col ++) {
		Focus_Set_sht(self->p_fcuu, 0, col + offset, g_arr_p_check[col]);
	}		
	
}

static void	RLT_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	sheet *p_sht = Focus_Get_sht(self->p_fcuu, fouse_row, fouse_col);
	
	if(p_sht == NULL)
		return;
	if(IS_CHECK(p_sht->id)) {
		//这是更具勾选图标在图片中的位置来编码的
		if(p_sht->area.n == 1)
			p_sht->area.n = 0;
		else if(p_sht->area.n == 3)
			p_sht->area.n = 2;
	} else {
		p_sht->cnt.effects = GP_CLR_EFF( p_sht->cnt.effects, EFF_FOCUS);
	}
	Sheet_force_slide(p_sht);
}
static void	RLT_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	sheet *p_sht = Focus_Get_focus(self->p_fcuu);
	
	if(p_sht == NULL)
		return;
	
	if(IS_CHECK(p_sht->id)) {
		//这是更具勾选图标在图片中的位置来编码的
		if(p_sht->area.n == 0)
			p_sht->area.n = 1;
		else if(p_sht->area.n == 2)
			p_sht->area.n = 3;
	} else {
		p_sht->cnt.effects = GP_SET_EFF( p_sht->cnt.effects, EFF_FOCUS);
	}
	Sheet_force_slide( p_sht);
}
//static int	RLT_div_input(void *self, void *data, int len)
//{
//	RLT_trendHMI		*cthis = Get_RLT_trendHMI();
//	if(len > 2)
//		return ERR_PARAM_BAD;
//	cthis->str_div[0] = ((char *)data)[0];
//	cthis->str_div[1] = ((char *)data)[1];
//	cthis->p_div->cnt.len = len;
//	cthis->min_div = atoi(data);
//	
//	return RET_OK;
//}
static void	RT_trendHmi_Show( HMI *self )
{
//	RLT_trendHMI		*cthis = SUB_PTR(self, HMI, RLT_trendHMI);
	
	Curve 					*p_crv = CRV_Get_Sington();
	g_p_curHmi = self;
	
	
	
	Sheet_refresh(g_p_sht_bkpic);
//	self->dataVisual(self, NULL);
	self->show_focus( self, 0, 0);
	
	//刷新了背景，就要重新开始绘制
	if(self->arg[0] == 0)
	{
//		p_crv->crv_show_curve(HMI_CMP_ALL, CRV_SHOW_WHOLE);
		hst_mgr.rtv_reflush = 1;
	}
	else
	{
		hst_mgr.hst_flags &= ~ HST_FLAG_DONE;
		
		
	}
	
	Flush_LCD();
//	
}




static void	RT_trendHmi_HitHandle( HMI *self, char kcd)
{
	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	Curve						*p_crv = CRV_Get_Sington();
//	shtCmd		*p_cmd;
	sheet		*p_focus;
	Button	*p = BTN_Get_Sington();
	
	uint8_t		focusRow = self->p_fcuu->focus_row;
	uint8_t		focusCol = self->p_fcuu->focus_col;
	uint8_t		chgFouse = 0;
	uint8_t		chn = 0;
	
	uint8_t		new_mins = 0;
	
	
	switch(kcd)
	{

			case KEYCODE_UP:
					p_focus = Focus_Get_focus(self->p_fcuu);
					if(p_focus == NULL)
						break;
					
					if(p_focus->id == SHTID_RTL_MDIV)
					{
						if(self->arg[0] == 1)	//历史曲线的时标是 1- 8
							Str_Calculations(p_focus->cnt.data, 2, OP_MUX, 2, 1, 8);
						else
							Str_Calculations(p_focus->cnt.data, 2, OP_MUX, 2, 1, 16);
						p_focus->cnt.len = strlen(p_focus->cnt.data);
						chgFouse = 1;
					}	
					else if(p_focus->id == SHT_FIRST_TIME)
					{
						CRV_Set_first_time(self);
					}
					break;
			case KEYCODE_DOWN:
					p_focus = Focus_Get_focus(self->p_fcuu);
					if(p_focus == NULL)
						break;
					
					if(p_focus->id == SHTID_RTL_MDIV)
					{
						if(self->arg[0] == 1)	//历史曲线的时标是 1- 8
							Str_Calculations(p_focus->cnt.data, 2, OP_DIV, 2, 1, 8);
						else
							Str_Calculations(p_focus->cnt.data, 2, OP_DIV, 2, 1, 16);
						p_focus->cnt.len = strlen(p_focus->cnt.data);
						chgFouse = 1;
					}		
					else if(p_focus->id == SHT_FIRST_TIME)
					{
						CRV_Set_first_time(self);
					}					
					break;
			case KEYCODE_LEFT:
					if(self->flag & HMIFLAG_FOCUS_IN_BTN)
					{
						if(self->btn_backward(self) != RET_OK)
						{
							Focus_move_right(self->p_fcuu);
							chgFouse = 1;
						}
						
					}
					else if(Focus_move_left(self->p_fcuu) != RET_OK)
					{
						self->btn_forward(self);	//跳到第一个按钮
						chgFouse = 1;
					}
					else
					{
						chgFouse = 1;
					}					 
					break;
			case KEYCODE_RIGHT:
					if(self->flag & HMIFLAG_FOCUS_IN_BTN)
					{
						if(self->btn_forward(self) != RET_OK)
						{
							Focus_move_left(self->p_fcuu);	//移动到最左端
							chgFouse = 1;
						}
						
					}
					else if(Focus_move_right(self->p_fcuu) != RET_OK)
					{
						self->btn_forward(self);	//跳到第一个按钮
						chgFouse = 1;
					}
					else
					{
						chgFouse = 1;
					}					 
					break;

			case KEYCODE_ENTER:
					if(self->flag & HMIFLAG_FOCUS_IN_BTN)
					{
						p->hit();
					}
					
					//处于按钮区的话p_focus 肯定是NULL
					p_focus = Focus_Get_focus(self->p_fcuu);
					if(p_focus == NULL)
						goto exit;
					
					if(IS_CHECK(p_focus->id)) {
						chn = GET_CHN_FROM_ID(p_focus->id);
						
						memset(hst_mgr.arr_hst_num, 0, sizeof(hst_mgr.arr_hst_num));
						
						//反转选中显示
						if(cthis->chn_show_map & (1 << chn)) {
							cthis->chn_show_map &= ~(1 << chn);
							p_crv->crv_ctl(cthis->arr_crv_fd[chn], CRV_CTL_HIDE, 1);
							
							p_focus->area.n = 3;
							//清除数字显示
							sprintf(g_arr_p_chnData[chn]->cnt.data, "    ");
							g_arr_p_chnData[chn]->cnt.len = strlen(g_arr_p_chnData[chn]->cnt.data);
							g_arr_p_chnData[chn]->p_gp->vdraw(g_arr_p_chnData[chn]->p_gp,\
							&g_arr_p_chnData[chn]->cnt, &g_arr_p_chnData[chn]->area);
							
							if(self->arg[0] == 0)
							{
								p_crv->crv_show_curve(HMI_CMP_ALL, CRV_SHOW_WHOLE);
							}
							else
							{
								hst_mgr.hst_flags &= ~ HST_FLAG_DONE;
								
								
							}
						} else {
							p_focus->area.n = 1;
							cthis->chn_show_map |= 1 << chn;
							p_crv->crv_ctl(cthis->arr_crv_fd[chn], CRV_CTL_HIDE, 0);
							
							
							
							if(self->arg[0] == 0)
							{
								p_crv->crv_show_curve(HMI_CMP_ALL, CRV_SHOW_WHOLE);
							}
							else
							{
								hst_mgr.hst_flags &= ~ HST_FLAG_DONE;
								
								
							}
						}
						
						//目前所有的曲线都是统一显示的，因此显示记录要保持一致
//						memset(hst_mgr.arr_hst_num, 0 , sizeof(hst_mgr.arr_hst_num));

						p_focus->p_gp->vdraw(p_focus->p_gp, &p_focus->cnt, &p_focus->area);
						Flush_LCD();
						
					} 
					else if(p_focus->id == SHTID_RTL_MDIV) 
					{
						if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
							goto exit;
						new_mins = atoi(p_focus->cnt.data);
						
						memset(hst_mgr.arr_hst_num, 0, sizeof(hst_mgr.arr_hst_num));
						
						arr_mdiv_change[self->arg[0]](cthis, new_mins);
						Sem_post(&phn_sys.hmi_mgr.hmi_sem);
					} 
					else if(p_focus->id == SHT_FIRST_TIME)
					{
						
						//借用时标更新后，从新刷新曲线的机制来重新显示曲线
						if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
							goto exit;
						
						arr_mdiv_change[self->arg[0]](cthis, cthis->min_div);
						Sem_post(&phn_sys.hmi_mgr.hmi_sem);
					}
					else 
					{
						
						self->switchHMI(self, g_p_HMI_menu, 0);
					}					
					break;		
			case KEYCODE_ESC:
					
					break;	
			
	}	
	

	
	
	exit:
	if( chgFouse)
	{
		self->clear_focus(self, focusRow, focusCol);
		self->show_focus(self, 0, 0);
	}
	

	

	
//	cthis->flags &= ~2;
//	Set_flag_keyhandle(&self->flag, 0);
//	self->flag &= ~HMI_FLAG_DEAL_HIT;
	
}


static void RTV_midv_change(RLT_trendHMI *cthis, uint8_t new_mins)
{
	Curve						*p_crv = CRV_Get_Sington();
	uint8_t						s = 1;
	uint8_t						pix_s = 1;
	uint8_t						pix_gap[2];
	p_crv->crv_ctl(HMI_CMP_ALL, CRV_CTL_STEP_PIX, 4 / new_mins);
	
	
	/*
	时标 数据数量倍率(即采样频率) 像素间隔
	1 1 4
	2 1 2
	4 1 1
	8 2 1
	16 4 1
	*/
	
	//先计算倍数
	//然后再去除像素间隔的倍数
	pix_gap[0] = 4 / cthis->min_div;
	pix_gap[1] = 4 / new_mins;
	if(pix_gap[0] == 0)
		pix_gap[0] = 1;
	if(pix_gap[1] == 0)
		pix_gap[1] = 1;
	
	if(new_mins > cthis->min_div)
	{
		s = new_mins / cthis->min_div;
		pix_s = pix_gap[0] / pix_gap[1];
		s /= pix_s;
		
	}
	else
	{
		s = cthis->min_div / new_mins;
		pix_s = pix_gap[1] / pix_gap[0];
		s /= pix_s;
	}
	
	
	
	
//	if((new_mins + cthis->min_div) == 24)		//8 与 16之间的变化，倍数是2
//		s = 2;
//	else
//		s = new_mins / 4;
	if(new_mins > cthis->min_div)
		p_crv->crv_data_flex(HMI_CMP_ALL, FLEX_ZOOM_OUT, s, 60 * new_mins + 1);
	else
		p_crv->crv_data_flex(HMI_CMP_ALL, FLEX_ZOOM_IN, s, 60 * new_mins + 1);
	
	cthis->min_div = new_mins;
	p_crv->crv_show_curve(HMI_CMP_ALL, CRV_SHOW_WHOLE);
}
static void HST_midv_change(RLT_trendHMI *cthis, uint8_t new_mins)
{
	Curve						*p_crv = CRV_Get_Sington();
	p_crv->crv_ctl(HMI_CMP_ALL, CRV_CTL_STEP_PIX, 16 / new_mins);	
//	p_crv->crv_ctl(HMI_CMP_ALL, CRV_CTL_MAX_NUM, 240 / new_mins);	
	p_crv->crv_data_flex(HMI_CMP_ALL, FLEX_CLEAN, 0, HST_Num_rcds(new_mins));
	
	HST_Flex(new_mins, cthis->min_div);
	
	cthis->min_div = new_mins;
	
}
static uint16_t HST_Num_rcds(uint8_t	mul)
{
	return 15 * mul + 1;
}

static uint16_t HST_Time_step(uint8_t	mul)
{
	//一个屏幕240个曲线点
	if(mul <= 4)
		return 1;
	return mul / 4;
}


static void HST_Init(HMI *self)
{
	int i;
	
	for(i = 0; i < NUM_CHANNEL; i++)
	{
		hst_mgr.arr_hst_num[i] = 0;
	}
	
	hst_mgr.real_first_time_s = 0;		
	
	if(hst_mgr.init_att & HMI_ATT_KEEP)
		return ;
	hst_mgr.hst_flags = 0;
	hst_mgr.has_pgdn = 0;
	hst_mgr.set_start_sec = 0;
}

static void HST_Flex(uint8_t new_mdiv, uint8_t old_mdiv)
{
//	uint8_t		i = 0;
		
//	for(i = 0; i < NUM_CHANNEL; i++)
//	{
//		hst_mgr.arr_hst_num[i] = 0;
//	}
	hst_mgr.hst_flags &= ~HST_FLAG_DONE;

	
}

static void HST_Move(RLT_trendHMI *cthis, uint8_t direction)
{
//	uint8_t		i;
	uint16_t		num = HST_Num_rcds(cthis->min_div);
	
	
	
	
	if(direction == HST_NEXT_PG)
	{
//		for(i = 0; i < NUM_CHANNEL; i++)
//		{
//			hst_mgr.arr_hst_num[i] += num;
//			hst_mgr.has_pgdn = 1;
//			
//		}
		hst_mgr.real_first_time_s += cthis->min_div * 60;
	}
	else
	{
		//向前翻2页，因为每次执行了一次显示，会自动往后走一页的偏移。
		//因此如果不多翻一页就会导致每次往前翻页都不起作用
//		num = num * 2;	
//		//往前翻页的时候，要把当前的记录数量减小。
//		for(i = 0; i < NUM_CHANNEL; i++)
//		{
//			
//				if(hst_mgr.arr_hst_num[i] > num)
//				{
//					
//					hst_mgr.arr_hst_num[i] -= num;
//					
//				}
//				else
//				{
//					hst_mgr.arr_hst_num[i] = 0;
//				}
//		}
		memset(hst_mgr.arr_hst_num, 0, sizeof(hst_mgr.arr_hst_num));
		
		//允许向前翻页到设置的起始时间之前
		if(hst_mgr.real_first_time_s > cthis->min_div * 60)
			hst_mgr.real_first_time_s -= cthis->min_div * 60;
		else
			hst_mgr.real_first_time_s = 0;
		
	}
	
	hst_mgr.hst_flags &= ~HST_FLAG_DONE;
	
}

//历史曲线运行方法
static void HMI_CRV_HST_Run(HMI *self)
{
	RLT_trendHMI			*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	Curve 					*p_crv = CRV_Get_Sington();
	Storage					*stg = Get_storage();
	Model					*p_mdl ;
	Button					*p_btn = BTN_Get_Sington();
	data_in_fsh_t			d;
//	int						read_len = 0;
//	uint32_t				first_time = 0;
	uint32_t				time_s = 0;
//	uint32_t				pos = 0;
	struct	tm				first_tm;
	uint32_t				end_time;
	uint16_t				i, num_point, crv_max_points, time_step, need_clean = 0;
	uint8_t					crv_prc, pg_up,pg_down;
	uint8_t					last_prc[NUM_CHANNEL];
	char					str_buf[7];
	//读取一条就记录记录一条
	//直到屏幕上容纳不下了，或者记录读完了
	
	if(IS_HMI_HIDE(self->flag))
		return;
	if(hst_mgr.hst_flags & HST_FLAG_DONE)
		return;
	
	
	if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
		return;
	

	
	crv_max_points = HST_Num_rcds(cthis->min_div) - 1;
	
	//第一次显示的时候，把设置的起始时间作为实际的起始时间。
	//而一旦已经显示过了，实际起始时间是根据翻页情况变化的，就不要赋初值了
	
	if(hst_mgr.real_first_time_s == 0)
		hst_mgr.real_first_time_s = hst_mgr.set_start_sec;
	
	end_time = hst_mgr.real_first_time_s + cthis->min_div * 60;
	//只要没有到达最前的位置，就允许向上翻页
	hst_mgr.has_pgdn = 0;
	
	
	//根据当前的时标计算曲线点之间的时间间隔
	time_step = HST_Time_step(cthis->min_div);
	/*  安装曲线数据 */
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++)
	{
		
		if((cthis->chn_show_map & (1 << i)) == 0) {
			continue;
		}
		
		//如果起始时间为0则使用第一条记录的时间作为起始时间
		if(hst_mgr.real_first_time_s == 0)
		{
			STG_Read_data_by_time(i, 0, 0, &d);
			if(d.rcd_time_s == 0xffffffff)
				break;
			
			hst_mgr.real_first_time_s = d.rcd_time_s;
			end_time = hst_mgr.real_first_time_s + cthis->min_div * 60;
			
		}
	
		//从起始时间开始到结束时间这段范围内的数据并装入曲线
		sprintf(str_buf, "chn_%d", i);
		p_mdl = Create_model(str_buf);
		num_point = 0;
		last_prc[i] = 0;
		for(time_s = hst_mgr.real_first_time_s; time_s < end_time;time_s += time_step)
		{
			hst_mgr.arr_hst_num[i] = STG_Read_data_by_time(i, time_s, hst_mgr.arr_hst_num[i], &d);
			hst_mgr.arr_hst_num[i] ++;
			//如果某个时间点没有数据，则装入0
			if(d.rcd_time_s != 0xffffffff)
			{
				
				crv_prc = MdlChn_Cal_prc(p_mdl, d.rcd_val);
				last_prc[i] = crv_prc;
			}
			else
			{
				
				crv_prc = last_prc[i];
			}
			
			
			
			p_crv->add_point(cthis->arr_crv_fd[i], crv_prc);
			num_point ++;
			if(num_point > crv_max_points)
				break;
			
		}
		
		
	}
	
	//获取是否有更早数据，来决定是否有向上按钮
	pg_up = 0;
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++)
	{
		STG_Read_data_by_time(i, 0, 0, &d);
		if(d.rcd_time_s == 0xffffffff)
			continue;
		
		if(d.rcd_time_s < hst_mgr.real_first_time_s)
		{
			
			pg_up = 1;
			break;
		}
		
		
	}
	
	//获取是否有更晚数据，来决定是否有向下按钮
	pg_down = 0;
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++)
	{
		STG_Read_data_by_time(i, 0xffffffff, 0, &d);
		if(d.rcd_time_s == 0xffffffff)
			continue;
		
		if(d.rcd_time_s > hst_mgr.real_first_time_s)
		{
			
			pg_down = 1;
			break;
		}
		
		
	}	
//	for(i = 0; i < RLTHMI_NUM_CURVE; i++)
//	{
//		
//		
//		
//		if((cthis->chn_show_map & (1 << i)) == 0) {
//			continue;
//		}	
//		
////		count = 0;		//
//		sprintf(str_buf, "chn_%d", i);
//		p_mdl = Create_model(str_buf);
//		
//		//获取最早记录

//		if(hst_mgr.arr_hst_num[i]) //只要没有到达最前的位置，就允许向上翻页
//			hst_mgr.has_pgdn = 1;
//		
//		STG_Set_file_position(STG_CHN_DATA(i), STG_DRC_READ, hst_mgr.arr_hst_num[i] * sizeof(data_in_fsh_t));	
//		p_crv->reset(cthis->arr_crv_fd[i]);
//		while(1)
//		{
//			read_len = stg->rd_stored_data(stg, STG_CHN_DATA(i), &d, sizeof(d));
//			
//			if(read_len == 0)
//				break;
//			if(read_len != sizeof(d))
//				goto exit;	//可能文件正在被其他线程访问，直接退出，下一次再尝试
//			if(d.rcd_time_s == 0xffffffff)
//				break;
//			
//			hst_mgr.arr_hst_num[i] ++;		//读取了一条有效记录，就把计数器加1
//			
//			if(hst_mgr.real_first_time_s == 0)
//			{
//				hst_mgr.real_first_time_s = d.rcd_time_s;
//				end_time = hst_mgr.real_first_time_s + cthis->min_div * 60;
//			}
//			
//			if(d.rcd_time_s < hst_mgr.real_first_time_s)
//				continue;
//			
//			if(d.rcd_time_s > end_time)
//			{
//				more= 1;
//				break;
//			}
//						
//			crv_prc = MdlChn_Cal_prc(p_mdl, d.rcd_val);
//			p_crv->add_point(cthis->arr_crv_fd[i], crv_prc);

//		}
//		
//		
//	}
	
	
	Sec_2_tm(hst_mgr.real_first_time_s, &first_tm);
	sprintf(cthis->p_first_time->cnt.data, "%02d-%02d-%02d %02d:%02d:%02d", \
		first_tm.tm_year, first_tm.tm_mon, first_tm.tm_mday, \
		first_tm.tm_hour, first_tm.tm_min, first_tm.tm_sec);

	
	cthis->p_first_time->cnt.len = strlen(cthis->p_first_time->cnt.data);
	Sheet_force_slide(cthis->p_first_time);
	
	if(pg_down) //说明可能还有记录
	{
		
		p_btn->build_each_btn(2, BTN_TYPE_PGDN, RLT_btn_hdl, self);
	}
	else {		
		p_btn->build_each_btn(2, BTN_FLAG_CLEAN | BTN_TYPE_PGDN, NULL, NULL);
//		need_clean = 1;
	}
	
	
	if(pg_up) 
	{
		
		p_btn->build_each_btn(1, BTN_TYPE_PGUP, RLT_btn_hdl, self);
	}
	else {		
		p_btn->build_each_btn(1, BTN_FLAG_CLEAN | BTN_TYPE_PGUP, NULL, NULL);
//		need_clean = 1;
	}
	if((cthis->chn_show_map & ((1 << NUM_CHANNEL) - 1)) == 0)
		need_clean = 1;		//没有曲线需要显示的时候，把界面清除掉
	if(need_clean)
		self->show(self);
	p_btn->show_vaild_btn();
	
	hst_mgr.hst_flags |= HST_FLAG_DONE;
	p_crv->crv_show_curve(HMI_CMP_ALL, CRV_SHOW_WHOLE);
		
	
	//显示颜色标识
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++)
	{
		if((cthis->chn_show_map & (1 << i)) == 0) {
			continue;
		}
		sprintf(g_arr_p_chnData[i]->cnt.data, "CLR");
		g_arr_p_chnData[i]->cnt.len = strlen(g_arr_p_chnData[i]->cnt.data);
		g_arr_p_chnData[i]->p_gp->vdraw(g_arr_p_chnData[i]->p_gp, &g_arr_p_chnData[i]->cnt, &g_arr_p_chnData[i]->area);
		
	}
//	exit:
	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	
}
//实时曲线的运行方法
static void HMI_CRV_RTV_Run(HMI *self)
{
	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	Curve 					*p_crv = CRV_Get_Sington();
	Model						*p_mdl ;
	int							i;
	uint8_t			crv_prc;
	char					chn_name[7];

	
	cthis->count ++;
	if(cthis->count < cthis->min_div / 4)
		return;
	if(IS_HMI_HIDE(self->flag))
		return;
	cthis->count = 0;
	//刷新时间未到就直接退出
	
	if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
		return;
	
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++) {
		sprintf(chn_name, "chn_%d", i);
		p_mdl = Create_model(chn_name);
		p_mdl->getMdlData(p_mdl, AUX_PERCENTAGE, &crv_prc);
		
		p_crv->add_point(cthis->arr_crv_fd[i], crv_prc);
		
		if((cthis->chn_show_map & (1 << i)) == 0) {
			continue;
		}		
		sprintf(g_arr_p_chnData[i]->cnt.data, "%%%3d",crv_prc);
		g_arr_p_chnData[i]->cnt.len = strlen(g_arr_p_chnData[i]->cnt.data);
		g_arr_p_chnData[i]->p_gp->vdraw(g_arr_p_chnData[i]->p_gp, &g_arr_p_chnData[i]->cnt, &g_arr_p_chnData[i]->area);
		

	}
	
	if(hst_mgr.rtv_reflush == 0)
		p_crv->crv_show_curve(HMI_CMP_ALL, CRV_SHOW_LATEST);
	else
	{
		hst_mgr.rtv_reflush = 0;
		p_crv->crv_show_curve(HMI_CMP_ALL, CRV_SHOW_WHOLE);
		
	}
	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
//	Flush_LCD();
	
	
	
	
	return;
	
}

static void RLT_Init_curve(HMI *self)
{
	
	
	HMI_Ram_init();		//曲线需要使用的缓存
	HST_Init(self);

}

static void RLTHmi_Init_chnSht(void)
{
	Expr 		*p_exp ;
//	Model		*p_mdl = NULL;
	uint8_t			data_vy[6] = {65, 98, 128, 160, 195, 225};
	uint16_t			i = 0;
	p_exp = ExpCreate( "text");
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++) {
		p_exp->inptSht( p_exp, (void *)RT_hmi_code_data, g_arr_p_chnData[i]) ;
		
		
		
		
		g_arr_p_chnData[i]->cnt.colour = arr_clrs[i];
		g_arr_p_chnData[i]->cnt.data = prn_buf[i];
		

		g_arr_p_chnData[i]->cnt.colour = arr_clrs[i];
		g_arr_p_chnData[i]->area.y0 = data_vy[i];
	}
}


static int CRV_Win_cmd(void *p_rcv, int cmd,  void *arg)
{
	HMI								*self = (HMI *)p_rcv;
//	RLT_trendHMI			*cthis = SUB_PTR(self, HMI, RLT_trendHMI);
//	winHmi						*p_win;
	char							*p = (char *)hst_mgr.set_first_tm_buf;
	struct  tm				t = {0};
	short							i;
	uint8_t						err;
								
	
	switch(cmd) {
		
		
		case wincmd_commit:
			
			t.tm_year = Get_str_data(p, "/", 0, &err);
			if(err)
				goto err;
			t.tm_mon = Get_str_data(p, "/", 1, &err);
			if(err)
				goto err;
			t.tm_mday = Get_str_data(p, "/", 2, &err);
			if(err)
				goto err;
			if(t.tm_mday > g_moth_day[t.tm_mon])
				goto err;
		
			
			i = strcspn(p, " ");
			p += i;
			
			t.tm_hour = Get_str_data(p, ":", 0, &err);
			if(err)
				goto err;
			t.tm_min = Get_str_data(p, ":", 1, &err);
			if(err)
				goto err;
			t.tm_sec = Get_str_data(p, ":", 2, &err);
			if(err)
				goto err;
		
			
			hst_mgr.set_start_sec = Time_2_u32(&t);
			memset(hst_mgr.arr_hst_num, 0 , sizeof(hst_mgr.arr_hst_num));

			g_p_winHmi->arg[0] = WINTYPE_TIPS;
			g_p_winHmi->arg[1] = WINFLAG_RETURN;
			Win_content("修改成功");

			
			self->switchHMI(self, g_p_winHmi, HMI_ATT_NOT_RECORD);		
			break;
				
				
			err:
				
				g_p_winHmi->arg[0] = WINTYPE_ERROR;
				g_p_winHmi->arg[1] = WINFLAG_RETURN;
				Win_content("设置时间错误");

				//本界面已经在wincmd_commit之前的命令时已经被记录到历史列表里面了，所以这就不用再记录了
				self->switchHMI(self, g_p_winHmi, HMI_ATT_NOT_RECORD);

			
		
			break;
		
	}
	
	return 0;
	
	
}

static void CRV_Set_first_time(HMI *self)
{
	
	winHmi						*p_win;
	struct tm t;
	
	if(hst_mgr.set_start_sec == 0)
		hst_mgr.set_start_sec = SYS_time_sec();
	Sec_2_tm(hst_mgr.set_start_sec, &t);
	sprintf(hst_mgr.set_first_tm_buf, "%02d/%02d/%02d %02d:%02d:%02d", t.tm_year, t.tm_mon, t.tm_mday, \
				t.tm_hour, t.tm_min, t.tm_sec);
	Win_content(hst_mgr.set_first_tm_buf);
	g_p_winHmi->arg[0] = WINTYPE_TIME_SET;
	g_p_winHmi->arg[1] = 0;
	p_win = Get_winHmi();
	p_win->p_cmd_rcv = self;
	p_win->cmd_hdl = CRV_Win_cmd;
	self->switchHMI(self, g_p_winHmi, 0);
	
}







