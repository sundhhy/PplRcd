#include "testView.h"
#include "Composition.h"
#include "Compositor.h"
#include "Reader.h"
#include "ExpFactory.h"



#define  TEST_CONTEXT  2



#if TEST_CONTEXT == 1
static const char testContext[] = \
"<h1 c=red >12点阵全字库!</> <h2 c=blue>16点阵全字库!</>\
<h3 c=yellow>2白日依山尽!</><h4 c=gren>黄河入海流流流</><h5>欲2穷千里目</><h6 c=purple>更上一层楼楼!</><h1 c=red >12点阵全字库!</>";
#elif TEST_CONTEXT == 2
static const char testContext[] = \
"<bu><h2 c=red >确认</></><bu><h2 c=red >返回</></>";


#endif




void View_test(void)
{
//	TestViewShow();
	void *pp = (void *)testContext;
	char	name[8];
	int		nameLen;
	Composition *ct = Get_Composition();
	Compositor *ctor = (Compositor *)Get_SimpCtor();
	Expr *myexp ;
	
	//设置排版的排版算法
	ct->setCtor( ct, ctor);
	ct->clean( ct);
	

	
	while(1)
	{
		nameLen = 8;
		nameLen = GetName( pp, name, nameLen);
		if( nameLen == 0)
			break;
		
		myexp = ExpCreate( name);
		if( myexp == NULL)
			break;
		
			//设置排版
		myexp->setCtion( myexp, ct);
		myexp->setVar( myexp, name);		//跟据Context中的变量来设置
		pp = myexp->interpret( myexp, NULL, pp);
		
	}
		
	ct->flush( ct);
	

}



