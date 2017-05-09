#ifndef __COMPOSITOR_H_
#define __COMPOSITOR_H_
#include <stdint.h>
//#include "arithmetic/list.h"
#include "Glyph.h"
#include "lw_oopc.h"

#include "Composition.h"




INTERFACE( Compositor)
{

	int ( *compose)( Composition *ction, Glyph *gh);
	
};

CLASS( SimpCtor)
{
	IMPLEMENTS( Compositor);
};

SimpCtor *Get_SimpCtor(void);

#endif
