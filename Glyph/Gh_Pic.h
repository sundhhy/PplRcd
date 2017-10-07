#ifndef __INC_Gh_pic_H_
#define __INC_Gh_pic_H_
#include "Glyph.h"
#include "lw_oopc.h"
#include <stdint.h>
#include "arithmetic/cycQueue.h"

#define SUBTYPE_NONE		0
#define SUBTYPE_CPIC		1
#define SUBTYPE_BPIC		2
#define SUBTYPE_ICON		3


CLASS(GhPic)
{
	EXTENDS( Glyph);

	
};


GhPic *Get_GhPic(void);
#endif
