#ifndef __GH_TXT_H_
#define __GH_TXT_H_
#include "Glyph.h"
#include "lw_oopc.h"

CLASS(GhTxt)
{
	EXTENDS( Glyph);
	char *txt;
	int   len;
	
};


GhTxt *Get_GhTxt(void);
#endif
