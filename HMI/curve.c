//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "curve.h"
#include "sdhDef.h"
#include "ExpFactory.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
//curve_ctl_t		g_curve[NUM_CHANNEL];
sheet  		*g_p_curve_bkg;
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define CURVE_HIDE		1
#define CURVE_SHOW		0
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static const char RT_hmi_curve_point[] = { "<line></>" };
static const char code_curve_bkPic[] =  {"<bpic vx0=0 vy0=50 m=1 >15</>" };
static	sheet  	*p_sht_curve;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static int Curve_end(curve_ctl_t *p_cctl);
static void Curve_remove_earlist(curve_ctl_t *p_cctl);
static int Curve_len(curve_ctl_t *p_cctl);
static void Curve_draw_left_to_right(curve_ctl_t *p_cctl);
static void Curve_draw_right_to_left(curve_ctl_t *p_cctl);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
void Curve_init(void)
{
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
//	int				i;
	

	
	
	
	p_shtctl = GetShtctl();
	p_exp = ExpCreate( "line");
	p_sht_curve = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)RT_hmi_curve_point, p_sht_curve) ;
	
	p_exp = ExpCreate( "pic");
	g_p_curve_bkg = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)code_curve_bkPic, g_p_curve_bkg) ;
}

void Curve_set(curve_ctl_t *p_cctl, int num, int clr, int start_x, int step)
{
	p_cctl->num_points  = num;
	p_cctl->colour = clr;
	p_cctl->start_x = start_x;
	if(step > 0) {
		p_cctl->direct = 0;
		p_cctl->step = step;
	} else {
		p_cctl->direct = 1;
		p_cctl->step = -step;
	}
}
void Curve_hide(curve_ctl_t *p_cctl)
{
	
	p_cctl->flags = CURVE_HIDE;
}
void Curve_visible(curve_ctl_t *p_cctl)
{
	
	p_cctl->flags = CURVE_SHOW;
}

void Curve_clean_bkg(curve_ctl_t *p_cctl)
{
	if(p_cctl->direct == 0)
		g_p_curve_bkg->p_gp->vdraw(g_p_curve_bkg->p_gp, &g_p_curve_bkg->cnt, &g_p_curve_bkg->area);
	else  {
		
		if(p_cctl->current_idx == (p_cctl->num_points + 1))
			g_p_curve_bkg->p_gp->vdraw(g_p_curve_bkg->p_gp, &g_p_curve_bkg->cnt, &g_p_curve_bkg->area);
		
		
	}
	
}

void Curve_clean(curve_ctl_t *p_cctl)
{
	p_cctl->start_idx = 0;
	p_cctl->current_idx = 0;
	p_cctl->colour = 0;
	p_cctl->num_points = 0;
	p_cctl->flags = CURVE_SHOW;
}
//调用者负责Y坐标的调整
void Curve_add_point(curve_ctl_t *p_cctl, int val)
{
	
	if(p_cctl->direct == 0) {
		//如果曲线点数已经达到了num_points
		if(Curve_end(p_cctl)) {
			//要移除掉最早的那个曲线点
			Curve_remove_earlist(p_cctl);
		}
		
		p_cctl->points[p_cctl->current_idx] = val;
		p_cctl->current_idx += 1;
		p_cctl->current_idx %= p_cctl->num_points;
	} else {
		
		if(p_cctl->current_idx == (p_cctl->num_points + 1)) {
			p_cctl->current_idx = 0;
		}
		
		p_cctl->points[p_cctl->current_idx] = val;
		p_cctl->current_idx ++;
		
	}
	
}

void Curve_draw(curve_ctl_t *p_cctl)
{
//	short 	i = 0;
	short	points = 0;
	
	if(p_cctl->flags == CURVE_HIDE)
		return;
	
	p_sht_curve->cnt.colour = p_cctl->colour;
	points = Curve_len(p_cctl);
	
	//如果只有一个点就画一个点
	if(points == 1) {
		p_sht_curve->area.x0 = p_cctl->start_x;
		p_sht_curve->area.x1 = p_sht_curve->area.x0;
		p_sht_curve->area.y0 = p_cctl->points[p_cctl->start_idx];
		p_sht_curve->area.y1 = p_sht_curve->area.y0;
		p_sht_curve->p_gp->vdraw(p_sht_curve->p_gp, &p_sht_curve->cnt, &p_sht_curve->area);
		
		return;
	}
		
	
	//如果有多个点，就要把这些点用直线连起来
	if(p_cctl->direct == 0) {
		Curve_draw_left_to_right(p_cctl);
	} else {
		Curve_draw_right_to_left(p_cctl);
	}
	
	
	
}

//只绘制最新的那条曲线
//void Curve_draw_curent(curve_ctl_t *p_cctl)
//{
//	short	points = 0;
//	int		i_tmp;
//	p_sht_curve->cnt.colour = p_cctl->colour;
//	points = Curve_len(p_cctl);
//	
//	//如果只有一个点就画一个点
//	if(points == 1) {
//		p_sht_curve->area.x0 = p_cctl->start_x;
//		p_sht_curve->area.x1 = p_sht_curve->area.x0;
//		p_sht_curve->area.y0 = p_cctl->points[p_cctl->start_idx];
//		p_sht_curve->area.y1 = p_sht_curve->area.y0;
//		p_sht_curve->p_gp->vdraw(p_sht_curve->p_gp, &p_sht_curve->cnt, &p_sht_curve->area);
//		
//		return;
//	}
//	
//	p_sht_curve->area.x0 = p_cctl->start_x;
//	i_tmp = p_sht_curve->area.x0;
//	i_tmp += p_cctl->step;
//	p_sht_curve->area.x1 = i_tmp;
//	
//	p_sht_curve->area.y0 = p_cctl->points[p_cctl->current_idx];
//	p_sht_curve->area.y1 = p_cctl->points[p_cctl->current_idx - 1];
//	p_sht_curve->p_gp->vdraw(p_sht_curve->p_gp, &p_sht_curve->cnt, &p_sht_curve->area);
//}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static int Curve_len(curve_ctl_t *p_cctl)
{
	int len = 0;
	if(p_cctl->direct == 0) {
		len = p_cctl->current_idx + p_cctl->num_points - p_cctl->start_idx;
		len %= p_cctl->num_points;
	} else {
		len = p_cctl->current_idx;
		
	}
	return len;
}

static int Curve_end(curve_ctl_t *p_cctl)
{
	if(Curve_len(p_cctl) == (p_cctl->num_points - 1))
		return 1;
	return 0;
}

static void Curve_remove_earlist(curve_ctl_t *p_cctl)
{
	p_cctl->start_idx ++;
	p_cctl->start_idx %= p_cctl->num_points;
}
static int RLE(curve_ctl_t *p_cctl, int cur_idx)
{
	int 	len = 0;
	short 	i = 0;
	short	points = 0;
	short	idx = 0;
	
	points = Curve_len(p_cctl);
	
	//对cur_idx之后所有的点进行计算
	for(i = 1; i < (points); i+= 1) {
		
		idx =cur_idx + i;
		idx %= p_cctl->num_points;
		
		if(abs(p_cctl->points[cur_idx] - p_cctl->points[idx]) < CURVE_BEEK)
			len ++;
		else
			break;
		
		
		
		
	}
	if(len == 0)
		len = 1;
	return len;
}
static void Curve_draw_left_to_right(curve_ctl_t *p_cctl)
{
	short 	i = 0;
	short	points = 0;
	short	idx = 0;
	short	len = 0;
	
	points = Curve_len(p_cctl);
	
	for(i = 0; i < (points - 1); i+= len) {
		//计算x0,y0
		p_sht_curve->area.x0 = p_cctl->start_x - ( points - 1 - i) * p_cctl->step;
		idx = p_cctl->start_idx + i;
		idx %= p_cctl->num_points;
		p_sht_curve->area.y0 = p_cctl->points[idx];
		
		//计算距离x1,y1的跳数
//		idx ++;
//		idx %= p_cctl->num_points;
		len = RLE(p_cctl, idx );
		//计算x1, y1
		p_sht_curve->area.x1 = p_sht_curve->area.x0 + p_cctl->step * (len + 0);
		idx += len;
		idx %= p_cctl->num_points;
		p_sht_curve->area.y1 = p_cctl->points[idx];
		
		if(p_sht_curve->area.x1 > p_cctl->start_x)
			break;
		
//		p_sht_curve->area.x0 = p_cctl->start_x - ( points - 1 - i) * p_cctl->step;
//		
//		//计算当前位置上之后有几个值是在平滑度之内的
//		idx = p_cctl->start_idx + i;
//		idx %= p_cctl->num_points;
//		len = RLE(p_cctl, idx + 1);
//		
//		//显示到末尾
////		len --;
////		if(len <= 0)
////			len = 1;
//		//计算第二个x坐标
//		p_sht_curve->area.x1 = p_sht_curve->area.x0 + p_cctl->step * (len + 0);
//		//获得两个y坐标
//		p_sht_curve->area.y0 = p_cctl->points[idx];
//		idx += len - 1;
//		idx %= p_cctl->num_points;
//		p_sht_curve->area.y1 = p_cctl->points[idx];
//		if( p_sht_curve->area.y0 < 50 || p_sht_curve->area.y1 < 50)
//			break;
//		if( (p_sht_curve->area.x0 < 0)
//			break;
		
		p_sht_curve->p_gp->vdraw(p_sht_curve->p_gp, &p_sht_curve->cnt, &p_sht_curve->area);
		
	}
}


//点数必须大于1点时才能调用，否则会出错
//本函数不检查点数是否大于1
static void Curve_draw_right_to_left(curve_ctl_t *p_cctl)
{
	p_sht_curve->cnt.colour = p_cctl->colour;
	
	p_sht_curve->area.x0 = p_cctl->start_x + p_cctl->step * ( p_cctl->current_idx - 2);
	p_sht_curve->area.x1 = p_sht_curve->area.x0 + p_cctl->step;
	
	p_sht_curve->area.y0 = p_cctl->points[p_cctl->current_idx - 2];
	p_sht_curve->area.y1 = p_cctl->points[p_cctl->current_idx - 1];
	p_sht_curve->p_gp->vdraw(p_sht_curve->p_gp, &p_sht_curve->cnt, &p_sht_curve->area);	
}


