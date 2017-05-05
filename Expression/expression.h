#ifndef __TEXPRESSION_H_
#define __TEXPRESSION_H_
#include "stdint.h"
#include "lw_oopc.h"

#define VAR_LEN		3

ABS_CLASS( Expr)
{
	char  variable[ VAR_LEN + 1];
	
	void ( *setVar)( Expr *self, char *variable);
	
	abstract int (*interpret)( Expr *self, void *context);
	
};

#endif
