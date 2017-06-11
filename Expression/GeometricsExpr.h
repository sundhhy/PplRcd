#ifndef __GeometricsExpr_H_
#define __GeometricsExpr_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "expression.h"

CLASS( GmtrExpr)
{
	IMPLEMENTS( Expr);
	
};

GmtrExpr *GetGmtrExpr(void);
#endif
