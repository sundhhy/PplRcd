#include "testView.h"
#include "TxtExpr.h"

 
static const char testContext[] = \
" <body> <h3>疯狂Java讲义</h3> </body>";



void View_test(void)
{
//	TestViewShow();
	char *pp;
	Expr *myexp = (Expr* )GetTxtExpr();
	myexp->setVar( myexp, "h3");		//跟据Context中的变量来设置
	myexp->interpret( myexp, (void *)testContext);
	
}



