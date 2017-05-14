
#include <string.h>

#include "Reader.h"

//< name> val </>
#define BEGIN_FLAG	'<'
#define END_FLAG	'>'
#define ATT_SPLIT_CHAR	' '

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
		
		if( pp[ idx] == ATT_SPLIT_CHAR)
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

//该函数将属性一次性全部取出
//att: c=red oth=someval

int GetAttribute( char *context, char *att, int attLen)
{
	char		*pp;
	short 		idx = 0;
	char		hasAtt = 0;
	uint8_t		count = 0;
	pp = strchr( context, BEGIN_FLAG);
	if( pp == NULL)
		goto exit;
	pp++;
	
	memset( att, 0, attLen);
	
	//去除空格
	pp = Eliminate_char( pp, ' ');
	
	//找到属性分隔符
	idx = 0;
	hasAtt = 0;
	count = 0;
	
	//留一个空间给结束符
	attLen --;
	
	while(1)
	{
		if( count > attLen)
			break;
		
		if( pp[ idx] == ATT_SPLIT_CHAR)
		{
			hasAtt = 1;		//在结束符之前找到了属性分隔符
		}
		
		
		
		if( pp[ idx] == END_FLAG)
		{
			
			break;
		}
		else
		{
			if( hasAtt)
				att[ count ++] = pp[ idx];
			
		}
		
		
		idx++;
		
		
	}
	
	
	
	exit:
	return count;
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

