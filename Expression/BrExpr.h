#ifndef __BREXPR_H_
#define __BREXPR_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "expression.h"

CLASS( BrExpr)
{
	IMPLEMENTS( Expr);
	
};

BrExpr *GetBrExpr(void);
#endif
