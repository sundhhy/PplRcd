#ifndef __TXTEXPR_H_
#define __TXTEXPR_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "expression.h"

CLASS( TxtExpr)
{
	EXTENDS( Expr);
	
};

TxtExpr *GetTxtExpr(void);
#endif
