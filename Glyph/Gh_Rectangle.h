#ifndef __GH_RECTANGLE_H_
#define __GH_RECTANGLE_H_
#include "Glyph.h"
#include "lw_oopc.h"
#include <stdint.h>
#include "arithmetic/cycQueue.h"

#define GMT_LINE		1
#define GMT_RECT		2

CLASS(GhRectangle)
{
	EXTENDS( Glyph);
	

	
	
};


GhRectangle *Get_GhRectangle(void);
#endif
