#ifndef __TEXPRESSION_H_
#define __TEXPRESSION_H_
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include "stdint.h"
#include "lw_oopc.h"
#include "Composition.h"
#include "Reader.h"
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
#define VAR_LEN		32
#define TEMPBUF_LEN		96

#define SET_ATT_OK		0

#define SET_ATT_BSIZEFAIL		1
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
ABS_CLASS( Expr)
{
	Composition 	*ction;
	char  variable[ VAR_LEN + 1];
	
	void ( *setVar)( Expr *self, char *variable);
//	void ( *setFather)( Expr *self, Expr *fa);
	void ( *setCtion)( Expr *self, Composition *ct);
	
//	int	( *str2colour)( char *s);
//	int	( *str2font)( char *s);
//	int	( *getBgc)( char *s);
//	int	( *getAreaEff)( char *s);
//	int	( *getCntEff)( char *s);
	
	abstract void* (*interpret)( Expr *self, void *father, void *context);
	abstract void* (*inptSht)( Expr *self, void *context, sheet *p_sht);
	
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

extern char expTempBUf[TEMPBUF_LEN];
extern const Except_T Exp_Failed;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
int Set_shtContextAtt( char *p_att, sheet *p_sht);
int Set_shtAreaAtt( char *p_att, sheet *p_sht);
#endif
