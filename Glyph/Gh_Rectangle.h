#ifndef __GH_RECTANGLE_H_
#define __GH_RECTANGLE_H_
#include "Glyph.h"
#include "lw_oopc.h"
#include <stdint.h>
#include "arithmetic/cycQueue.h"


CLASS(GhRectangle)
{
	EXTENDS( Glyph);
	
	//只好通过font这个成员来设置宽和高了
	uint16_t	width;
	uint16_t	height;
	
	
};


GhRectangle *Get_GhRectangle(void);
#endif
