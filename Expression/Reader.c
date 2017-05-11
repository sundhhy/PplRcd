
#include <string.h>

#include "Reader.h"

void *GetNameVale( char *context, char *name, char **value, int *len)
{
	
	char	*pp;
	pp = strstr((const char*)context, name);
	if( pp == NULL)
	{
		*len = 0;
		return context;
	}
	while( *pp != '>')
		pp ++;
	pp ++;
	*value = pp;
	while( *pp++ != '<')
	{
			(*len) ++;
	}
	
	while( *pp != '>')
		pp ++;
	
	
	return pp;
}