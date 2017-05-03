#ifndef __VIEW_H_
#define __VIEW_H_
#include "stdint.h"
#include "lw_oopc.h"
#include "glyph.h"

ABS_CLASS( View)
{
	void*		myModel;
	void*		myController;
	
	
	int		(*draw)( View *self, void *arg);
}


#endif
