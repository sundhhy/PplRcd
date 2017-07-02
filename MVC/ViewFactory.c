#include "ViewFactory.h"
#include <string.h>
#include "testView.h"


View *aView[4] ;
View *ViewCreate( char *type)
{
	char *p;
	
	p = strstr( type, "test");
	
	if( p)
	{
		if(aView[0] == NULL)
			aView[0] = (View *)TestView_new();
		return aView[0];
	}
	
	
	return NULL;
	
}





