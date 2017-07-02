#ifndef __INC_Dyn_TimeExpr_H_
#define __INC_Dyn_TimeExpr_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "expression.h"

CLASS( Dyn_TimeExpr)
{
	EXTENDS( Expr);
	
};

Dyn_TimeExpr *GetTimeExpr(void);
#endif
