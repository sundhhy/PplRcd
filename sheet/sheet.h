#ifndef _INC_sheet_H_
#define _INC_sheet_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
#include "glyph.h"
#include "ModelFactory.h"

//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define MAX_SHEETS 128
#define MAX_SUBSHEETS 2
 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
struct SHTCTL;
struct SHEET;


typedef struct {
	uint8_t		numSubRow;
	uint8_t		numSubCol;
	uint8_t		subColGrap;
	uint8_t		subRowGrap;
}subShtAtt_t;

typedef struct {
	uint16_t		vxsize;
	uint16_t		vysize;
	
}video_t;

//INTERFACE( shtCmd)
//{
//	
//	void (*shtExcute)( shtCmd *self, struct SHEET *p_sht, void *arg);
//};



//CLASS( shtDefCmd)
//{
//	IMPLEMENTS( shtCmd);
//};

/* sheet.c   */
typedef struct SHEET {	
	subShtAtt_t		subAtt;
	struct SHEET	**pp_sub;
	
	//图像的尺寸
    uint16_t 	bxsize;
	uint16_t 	bysize;

	
	//在整个图层系统中的高度,要连续分配，否则不能正确处理
	short		height;
	uint8_t		flags;	
	uint8_t		col_inv;
	
	uint8_t		id;
//	uint8_t		e_heifht;		//在调用Sheet_slide 时将其与height 相加作为是否显示的依据
	uint8_t		none[3];
	
	dspContent_t	cnt;
	vArea_t			area;
	Glyph			*p_gp;
	
	//键盘相关
//	shtCmd		*p_enterCmd;
//	sht_commit	sht_cnt_commit;
//	int	(*input)(void *self, void *data, int len);
	
//    struct SHTCTL *p_shtctl;
}sheet;

typedef struct SHTCTL {
	video_t		v;
	int16_t		top;
    struct SHEET *arr_p_sheets[ MAX_SHEETS];
    struct SHEET  arr_sheets[ MAX_SHEETS];
}shtctl;


//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

shtctl *GetShtctl(void);

struct SHTCTL* Shtctl_init(   uint16_t vxsize, uint16_t vysize);
struct SHEET *Sheet_alloc( struct SHTCTL *p_ctl);
void Sheet_setbuf( struct SHEET *p_sht, uint8_t *buf, int bxsize, int bysize, int col_inv);
void Sheet_updown( struct SHEET *p_sht, int height);
void Sheet_append( struct SHEET *p_sht);
//int ShtUpdate( void *p_sht, void *p);
// void sheet_refresh( struct SHTCTL *p_ctl);
void Sheet_refresh( struct SHEET *p_sht);
void Sheet_slide(  struct SHEET *p_sht);
void Sheet_force_slide( struct SHEET *p_sht);
void Sheet_free( struct SHEET *p_sht);
int Sheet_is_hide(sheet *p_sht);
//shtDefCmd *Get_shtDefCmd(void);
#endif
