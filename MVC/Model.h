#ifndef __MODEL_H_
#define __MODEL_H_
#include <stdint.h>
//#include "list.h"
#include "lw_oopc.h"

INTERFACE( Observer)
{
	int ( *update)( Observer *self);
	
	
};


ABS_CLASS( Model)
{
	void *arg;
	
};

#endif
