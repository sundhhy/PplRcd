#ifndef __Reader_H_
#define  __Reader_H_
#include <stdint.h>

int GetName( char *context, char *name, int nameLen);
void *GetNameVale( char *context, char *name, char **value, int *len);

#endif
