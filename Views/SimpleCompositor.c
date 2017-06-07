#include "Compositor.h"
#include "device.h"

#include "Composition.h"


#include "basis/sdhDebug.h"
#include "basis/sdhError.h"

SimpCtor *signalSimpCtor;


static void DspViewData(void **ppvd, void *cl);
static void Layout(void **ppvd, void *cl);

SimpCtor *Get_SimpCtor(void)
{
	
	if( signalSimpCtor == NULL)
	{
		signalSimpCtor = SimpCtor_new();
		
	}
	
	return signalSimpCtor;
}

//




static int Simp_VdLayout( Composition *ction)
{
	//数据中单个元素在x轴和y轴上的尺寸
	List_map( ction->t_vd, Layout, ction);
	
	
	
	return RET_OK;
}

static int Simp_Show( Composition *ction)
{
	
	
	//显示数据
	List_map( ction->t_vd, DspViewData, ction);
	
	return RET_OK;
}

static void SumChldSize(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	ViewData_t *pPvd = pvd->paraent;
	
	
	
	
	pPvd->dspArea.sizeX += pvd->dspArea.sizeX *  pvd->dspCnt.len;
	if( pPvd->dspArea.sizeY < pvd->dspArea.sizeY)
		pPvd->dspArea.sizeY = pvd->dspArea.sizeY;
	
	//
	pPvd->dspArea.sizeX += pPvd->columnGap;
//	pPvd->dspArea.sizeY += pPvd->lineSpacing;
	
}

//认为所有的子图元都是能在一行内显示id
static void LayoutChld(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	ViewData_t *pPvd = pvd->paraent;
	short	totalX = 0;
	
	totalX = pvd->dspArea.sizeX * pvd->dspCnt.len;
	
	pvd->dspArea.useArea.x1 = pPvd->dspArea.cursorX ;
	pvd->dspArea.useArea.x2 = pPvd->dspArea.cursorX + totalX;
	pvd->dspArea.useArea.y1 = pPvd->dspArea.cursorY;
	pvd->dspArea.useArea.y2 = pPvd->dspArea.cursorY + pvd->dspArea.sizeY;
	
	pPvd->dspArea.cursorX += totalX + pPvd->columnGap;
}

//这个算法对任何不能在一行内显示的图元都不处理
static void Layout(void **ppvd, void *cl)
{
	ViewData_t *pvd;
	Composition	*pction = ( Composition *)cl;
//	char 	numGpInRow = 0;
//	char 	numRow = 1;
	short	totalX = 0;
	short	boundaryX = 0;
 
	pvd = *ppvd;
	//先设置一个非法值
	pvd->dspArea.useArea.x1 = 0;
	pvd->dspArea.useArea.y1 = 0;
	pvd->dspArea.useArea.x2 = 0;
	pvd->dspArea.useArea.y2 = 0;
	
	if( pvd->dspArea.boundary == NULL)
	{
		pvd->dspArea.boundary = &pction->mySCI.scBoundary;
	}
	
	List_map( pvd->t_childen, SumChldSize, cl);
	
	
	boundaryX = pvd->dspArea.boundary->x2 - pvd->dspArea.boundary->x1;
	if( pvd->dspArea.sizeY > ( pvd->dspArea.boundary->y2 - pvd->dspArea.boundary->y1) )
		return;
	//计算总共需要的行宽, 先不考虑多行
	totalX = pvd->dspArea.sizeX * pvd->dspCnt.len;
	if( totalX > boundaryX)
	{
		return;
		
	}
	
	
	pvd->dspArea.curScInfo = &pction->mySCI;
	//设置行和列
	if( pvd->dspArea.curScInfo->rowSize < pvd->dspArea.sizeY)
		pvd->dspArea.curScInfo->rowSize = pvd->dspArea.sizeY;
	if( pvd->dspArea.curScInfo->colSize < pvd->dspArea.sizeX)
		pvd->dspArea.curScInfo->colSize = pvd->dspArea.sizeX;
	
	
	
	//对需要处理对齐的，要根据显示的整体尺寸来确定起始位置
	if( pvd->dealAli  || pvd->dspArea.ali == ALIGN_MIDDLE)
	{
		pvd->dspArea.useArea.x1 = pction->mySCI.cursorX + ( ( boundaryX - totalX) >> 1);
	}
	else if( pvd->dealAli || pvd->dspArea.ali == ALIGN_RIGHT)
	{
		pvd->dspArea.useArea.x1 = pction->mySCI.cursorX + boundaryX - totalX;
		
	}
	else if( pvd->dealAli == 0 || pvd->dspArea.ali == ALIGN_LEFT)
	{
	
		//将分配的区域x1,y1,x2,y2都初始化为光标的位置
		pvd->dspArea.useArea.x1 = pction->mySCI.cursorX;
		
		
	}
	pvd->dspArea.numRow = 1;
	pvd->dspArea.useArea.y1 = pction->mySCI.cursorY;
	pvd->dspArea.useArea.x2 = pvd->dspArea.boundary->x2;;
	pvd->dspArea.useArea.y2 = pction->mySCI.cursorY ;
	
	pvd->dspArea.cursorX = pvd->dspArea.useArea.x1;
	pvd->dspArea.cursorY = pvd->dspArea.useArea.y1 ;
	List_map( pvd->t_childen, LayoutChld, cl);
	

	//光标跳到下一行
	pction->mySCI.cursorX = 0;
	pction->mySCI.cursorY += pvd->dspArea.curScInfo->rowSize;
	
//	//每行能显示的数量
//	numGpInRow = ( pvd->dspArea.boundary->x2 - pvd->dspArea.boundary->x1) / pvd->dspArea.sizeX;
//	
//	numRow += pvd->dspCnt.len / numGpInRow;
//	pvd->dspArea.numRow = numRow;
//	pvd->dspArea.useArea.x2 = pvd->dspArea.boundary->x2;
//	pvd->dspArea.useArea.y2 += ( numRow - 1) * pvd->dspArea.curScInfo->rowSize;
//	
//	//移动光标
//	pction->mySCI.cursorY += pvd->dspArea.useArea.y2;
	
	//补充行间距 列间距
	pction->mySCI.cursorY += pction->lineSpacing;
	pction->mySCI.cursorX += pction->columnGap;
	
}

static void DspViewData(void **ppvd, void *cl)
{
	ViewData_t *pvd;
	Composition	*pction = ( Composition *)cl;
	
	if( ppvd == NULL)
		return ;
	
	pvd = *ppvd;
	
	if( pvd->gh)
		pvd->gh->draw( pvd->gh, &pvd->dspCnt, &pvd->dspArea);
	
	List_map( pvd->t_childen, DspViewData, cl);
	
}























//无用

//static void VDShow(void **ppvd, void *cl)
//{
//	ViewData_t *pvd = *ppvd;
//	Composition	*pction = ( Composition *)cl;
//	//每行能够显示的量	
//	short 	numInrow = 0;
//	
//	
//	short	displayed = 0;	//已经被显示的数量
//	short	 y = 0;
//	short 	xend = 0 ;
//	short	xstart	= 0, ableNum;
//	char	c;
//	
//	int16_t		oldx1 = pvd->area_x1;
//	int16_t		oldy1 = pvd->area_y1;
//	
//	//设置字体和颜色
//	pvd->gh->setClu( pvd->gh, pvd->colour);
//	pvd->gh->setFont( pvd->gh, pvd->font);
//	pvd->gh->insert( pvd->gh, pvd->data, pvd->len);
//	
//	pvd->gh->setWidth( pvd->gh, pvd->size_x);
//	pvd->gh->setHeight( pvd->gh, pvd->size_y);
//#if 1	
//	//设置行和列
//	if( pction->rowSize[ pction->step] < pvd->size_y)
//		pction->rowSize[ pction->step] = pvd->size_y;
//	if( pction->colSize[ pction->step] < pvd->size_x)
//		pction->colSize[ pction->step] = pvd->size_x;
//	

//	displayed = 0;
//	//一行行显示
//	for( y = pvd->area_y1; y <= pvd->area_y2; y += pction->rowSize[ pction->step])
//	{
//		//在第一行，x轴起始位置是分配区域的x轴起始位置
//		if( y == pvd->area_y1)
//			xstart = pvd->area_x1;
//		else
//			xstart = 0;
//		//在最后一行，x轴的结束位置是分配区域的结束位置
//		if( y == pvd->area_y2 || pvd->area_y1 == pvd->area_y2)
//			xend = pvd->area_x2;
//		else
//			xend = pction->lcdArea.LcdSizeX;
//		
//		ableNum = ( xend - xstart) / pvd->size_x;
//		
//		//计算这一行能显示几个字符
//		numInrow  = 0;
//		while(1)
//		{
//			c = pvd->data[ displayed];
//			
//			if( IS_CHINESE( c))
//			{
//				if( ableNum > 1)
//				{
//					numInrow += 2;
//					displayed += 2;
//					ableNum -= 2;
//				}
//				
//				
//				
//			}
//			else
//			{
//				if( ableNum >  0)
//				{
//					numInrow ++;
//					displayed ++;
//					ableNum -= 1;
//				}	
//				
//			}
//			//如果显示够了就退出
//			if( displayed >= pvd->len)
//				break;
//			//如果本行已经没有空间了，就显示本行
//			if( ableNum == 0)
//				break;
//			//如果本行还能显示一个字符，看看下一个是不是能放进去了
//			if( ableNum == 1)
//			{		
//				c = pvd->data[ displayed];
//				if( !IS_CHINESE( c))
//				{
//					displayed ++;
//					numInrow ++;
//				}
//				break;
//			}
//				
//		}
//		pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, numInrow);
//		
//		//增加一行
//		pvd->area_x1 = 0;
//		pvd->area_y1 += pction->rowSize[ pction->step];
//		
//	}
//	
//	
//	pvd->done = displayed;
//	
//	
//	//恢复起始位置，方便下次显示
//	pvd->area_x1 = oldx1;
//	pvd->area_y1 = oldy1;

//#endif	
//	//显示子图元
//	List_map( pvd->t_childen, ShowChild, cl);
//}



//static void DealChild(void **ppvd, void *cl)
//{
//	ViewData_t *pvd = *ppvd;
//	ViewData_t *pPvd = pvd->paraent;
//	
//	
//	
//	//为了对称所以父元素的尺寸/2
////	pvd->area_x1 = pPvd->area_x1 + pPvd->size_x/2;
////	pvd->area_y1 = pPvd->area_y1 + pPvd->size_y/2;
//	
//	
//	if( SET_CHILDASWHOLE( pPvd->childAttr))
//	{
//		
//		pPvd->size_x += pvd->size_x * pvd->len;
//		
//	}
//	else
//	{
////		pPvd->size_x += pvd->size_x;
//		
//		
//	}
//	pPvd->size_y += pvd->size_y;
//	
//}

//static void VDapply(void **ppvd, void *cl)
//{
//	ViewData_t *pvd = *ppvd;
//	Composition *pction = ( Composition *)cl;
//	area_t	*area = &pction->lcdArea;
//	char		c;
//	uint8_t		dealNum = 0;
//	uint8_t		dealIdx = 0;
//	uint8_t		rowNum = 0;
//	uint16_t	usableX, usableY;
//	uint16_t	needX, needY;
//	
//	List_map( pvd->t_childen, DealChild, NULL);

//	
//	
//	//一个字符一个字符来分配
//	
//	
//	
//	//将分配的区域x1,y1,x2,y2都初始化为光标的位置
//	pvd->area_x1 = area->cursorX;
//	pvd->area_y1 = area->cursorY;
//	pvd->area_x2 = area->cursorX;
//	pvd->area_y2 = area->cursorY;
//	
//	pvd->dspArea.x1 = area->cursorX;
//	pvd->dspArea.y1 = area->cursorY;
//	pvd->dspArea.x2 = area->cursorX;
//	pvd->dspArea.y2 = area->cursorY;
//	
//	  
//	
//	//设置行和列
//	if( pction->rowSize[ pction->step] < pvd->size_y)
//		pction->rowSize[ pction->step] = pvd->size_y;
//	if( pction->colSize[ pction->step] < pvd->size_x)
//		pction->colSize[ pction->step] = pvd->size_x;
//	
//	//如果一个x轴一个字符也放不下就直接退出
//	if( pvd->size_x > area->LcdSizeX)
//		return;
//	
//	
//	

//	//初始化处理字符索引为0
//	dealIdx = 0; 
//	while( dealIdx < pvd->len)
//	{
//	
//	//取出一个字符,设置处理长度为1
//		c = pvd->data[ dealIdx];
//		dealNum = 1;
//	//判断是否为汉字，是汉字的话处理长度设置为2
//		if( IS_CHINESE( c))
//		{
//			dealNum = 2;
//			
//		}
//		//是换行符的话就直接换行
//		if( IS_BR( c))
//		{
//			pction->addRow( pction);
//			
//			rowNum ++;
//			dealIdx ++;
//			continue;
//		}
//		
//			
//	//计算本次处理需要的x轴空间
//		if( IS_TAB( c))
//		{
//			
//			needX = 4 * pvd->size_x;
//		}
//		else
//		{
//			needX = dealNum * pvd->size_x;
//		}
//		
//	//计算本次处理需要的y轴空间
//		needY =  pvd->size_y;
//		while(1)
//		{
//			//使用光标位置和屏幕的界限来计算x轴和y轴上的空闲空间 A
//		
//			usableX = area->LcdSizeX - area->cursorX;
//			usableY = area->LcdSizeY - area->cursorY;
//			//如果y轴方向放的下
//			if( needY < usableY)
//			{
//				//如果x轴放的下，光标在x轴移动相应的位置，处理索引也移动相应的位置，退出本次处理
//				//并分配空间
//				if( needX < usableX)
//				{
//					area->cursorX += needX;
//					dealIdx += dealNum;					
//					break;
//				}
//				//如果x轴放不下，新增一行,移动光标，回到A继续判断
//				else
//				{
//					pction->addRow( pction);
//					rowNum ++;
//				}
//			}
//			else
//			//如果y轴放不下，设置需要分页标志，直接退出
//			{
//				pvd->more	= 1;
//				goto exit;
//			}
//	
//		}
//	}
//	
//	exit:
//		pvd->area_y2 += pction->rowSize[ pction->step] * rowNum;
//		pvd->area_x2 = area->cursorX;
//	
//		pvd->dspArea.x2 = area->cursorX;
//		pvd->dspArea.y2 += pction->rowSize[ pction->step] * rowNum;
//		pvd->dspArea.numRow = rowNum;
//		rowNum = 0;
//		return;
//	
//}


//static void ShowChild(void **ppvd, void *cl)
//{
//	ViewData_t *pvd = *ppvd;
//	ViewData_t *pPvd = pvd->paraent;
////	Composition	*ction = ( Composition *)cl;
////	short		x,y;
//	
//	if( SET_CHILDASWHOLE( pPvd->childAttr))
//	{

//		pvd->area_x1 = pPvd->area_x1 + 2;
//		pvd->area_y1 = pPvd->area_y1 + 2;
//		
//		pvd->dspArea.x1 = pPvd->area_x1 + 2;
//		pvd->dspArea.y1 = pPvd->area_y1 + 2;
//		
//		
//	}
//	else
//	{
//		
//		
//	}
//	
//	//设置字体和颜色
//	pvd->gh->setClu( pvd->gh, pvd->colour);
//	pvd->gh->setFont( pvd->gh, pvd->font);
//	pvd->gh->insert( pvd->gh, pvd->data, pvd->len);
//	
//	pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, pvd->len);
//	
//}





CTOR( SimpCtor)
FUNCTION_SETTING( Compositor.vdLayout, Simp_VdLayout);



FUNCTION_SETTING( Compositor.show, Simp_Show);


END_CTOR

