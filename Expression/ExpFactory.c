#include "ExpFactory.h"
#include <string.h>
#include "TxtExpr.h"
#include "NtButton.h"

Expr *ExpCreate( char *type)
{
	//不注意大小写
	if( !strcasecmp( type, "h1"))
	{
		return (Expr *)GetTxtExpr();
	}
	if( !strcasecmp( type, "h2"))
	{
		return (Expr *)GetTxtExpr();
	}
	if( !strcasecmp( type, "h3"))
	{
		return (Expr *)GetTxtExpr();
	}
	if( !strcasecmp( type, "h4"))
	{
		return (Expr *)GetTxtExpr();
	}
	if( !strcasecmp( type, "h5"))
	{
		return (Expr *)GetTxtExpr();
	}
	if( !strcasecmp( type, "h6"))
	{
		return (Expr *)GetTxtExpr();
	}
	if( !strcasecmp( type, "bu"))
	{
		return (Expr *)GetNtButton();
	}
	
	
	return NULL;
	
}





