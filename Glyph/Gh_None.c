#include "Gh_none.h"



static GhNone *signalGhNone;


GhNone *Get_GhNone(void)
{
//	Glyph *gh;
	
	if( signalGhNone == NULL)
	{
		signalGhNone = GhNone_new();
		if(signalGhNone  == NULL) while(1);
		
		
	}
	return signalGhNone;
}



CTOR( GhNone)
SUPER_CTOR( Glyph);


END_CTOR
