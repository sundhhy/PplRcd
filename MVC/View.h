#ifndef __VIEW_H_
#define __VIEW_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "glyph.h"
#include "arithmetic/list.h"
#include "View.h"
#include "expression.h"

ABS_CLASS( View)
{
	void*		myModel;
	void*		myController;
	
	
	int		(*show)( View *self, void *arg);
	
};


#endif
