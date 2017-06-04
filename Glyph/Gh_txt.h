#ifndef __GH_TXT_H_
#define __GH_TXT_H_
#include "Glyph.h"
#include "lw_oopc.h"
#include <stdint.h>
#include "arithmetic/cycQueue.h"


CLASS(GhTxt)
{
	EXTENDS( Glyph);
//	uint8_t 	*buf;
//	char 		*cq_buf;
//	CycQueus_t   cq;
	
};


GhTxt *Get_GhTxt(void);
#endif
