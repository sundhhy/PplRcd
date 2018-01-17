#ifndef _INC_focus_H__
#define _INC_focus_H__
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
#include "sheet.h"

//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define	MAX_FOCUS		24		
#define	MAX_USER		2
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

typedef struct {
	uint8_t		rows;
	uint8_t		columns;
	uint8_t		focus_row;
	uint8_t		focus_col;
	short		first_idx;		//

	uint8_t		flags;


	uint8_t		none;
//	sheet		*p_firstSht;
}focus_user_t;

typedef struct {
	uint8_t			sht_free;
	uint8_t			sht_map[3];		//长度应与MAX_FOCUS/8 现匹配
	focus_user_t	arr_fcu[MAX_USER];
}focus_manager_t;


//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
void Focus_init(void);
focus_user_t* Focus_alloc(int rows, int columns);
void Focus_free(focus_user_t *p_fcuu);
void Focus_Set_focus(focus_user_t *p_fcuu, int row, int col);
void Focus_Set_sht(focus_user_t *p_fcuu, int row, int col, sheet *p_sht);
int Focus_move_left(focus_user_t *p_fcuu);
int Focus_move_right(focus_user_t *p_fcuu);
int Focus_move_up(focus_user_t *p_fcuu);
int Focus_move_down(focus_user_t *p_fcuu);
sheet* Focus_Get_focus(focus_user_t *p_fcuu);
sheet* Focus_Get_sht(focus_user_t *p_fcuu, int row, int col);
#endif
