#ifndef __NTGRID_H_
#define __NTGRID_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "expression.h"


CLASS( NtGrid)
{
	IMPLEMENTS( Expr);
	
};


NtGrid *GetNtGrid(void);

#endif
