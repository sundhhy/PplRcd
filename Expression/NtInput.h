#ifndef __NTINPUT_H_
#define __NTINPUT_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "expression.h"


CLASS( NtInput)
{
	IMPLEMENTS( Expr);
	
};


NtInput *GetNtInput(void);

#endif
