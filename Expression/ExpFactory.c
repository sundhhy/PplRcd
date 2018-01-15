#include "ExpFactory.h"
#include <string.h>
#include "TxtExpr.h"
#include "NtButton.h"
#include "NtInput.h"
#include "GeometricsExpr.h"
#include "PicExpr.h"


Expr *ExpCreate( char *type)
{
	char *p;
	
	p = strstr( type, "/");
	if( p)
	{
		
		return NULL;
	}
	
	p = strstr( type, "title");
	if( p)
	{
		return (Expr *)GetTxtExpr();
	}
	p = strstr( type, "text");
	if( p)
	{
		return (Expr *)GetTxtExpr();
	}
	
	p = strstr( type, "input");
	if( p)
	{
		return (Expr *)GetNtInput();
	}
	p = strstr( type, "box");
	if( p)
	{
		return (Expr *)GetGmtrExpr();
	}
	
	p = strstr( type, "rct");
	if( p)
	{
		return (Expr *)GetGmtrExpr();
	}
	p = strstr( type, "line");
	if( p)
	{
		return (Expr *)GetGmtrExpr();
	}

	p = strstr( type, "pic");
	if( p)
	{
		return (Expr *)GetPictExpr();
	}
	
	

	
	if( !strcasecmp( type, "bu"))
	{
		return (Expr *)GetNtButton();
	}

	

//	err:		
	Except_raise(&Exp_Failed, __FILE__, __LINE__);
	
	return NULL;
	
}





