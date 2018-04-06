#include "ModelFactory.h"
#include <string.h>


Model *aMdl[4] ;
Model *Create_model( char *type)
{
	char *p;
	
	p = strstr( type, "time");
	
	if( p)
	{
		if(aMdl[0] == NULL)
		{
			aMdl[0] = (Model *)ModelTime_new();
			if(aMdl[0]  == NULL) while(1);
			
		}
			
		return aMdl[0];
	}
	
//	p = strstr( type, "test");
//	
//	if( p)
//	{
//		
//		return SUPER_PTR( Get_ModeTest(), Model);
//	}
	
	p = strstr( type, "chn_0");
	if( p)
	{
		
		return SUPER_PTR(Get_Mode_chn(0), Model);
	}
	p = strstr( type, "chn_1");
	if( p)
	{
		
		return SUPER_PTR(Get_Mode_chn(1), Model);
	}
	p = strstr( type, "chn_2");
	if( p)
	{
		
		return SUPER_PTR(Get_Mode_chn(2), Model);
	}
	p = strstr( type, "chn_3");
	if( p)
	{
		
		return SUPER_PTR(Get_Mode_chn(3), Model);
	}
	p = strstr( type, "chn_4");
	if( p)
	{
		
		return SUPER_PTR(Get_Mode_chn(4), Model);
	}
	p = strstr( type, "chn_5");
	if( p)
	{
		
		return SUPER_PTR(Get_Mode_chn(5), Model);
	}
	
	
	Except_raise(&model_Failed, __FILE__, __LINE__);
	return NULL;
	
}





