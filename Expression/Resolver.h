#ifndef __NONTERMINALEXP_H_
#define __NONTERMINALEXP_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "expression.h"


ABS_CLASS( Resolver)
{
	int ( *getEle)( char *varName, void **value);
	
};

#endif
