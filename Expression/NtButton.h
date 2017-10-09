#ifndef __NTBUTTON_H_
#define __NTBUTTON_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "expression.h"


CLASS( NtButton)
{
	IMPLEMENTS( Expr);
	
};


NtButton *GetNtButton(void);
#endif
