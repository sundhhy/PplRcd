#ifndef _INC_HMI_H_
#define _INC_HMI_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
#include <string.h>

#include "lw_oopc.h"

#include "sdhDef.h"
#ifdef NO_ASSERT
#include "basis/assert.h"
#else
#include "assert.h"
#endif

#include "sheet.h"


//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define HMIKEY_UP		"up"
#define HMIKEY_DOWN		"down"
#define HMIKEY_LEFT		"left"
#define HMIKEY_RIGHT		"right"
#define HMIKEY_ENTER		"enter"
#define HMIKEY_ESC		"esc"

 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef struct {
	uint8_t		rowGrap;			//行间距
	uint8_t		colGrap;			//列间距
	uint8_t		bkc;
	uint8_t		numRow;
	uint8_t		numCol;
	uint8_t		none[3];
}hmiAtt_t;


ABS_CLASS( HMI)
{
	HMI*			prev;
	HMI*			next;
	
	//显示静态画面
	int		( *init)( HMI *self, void *arg);
	void		(*show)( HMI *self);
	void		(*hide)( HMI *self);
	void		(*initSheet)( HMI *self);
	void		(*switchHMI)( HMI *self, HMI *p_hmi);
	void		(*switchBack)( HMI *self);
	void		(*hitHandle)( HMI *self, char *s_key);
	void		(*dhitHandle)( HMI *self, char *s_key);
	void		(*longpushHandle)( HMI *self,  char *s_key);
	void		(*conposeKeyHandle)( HMI *self, char *s_key1, char *s_key2);
};

typedef   const char  ro_char;


//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern const Except_T Hmi_Failed;
//extern  ro_char str_endRow[];
//extern	ro_char str_endCol[];
extern HMI *g_p_curHmi;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
int Is_rowEnd( const char *str);
int Is_colEnd( const char *str);
#endif
