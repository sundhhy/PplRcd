#ifndef __INC_PicExpr_H__
#define __INC_PicExpr_H__
#include "PicExpr.h"
#include "lw_oopc.h"
#include "expression.h"

CLASS( PicExpr)
{
	EXTENDS( Expr);
	
};

PicExpr *GetPictExpr(void);
#endif
