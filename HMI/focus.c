//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "focus.h"
#include "sdhDef.h"

#ifdef NO_ASSERT
#include "basis/assert.h"
#else
#include "assert.h"
#endif
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
const Except_T FOCUS_Failed = { "[focus]" };
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

#define FCS_FLAG_IDLE		0
#define FCS_FLAG_USED		1

//当需要焦点跳出功能的时候，焦点跃出边界的时候，焦点会移动到界外
//否则焦点跃出边界的时候，进入移动方向的另外一端
#define FCS_NEED_JUMPOUT		1		
//#define FCS_FLAG_CHANGED	2
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static focus_manager_t fcsMgr;
static sheet *arr_p_focus_shts[MAX_FOCUS];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int  Focus_find(int rows, int columns);
//static sheet*  Focus_getSht(int rows, int columns);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
void Focus_init(void)
{
	int	i = 0;
	
	fcsMgr.sht_free = MAX_USER;
	for(i = 0; i< MAX_USER; i++) {
		
		fcsMgr.arr_fcu[i].flags = FCS_FLAG_IDLE;
	}
}

focus_user_t* Focus_alloc(int rows, int columns)
{
	focus_user_t	*p_fcu = NULL;
//	sheet			*p_sht = NULL;
	int	i = 0;
	
	
//	if(Focus_find(rows, columns) < 0)
//		return NULL;
	
	for(i = 0; i< MAX_USER; i++) {
		
		if(fcsMgr.arr_fcu[i].flags == FCS_FLAG_IDLE) {
			p_fcu = &fcsMgr.arr_fcu[i];
			p_fcu->rows = rows;
			p_fcu->columns = columns;
			p_fcu->first_idx = Focus_find(rows, columns);
			if(p_fcu->first_idx < 0)
				p_fcu = NULL;
			else 
				fcsMgr.arr_fcu[i].flags = FCS_FLAG_USED;
			break;
//			p_fcu->p_firstSht = Focus_getSht(rows, columns);
		}
	}
	
	if(p_fcu == NULL)
		Except_raise(&FOCUS_Failed, __FILE__, __LINE__);
	p_fcu->focus_col = 0;
	p_fcu->focus_row = 0;
	
	return p_fcu;
	
}

void Focus_free(focus_user_t *p_fcuu)
{
	short 	i = 0;
	short	num;
	
	if(p_fcuu == NULL)
		return;
	
	num = p_fcuu->rows * p_fcuu->columns;
	num += p_fcuu->first_idx;
	for(i = p_fcuu->first_idx; i < num; i ++) {		
		fcsMgr.sht_map[i >> 3] &= ~(1 << (i % 8));
	}
	
	p_fcuu->flags = FCS_FLAG_IDLE;
}

void Focus_Set_focus(focus_user_t *p_fcuu, int row, int col)
{
	if(row <= p_fcuu->rows) 
		p_fcuu->focus_row = row;
	
	if(col <= p_fcuu->columns) 
		p_fcuu->focus_col = col;
}
	

void Focus_Set_sht(focus_user_t *p_fcuu, int row, int col, sheet *p_sht)
{
	if(row > (p_fcuu->rows - 1)) 
		return;
	
	if(col > (p_fcuu->columns - 1)) 
		return;
	
	arr_p_focus_shts[p_fcuu->first_idx + row * p_fcuu->rows + col] = p_sht; 
}

int Focus_move_left(focus_user_t *p_fcuu)
{
	if(p_fcuu->focus_col == 0xff)
		p_fcuu->focus_col = p_fcuu->columns - 1;
	else if(p_fcuu->focus_col > 0)
		p_fcuu->focus_col --;
	else
	{
#if FCS_NEED_JUMPOUT == 0
		p_fcuu->focus_col = p_fcuu->columns - 1;
#else
		p_fcuu->focus_col = 0xff;
		return RET_FAILED;
#endif
	}
	
	return RET_OK;
	
}
int Focus_move_right(focus_user_t *p_fcuu)
{
	if(p_fcuu->focus_col == 0xff)	//从外部进入
		p_fcuu->focus_col = 0;
	else if(p_fcuu->focus_col < (p_fcuu->columns - 1))
		p_fcuu->focus_col ++;
	else
	{
#if FCS_NEED_JUMPOUT == 0
		p_fcuu->focus_col = 0;
#else
		p_fcuu->focus_col = 0xff;		//跳到外部去
		return RET_FAILED;
#endif
	}
	
	return RET_OK;
}
int Focus_move_up(focus_user_t *p_fcuu)
{

	if(p_fcuu->focus_row == 0xff)
		p_fcuu->focus_row = p_fcuu->rows - 1;
	else if(p_fcuu->focus_row > 0)
		p_fcuu->focus_row --;
	else
	{
#if FCS_NEED_JUMPOUT == 0
		p_fcuu->focus_row = p_fcuu->rows - 1;
#else
		p_fcuu->focus_row = 0xff;		//跳到外部去
		return RET_FAILED;
#endif		
		
		

	}
	return RET_OK;
}
int Focus_move_down(focus_user_t *p_fcuu)
{
	if(p_fcuu->focus_row== 0xff)
		p_fcuu->focus_row = 0;
	else if(p_fcuu->focus_row < (p_fcuu->rows - 1))
		p_fcuu->focus_row ++;
	else
	{
		
#if FCS_NEED_JUMPOUT == 0
		p_fcuu->focus_row = 0;
#else
		p_fcuu->focus_row = 0xff;		//跳到外部去
		return RET_FAILED;
#endif			

	}
	return RET_OK;
	
}
sheet* Focus_Get_focus(focus_user_t *p_fcuu)
{
	//这个if是为了能够让界面在选择区域超过边界时，不显示任何焦点
	if(p_fcuu->focus_row == 0xff || p_fcuu->focus_col == 0xff)
		return NULL;
	
	return arr_p_focus_shts[p_fcuu->first_idx + p_fcuu->focus_row * p_fcuu->rows + p_fcuu->focus_col];
}

sheet* Focus_Get_sht(focus_user_t *p_fcuu, int row, int col)
{
	if(p_fcuu == NULL)
		return NULL;
	
	if(row >= p_fcuu->rows || col >= p_fcuu->columns)
		return NULL;
	return arr_p_focus_shts[p_fcuu->first_idx + row * p_fcuu->rows + col];
}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static int  Focus_find(int rows, int columns)
{
	int		first_idx = -1;
	uint8_t 	i = 0;
	uint8_t 	available = 0;
	
	
	for(i = 0; i < MAX_FOCUS; i ++) {
		if( (fcsMgr.sht_map[i >> 3] & (1 << (i % 8))) == 0) {
			if(first_idx < 0)
				first_idx = i;
			available ++;
			if(available >= rows * columns)
				break;
		} else {
			available = 0;
			first_idx = -1;
		}
	}
	
	return first_idx;
	
	
}
//static sheet*  Focus_getSht(int rows, int columns)
//{
//	int 	idx = -1;
//	short 	i = 0;
//	short	num = rows * columns;
//	sheet	*p = NULL;
//	
//	idx = Focus_find(rows, columns);
//	if(idx < 0)
//		return NULL;
//	
//	p = fcsMgr.arr_fcu[idx];
//	num += idx;
//	for(i = idx; i < num; i ++) {
//		
//		fcsMgr.sht_map[i >> 3] |= 1 << (i % 8); 
//	}
//	
//	return p;
//	
//	
//}


