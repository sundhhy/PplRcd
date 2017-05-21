#ifndef __TEXPRESSION_H_
#define __TEXPRESSION_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "Composition.h"


#define VAR_LEN		3

ABS_CLASS( Expr)
{
	Composition 	*ction;
	char  variable[ VAR_LEN + 1];
	
	void ( *setVar)( Expr *self, char *variable);
//	void ( *setFather)( Expr *self, Expr *fa);
	void ( *setCtion)( Expr *self, Composition *ct);
	
	int	( *str2colour)( char *s);
	int	( *str2font)( char *s);
	
	abstract void* (*interpret)( Expr *self, void *father, void *context);
	
};

#endif
