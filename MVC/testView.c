
#include "testView.h"
#include "Composition.h"
#include "Compositor.h"
#include "Reader.h"
#include "ExpFactory.h"
#include "Model.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define  TEST_CONTEXT  4



#if TEST_CONTEXT == 1
static const char testContext[] = \
"<h1 c=red >12点阵全字库!</><h2 c=blue>16点阵全字库!</>\
<h3 c=yellow>2白日依山尽!</><h4 c=gren>黄河入海流流流</><h3>欲2穷千里目</><h6 c=purple>更上一层楼楼!</><h1 c=red >12点阵全字库!</>";
#elif TEST_CONTEXT == 2
static const char testContext[] = \
"<bu><h2 c=red >确认</></><bu><h2 c=red >返回</></>";
#elif TEST_CONTEXT == 3

static const char testContext[] = \
"<h2 c=red >组态</> <br/>\
<br/>\
<bu c=blue ><h2 c=red >系统组态</></bu><bu c=blue ><h2 c=red> 显示组态</></bu><br/>\
<bu c=blue ><h2 c=red >输入组态</></bu><bu c=blue ><h2 c=red> 输出组态</></bu><br/>\
<bu c=blue ><h2 c=red >记录组态</></bu><bu c=blue ><h2 c=red> 报警组态</></bu><br/>\
<bu c=blue ><h2 c=red >表报组态</></bu><bu c=blue ><h2 c=red> 打印组态</></bu><br/>\
<bu c=blue ><h2 c=red >通信组态</></bu><bu c=blue ><h2 c=red> 系统信息</></bu><br/>\
<br/>\
<bu c=yellow><h2 c=purple>组态文件</></bu><bu c=yellow><h2 c=purple>退出</></bu><br/>";
#elif TEST_CONTEXT == 4
//static const char testContext[] = \
//"<title bkc=black  f=24 ali=l>设置</> \
//<gr ali=m cg=2  id=0x01 > <text f=24 clr=black >Passwd:</> <rct bkc=black x=96 y=30></></gr> \
//<gr cols=2 cg=2 ls=2 f=24 bkc=black ali=m x=96 y=30 > <bu ><text >System</></bu><bu  ><text > View</></bu>\
//<bu ><text >Input</></bu><bu ><text > Output</></bu></gr>";

static   char* const testContext[2] = {	\
"<title bkc=black clr=blue f=24 xali=l>设置</> \
<gr cols=2 xali=m   id=0x01 > <text f=16  yali=m clr=blue >密码:</> <rct bkc=black x=126 y=30></></gr> \
<gr cols=2 cg=2 ls=2 f=16 bkc=black clr=blue xali=m x=126 y=30 >\
<bu ><text yali=m >系统设置</></bu><bu  ><text yali=m > 通道设置</></bu>\
<bu ><text yali=m >报警设置</></bu><bu ><text yali=m > 算法设置</></bu> \
<bu ><text yali=m >显示设置</></bu><bu ><text yali=m > 数据备份</></bu>\
<bu ><text yali=m >数据打印</></bu><bu ><text yali=m > 退出</></bu>\
</gr> \
<text f=16 bkc=white clr=red xali=m>重要数据，非专业人士请退出</> ", \
//动态画面部分
"<time bndx1=180 bndx2=320 bndy1=0  bndy2=24 f=24 xali=m bkc=black clr=yellow id=0 spr=/> </time>" \


};

#endif

//通过id绑定的指针
static  void * const dynId_ptr[2] = { &g_SysTime, NULL};

//背景色 "none" 或某种颜色
#define SCREENBKC		"white"
#define LINESPACING		4
#define COLUMGRAP		0
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
/* Cycle/Sync Callback functions */


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

void View_test(void)
{
//	TestViewShow();
	char *pp = (void *)testContext[0];
	char	name[8];
	short		nameLen;
	short		i = 0;
	Composition *ct = Get_Composition();
	Compositor *ctor = (Compositor *)Get_SimpCtor();
	Expr *myexp ;
	
	//设置排版的排版算法
	ct->lineSpacing = LINESPACING;
	ct->columnGap = COLUMGRAP;
	ct->setCtor( ct, ctor);
	ct->setSCBkc( ct, SCREENBKC);
	ct->clean( ct);
	
	ct->dynPtr = (void *)dynId_ptr;
	
	i = 0;
	while( i < 2)
	{
		pp = (char *)testContext[i ++];
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
		
	}
	
	
	
		
	ct->flush( ct);
	

}


//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static int TestView_init( View *self, void *arg)
{
	
	View_test();
	
	return RET_OK;
}

static int		TestView_show( View *self, void *arg)
{
	Model *m = ( Model *)( self->myModel);
	Composition *ct = Get_Composition();
	
	m->getMdlData( m, 0, &g_SysTime);
	
	ct->dynShow( ct);
	return RET_OK;
}

CTOR( TestView)
SUPER_CTOR( View);
FUNCTION_SETTING( View.init, TestView_init);
FUNCTION_SETTING( View.show, TestView_show);
END_CTOR













