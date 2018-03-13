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

#define DEFAULT_MIN_DIV		'1'

#define CHNSHOW_ROW_SPACE		32


static const char RT_hmi_code_chninfo[] =  {"<cpic vx0=260 vy0=30 vx1=320 vy1=240>23</>" };


static const char RT_hmi_code_div[] = { "<text vx0=8 vy0=36 f=16 m=0 clr=red> </>" };

static const char RT_hmi_code_data[] = { "<text f=16 vx0=285 m=0 aux=3>100</>" };
static const char RLT_hmi_code_chnshow[] ={ "<icon vx0=265 vy0=62 xn=5 yn=1 n=0>19</>" };

//static const char RT_hmi_code_div[] = { "<text vx0=8 vy0=36 f=16 m=0 clr=red> </>" };


//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef void (*midv_change)(RLT_trendHMI *cthis, uint8_t new_mins);


struct {
	uint32_t			arr_hst_num[NUM_CHANNEL];
	uint8_t				hst_flags;
	uint8_t				has_pgdn;		//执行过向下翻页
	uint8_t				rtv_reflush;	//为1时，实时曲线重绘。用于页面定期刷新
	uint8_t				none;
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
static void RT_trendHmi_InitSheet( HMI *self );
static void RT_trendHmi_HideSheet( HMI *self );
static void	RT_trendHmi_Show( HMI *self);


static void	RT_trendHmi_HitHandle( HMI *self, char *s);

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
static void RLT_Init_curve(RLT_trendHMI *self);
static void HST_Init(void);
static uint16_t HST_Num_rcds(uint8_t	mul);
static void HST_Flex(uint8_t new_mdiv, uint8_t old_mdiv);
static void HST_Move(RLT_trendHMI *cthis, uint8_t direction);


//数据
static void RLTHmi_Init_chnSht(void);
static int RLTHmi_Data_update(void *p_data, void *p_mdl);

//键盘
//static int	RLT_div_input(void *self, void *data, int len);


static void HMI_CRV_Build_cmp(HMI *self);
static void RLT_btn_hdl(void *arg, uint8_t btn_id);
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


//FUNCTION_SETTING(Observer.update, RLT_trendHmi_MdlUpdata);		

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
	
	
	
	cthis->str_div[0] = DEFAULT_MIN_DIV;

	
	

//	Bulid_rtCurveSheet(cthis);
	
	
	//初始化通道勾选图形
	p_exp = ExpCreate( "pic");
	cthis->chn_show_map = 0xff;
	for(i = 0; i < RLTHMI_NUM_CURVE; i++) {
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
	short							i;
	short				num = 0;
	
	p->build_each_btn(0, BTN_TYPE_MENU, Main_btn_hdl, self);
//	p->build_each_btn(0, BTN_TYPE_PGUP, RLT_btn_hdl, self);
//	p->build_each_btn(1, BTN_TYPE_PGDN, RLT_btn_hdl, self);
//	p->build_each_btn(2, BTN_TYPE_LOOP, RLT_btn_hdl, self);
	
	for(i = 0; i < NUM_CHANNEL; i++)
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
		crv.crv_y0 = 50 + i * 10;
		crv.crv_x1 = 240;
		crv.crv_y1 = 150 + i * 10;		//210
		
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



static void RT_trendHmi_InitSheet( HMI *self )
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
	cthis->min_div = atoi(cthis->str_div);
	cthis->p_div->cnt.data = cthis->str_div;
	cthis->p_div->cnt.len = strlen(cthis->p_div->cnt.data);
//	cthis->p_div ->p_enterCmd = &g_keyHmi->shtCmd;
//	cthis->p_div->input =  RLT_div_input;
	cthis->p_div->id = SHTID_RTL_MDIV;
	
	
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
//	Sheet_updown(g_p_ico_memu, h++);
	Sheet_updown(cthis->p_div, h++);
//	Sheet_updown( cthis->p_clean_chnifo, h++);
	for(i = 0; i < RLTHMI_NUM_CURVE; i++) {
//		Sheet_updown(g_arr_p_chnData[i], h++);
		Sheet_updown(g_arr_p_check[i], h++);
	}
	
	RLTHmi_Init_chnSht();
	RLT_Init_curve(cthis);
	self->init_focus(self);
}

static void RT_trendHmi_HideSheet( HMI *self )
{
	RLT_trendHMI			*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	
	int i;
	
	
	for( i = RLTHMI_NUM_CURVE - 1; i >= 0; i--) {
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
	Sheet_free(cthis->p_div);
	Focus_free(self->p_fcuu);
}	



static void	RLT_init_focus(HMI *self)
{
	int	col = 0; 
	RLT_trendHMI *cthis = SUB_PTR(self, HMI, RLT_trendHMI);
	
	self->p_fcuu = Focus_alloc(1, RLTHMI_NUM_CURVE + 1);
	
	Focus_Set_focus(self->p_fcuu, 0, 0);
	
	Focus_Set_sht(self->p_fcuu, 0, 0, cthis->p_div);
	for(col = 0; col < RLTHMI_NUM_CURVE; col ++) {
		Focus_Set_sht(self->p_fcuu, 0, col + 1, g_arr_p_check[col]);
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
	Sheet_slide(p_sht);
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
	Sheet_slide( p_sht);
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

static void	RT_trendHmi_HitHandle( HMI *self, char *s)
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

	
//	cthis->flags |= 2;
//	Set_flag_keyhandle(&self->flag, 1);
//	self->flag |= HMI_FLAG_DEAL_HIT;
	if(!strcmp(s, HMIKEY_UP))
	{
		p_focus = Focus_Get_focus(self->p_fcuu);
		if(p_focus != NULL && p_focus->id == SHTID_RTL_MDIV)
		{
			Str_Calculations(p_focus->cnt.data, 2, OP_MUX, 2, 1, 16);
//			if(cthis->min_div < 16)
//				cthis->min_div *= 2;
//			else
//				cthis->min_div = 1;
//			sprintf(p_focus->cnt.data, "%2d", cthis->min_div);
			p_focus->cnt.len = strlen(p_focus->cnt.data);
			chgFouse = 1;
		}
	}
	else if( !strcmp( s, HMIKEY_DOWN) )
	{
		p_focus = Focus_Get_focus(self->p_fcuu);
		if(p_focus != NULL && p_focus->id == SHTID_RTL_MDIV)
		{
			
			Str_Calculations(p_focus->cnt.data, 2, OP_DIV, 2, 1, 16);
//			if(cthis->min_div > 1)
//				cthis->min_div /= 2;
//			else
//				cthis->min_div = 16;
//			sprintf(p_focus->cnt.data, "%2d", cthis->min_div);
			p_focus->cnt.len = strlen(p_focus->cnt.data);
			chgFouse = 1;
		}
	}
	else if( !strcmp( s, HMIKEY_LEFT))
	{
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
	}
	else if( !strcmp( s, HMIKEY_RIGHT))
	{
		
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
	}

	if( !strcmp( s, HMIKEY_ENTER))
	{
		if(self->flag & HMIFLAG_FOCUS_IN_BTN)
		{
			p->hit();
//			self->btn_hit(self);
		}
		
		//处于按钮区的话p_focus 肯定是NULL
		p_focus = Focus_Get_focus(self->p_fcuu);
		if(p_focus == NULL)
			goto exit;
		
		if(IS_CHECK(p_focus->id)) {
			chn = GET_CHN_FROM_ID(p_focus->id);
			
			if(cthis->chn_show_map & (1 << chn)) {
				cthis->chn_show_map &= ~(1 << chn);
				p_crv->crv_ctl(cthis->arr_crv_fd[chn], CRV_CTL_HIDE, 1);
				
				p_focus->area.n = 3;
				//清除数字显示
				sprintf(g_arr_p_chnData[chn]->cnt.data, "    ");
//				g_arr_p_chnData[chn]->cnt.data = "   ";
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
			p_focus->p_gp->vdraw(p_focus->p_gp, &p_focus->cnt, &p_focus->area);
			Flush_LCD();
			
		} else if(p_focus->id == SHTID_RTL_MDIV) {
			if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
				goto exit;
			new_mins = atoi(p_focus->cnt.data);
			
			arr_mdiv_change[self->arg[0]](cthis, new_mins);
//			self->switchHMI(self, self);
//			HMI_Ram_init();
			
			
			
			Sem_post(&phn_sys.hmi_mgr.hmi_sem);
//			if(p_focus) {
//				p_cmd = p_focus->p_enterCmd;
//				if(p_cmd)
//					p_cmd->shtExcute( p_cmd, p_focus, self);
//			} else
//				self->switchHMI(self, g_p_HMI_menu);
		} else {
			
			self->switchHMI(self, g_p_HMI_menu);
		}
	}
	
	exit:
	if( chgFouse)
	{
		self->clear_focus(self, focusRow, focusCol);
		self->show_focus(self, 0, 0);
	}
	

	
	if( !strcmp( s, HMIKEY_ESC))
	{
		self->switchBack(self);
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
static void HST_Init(void)
{
	int i;
	
	for(i = 0; i < NUM_CHANNEL; i++)
	{
		hst_mgr.arr_hst_num[i] = 0;
	}
	hst_mgr.hst_flags = 0;
	hst_mgr.has_pgdn = 0;
}

static void HST_Flex(uint8_t new_mdiv, uint8_t old_mdiv)
{
	uint8_t		i = 0;
		
	for(i = 0; i < NUM_CHANNEL; i++)
	{
		hst_mgr.arr_hst_num[i] = 0;
	}
	hst_mgr.hst_flags &= ~HST_FLAG_DONE;

	
}

static void HST_Move(RLT_trendHMI *cthis, uint8_t direction)
{
	uint8_t		i;
	uint16_t		num = HST_Num_rcds(cthis->min_div);
	
	
	
	
	if(direction == HST_NEXT_PG)
	{
		for(i = 0; i < NUM_CHANNEL; i++)
		{
			hst_mgr.arr_hst_num[i] += num;
			Set_bit(&hst_mgr.has_pgdn, i);
			
		}
	}
	else
	{
		for(i = 0; i < NUM_CHANNEL; i++)
		{
			
				if(hst_mgr.arr_hst_num[i] > num)
				{
					
					hst_mgr.arr_hst_num[i] -= num;
					
				}
				else
				{
					hst_mgr.arr_hst_num[i] = 0;
					Clear_bit(&hst_mgr.has_pgdn, i);
				}
		}
		
	}
	
	hst_mgr.hst_flags &= ~HST_FLAG_DONE;
	
}

//历史曲线运行方法
static void HMI_CRV_HST_Run(HMI *self)
{
	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	Curve 					*p_crv = CRV_Get_Sington();
	Storage					*stg = Get_storage();
	Model						*p_mdl ;
	Button					*p_btn = BTN_Get_Sington();
	data_in_fsh_t			d;
//	crv_val_t				cval;
	uint16_t				i, count, end, need_clean = 0;
	int						read_len = 0;
	uint8_t					crv_prc;
	//读取一条就记录记录一条
	//知道屏幕上容纳不下了，或者记录读完了
	
	if(IS_HMI_HIDE(self->flag))
		return;
	if(hst_mgr.hst_flags & HST_FLAG_DONE)
		return;
	
	
	if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
		return;
	

	
	end = HST_Num_rcds(cthis->min_div) - 1;
	
	
	
	//显示曲线
	
	for(i = 0; i < RLTHMI_NUM_CURVE; i++)
	{
		
		
		
		if((cthis->chn_show_map & (1 << i)) == 0) {
			continue;
		}	
		
		count = 0;
		
		p_mdl = g_arr_p_chnData[i]->p_mdl;
//		p_mdl->getMdlData(p_mdl, chnaux_upper_limit, &cval.up_limit);
//		p_mdl->getMdlData(p_mdl, chnaux_lower_limit, &cval.lower_limit);
//		p_mdl->getMdlData(p_mdl, AUX_PERCENTAGE, &crv_prc);
		
		STG_Set_file_position(STG_CHN_DATA(i), STG_DRC_READ, hst_mgr.arr_hst_num[i] * sizeof(data_in_fsh_t));	
		p_crv->reset(cthis->arr_crv_fd[i]);
		while(1)
		{
			read_len = stg->rd_stored_data(stg, STG_CHN_DATA(i), &d, sizeof(d));
			
			if(read_len == 0)
				break;
			if(read_len != sizeof(d))
				goto exit;	//可能文件正在被其他线程访问，直接退出，下一次再尝试
			if(d.rcd_time_s == 0xffffffff)
				continue;
			
//			cval.val = d.rcd_val;
//			p_mdl->getMdlData(p_mdl, AUX_PERCENTAGE, &crv_prc);
			crv_prc = MdlChn_Cal_prc(p_mdl, d.rcd_val);
			p_crv->add_point(cthis->arr_crv_fd[i], crv_prc);
			count ++;
			if(count > end)
				break;
		}
		
		
	}
	
	if(count >= end) //说明可能还有记录
	{
		
		p_btn->build_each_btn(2, BTN_TYPE_PGDN, RLT_btn_hdl, self);
	}
	else {		
		p_btn->build_each_btn(2, BTN_FLAG_CLEAN | BTN_TYPE_PGDN, NULL, NULL);
//		need_clean = 1;
	}
	
	if(hst_mgr.has_pgdn) //说明可能还有记录
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
	for(i = 0; i < RLTHMI_NUM_CURVE; i++)
	{
		if((cthis->chn_show_map & (1 << i)) == 0) {
			continue;
		}
		sprintf(g_arr_p_chnData[i]->cnt.data, "CLR");
		g_arr_p_chnData[i]->cnt.len = strlen(g_arr_p_chnData[i]->cnt.data);
		g_arr_p_chnData[i]->p_gp->vdraw(g_arr_p_chnData[i]->p_gp, &g_arr_p_chnData[i]->cnt, &g_arr_p_chnData[i]->area);
		
	}
	exit:
	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	
}
//实时曲线的运行方法
static void HMI_CRV_RTV_Run(HMI *self)
{
	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	Curve 					*p_crv = CRV_Get_Sington();
//	HMI				*p_hmi = SUPER_PTR(cthis, HMI);
	Model						*p_mdl ;
//	crv_val_t				cval;
	int							i;
	uint8_t			crv_prc;
//	int				range = 100;
//	
//	uint8_t			vy0 = 208;
//	uint8_t			height = 150;
//	uint8_t			i = 0;
//	int				y = 0;
	
	cthis->count ++;
	if(cthis->count < cthis->min_div / 4)
		return;
	if(IS_HMI_HIDE(self->flag))
		return;
	cthis->count = 0;
	//刷新时间未到就直接退出
	
	if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
		return;
	
	for(i = 0; i < RLTHMI_NUM_CURVE; i++) {
//		if(self->flag & HMI_FLAG_DEAL_HIT)
//			break;


		
		p_mdl = g_arr_p_chnData[i]->p_mdl;
		
//		p_mdl->getMdlData(p_mdl, chnaux_upper_limit, &cval.up_limit);
//		p_mdl->getMdlData(p_mdl, chnaux_lower_limit, &cval.lower_limit);
//		if(p_mdl->getMdlData(p_mdl, AUX_DATA, &cval.val) != RET_OK)
//		{
//			
//			cval.val = cval.lower_limit;
//		}

		p_mdl->getMdlData(p_mdl, AUX_PERCENTAGE, &crv_prc);
		
		p_crv->add_point(cthis->arr_crv_fd[i], crv_prc);
		
		if((cthis->chn_show_map & (1 << i)) == 0) {
			continue;
		}		
		sprintf(g_arr_p_chnData[i]->cnt.data, "%%%3d",crv_prc);
		g_arr_p_chnData[i]->cnt.len = strlen(g_arr_p_chnData[i]->cnt.data);
		g_arr_p_chnData[i]->p_gp->vdraw(g_arr_p_chnData[i]->p_gp, &g_arr_p_chnData[i]->cnt, &g_arr_p_chnData[i]->area);
		

	}
	
//	if((self->flag & HMI_FLAG_DEAL_HIT) == 0)
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

static void RLT_Init_curve(RLT_trendHMI *self)
{
	
	
	HMI_Ram_init();		//曲线需要使用的缓存
	
	HST_Init();
//	curve_ctl_t *p_cctl;
//	int i = 0;
//	
//	for(i = 0; i < RLTHMI_NUM_CURVE; i++) {
//		p_cctl = &g_curve[i];
//		arr_p_crv[i] = p_cctl;
//		
//		Curve_clean(p_cctl);
//		self->count = 0;
////		Curve_set(p_cctl, 240/self->min_div, arr_clrs[i], 239, self->min_div);
////		if(self->min_div == 1)
//			Curve_set(p_cctl, CURVE_POINT - 1, arr_clrs[i], 0, -1);
////		else
////			Curve_set(p_cctl, CURVE_POINT/self->min_div, arr_clrs[i], 0, -self->min_div);

//	}
	
	
	
}

static void RLTHmi_Init_chnSht(void)
{
	Expr 		*p_exp ;
//	Model		*p_mdl = NULL;
	uint8_t			data_vy[6] = {65, 98, 128, 160, 195, 225};
	uint16_t			i = 0;
	p_exp = ExpCreate( "text");
	for(i = 0; i < NUM_CHANNEL; i++) {
		p_exp->inptSht( p_exp, (void *)RT_hmi_code_data, g_arr_p_chnData[i]) ;
		
		
		
		
		g_arr_p_chnData[i]->cnt.colour = arr_clrs[i];
		g_arr_p_chnData[i]->cnt.data = prn_buf[i];
		
		g_arr_p_chnData[i]->update = RLTHmi_Data_update;

		g_arr_p_chnData[i]->cnt.colour = arr_clrs[i];
		g_arr_p_chnData[i]->area.y0 = data_vy[i];
	}
}

static int RLTHmi_Data_update(void *p_data, void *p_mdl)
{
	
	
	return 0;
	
	
	
}





