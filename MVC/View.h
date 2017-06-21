#ifndef __VIEW_H_
#define __VIEW_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "glyph.h"
#include "arithmetic/list.h"
#include "View.h"
#include "sdhDef.h"
//#include "expression.h"

ABS_CLASS( View)
{
	void*		myModel;
	void*		myController;
	
	//œ‘ ææ≤Ã¨ª≠√Ê
	int		( *init)( View *self, void *arg);
//	int		( *setMdl)( View *self, void *m);
//	int		( *setMdl)( View *self, void *c);
	int		(*show)( View *self, void *arg);
	
};


extern struct  tm g_SysTime;

#endif
