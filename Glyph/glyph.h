#ifndef __GLYPH_H_
#define __GLYPH_H_
#include "lw_oopc.h"
#include <stdint.h>
ABS_CLASS( Glyph)
{
	
	uint16_t	font;
	uint16_t	colour;
	
	//abs func
	abstract void (*draw)( Glyph *self, int x, int y);
	
};
void View_init(void);
void View_test(void);
#endif
