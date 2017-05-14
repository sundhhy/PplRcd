
#include <string.h>

#include "Reader.h"

//< name> val </>
#define BEGIN_FLAG	'<'
#define END_FLAG	'>'

static char *Eliminate_char( char *str, char c);

//返回获取的名字的长度
int GetName( char *context, char *name, int nameLen)
{
	char	*pp;
	int 	idx = 0;
	pp = strchr( context, BEGIN_FLAG);
	if( pp == NULL)
		goto exit;
	pp++;
	//去除空格
	pp = Eliminate_char( pp, ' ');
	
	//留一个空间给结束符
	nameLen --;
	
	//取值
	while(1)
	{
		if( idx >= nameLen)
			break;
		if( pp[ idx] == END_FLAG)
			break;
		name[idx] = pp[ idx];
		idx ++;
		
	}
	
	name[ idx] = '\0';
	
	exit:
	return idx;
}	

void *GetNameVale( char *context, char *name, char **value, int *len)
{
	
	char	*pp;
	pp = strstr((const char*)context, name);
	if( pp == NULL)
	{
		*len = 0;
		return context;
	}
	while( *pp != END_FLAG)
		pp ++;
	pp ++;
	*value = pp;
	while( *pp++ != BEGIN_FLAG)
	{
			(*len) ++;
	}
	
	while( *pp != END_FLAG)
		pp ++;
	
	
	return pp;
}


static char *Eliminate_char( char *str, char c)
{
	while( *str == c && *str != '\0')
		str ++;
	
	return str;
	
}

