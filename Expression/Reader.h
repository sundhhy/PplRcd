#ifndef __Reader_H_
#define  __Reader_H_
#include <stdint.h>

int GetName( char *context, char *name, int nameLen);
int GetAttribute( char *context, char *att, int attLen);
void *GetNameVale( char *context, char *name, char **value, int *len);
char *RemoveHead( char *context);
char *RemoveTail( char *context, char *tailName, int size);
#endif
