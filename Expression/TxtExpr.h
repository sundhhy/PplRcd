#ifndef __TXTEXPR_H_
#define __TXTEXPR_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "expression.h"

CLASS( TxtExpr)
{
	IMPLEMENTS( Expr);
	
};

TxtExpr *GetTxtExpr(void);
#endif
