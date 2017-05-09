#ifndef __MODEL_H_
#define __MODEL_H_
#include <stdint.h>
//#include "arithmetic/list.h"
#include "lw_oopc.h"
#include "Glyph.h"



CLASS( Composition)
{
//	List_T	Lgh;
	uint16_t	x;
	uint16_t	y;
	void	*ctor;
	
	uint16_t	lcdWidth, lcdHeight;
	uint16_t	ghWidth, ghHeight;
	
	void ( *setCtor)( Composition *self, void *ctor);
	
	int ( *clean)( Composition *self);
	int ( *insert)( Composition *self, Glyph *gh);
	int ( *addRow)( Composition *self);
};


Composition *Get_Composition(void);

#endif
