#include "Gh_txt.h"
#include "device.h"

//#define GHTXTBUFLEN		64


static GhTxt *signalGhTxt;


GhTxt *Get_GhTxt(void)
{
	Glyph *gh;
	
	if( signalGhTxt == NULL)
	{
		signalGhTxt = GhTxt_new();
//		gh = ( Glyph *)signalGhTxt;
//		signalGhTxt->buf = malloc( GHTXTBUFLEN);
//		signalGhTxt->cq_buf = malloc( GHTXTBUFLEN);
//		gh->setFont( gh, DEF_FONT);
//		gh->setClu( gh, DEF_COLOUR);
//		CQ_Init( &signalGhTxt->cq, signalGhTxt->cq_buf, GHTXTBUFLEN);
		
	}
	

	return signalGhTxt;
}









//static int GhTxt_Clean( Glyph *self)
//{
//	GhTxt *cthis = ( GhTxt *)self;
//	return CQ_Clean( &cthis->cq);
//	
//}

//static void GhTxt_Insert( Glyph *self, void *context, int len)
//{
//	GhTxt *cthis = ( GhTxt *)self;
//	CQ_Write( &cthis->cq, context, len);
//	
//}

//static void GhTxt_Draw( Glyph *self, int x, int y, int len)
//{
////	I_dev_lcd *lcd;
////	
////	GhTxt *cthis = ( GhTxt *)self;
////	char *p = cthis->buf;
////	int txtLen = 0;
////	

////	txtLen = CQ_Read( &cthis->cq, p, len);
//////	txtLen = CQ_GetPtr( &cthis->cq, &p, len);
////	if( txtLen == 0)
////		return;
////	
////	Dev_open( LCD_DEVID, (void *)&lcd);
////	
//////	lcd->Clear();
////	lcd->wrString( p, txtLen, x, y, self->font, self->colour);
//	
//}




//在跨行显示的时候，为了防止一个汉字被拆分显示，所以回退一格
//计算这一行能显示的数量
static uint8_t CalNumDisplayInthisRow(dspContent_t *cnt, dspArea_t *area, uint8_t rouNum)
{
	uint8_t 	numInrow = 0;
	short 	displayed = 0;
	short 	xend = 0 ;
	short	xstart	= 0, ableNum;
	char	c;

	//在第一行，x轴起始位置是分配区域的x轴起始位置
	//在新的一行的时候，就是
//	if( rouNum == 0)
//		xstart = area->x1;
//	else
//		xstart = area->curScInfo->xLimit;
//	//在最后一行，x轴的结束位置是分配区域的结束位置
//	if( rouNum == ( area->numRow - 1))
//		xend = area->x2;
//	else
//		xend = area->curScInfo->yLimit;
	
	ableNum = ( xend - xstart) / area->sizeX;
	
	//计算这一行能显示几个字符
	numInrow  = 0;
	displayed = 0;
	while(1)
	{
		c = cnt->data[ displayed];
		
		if( IS_CHINESE( c))
		{
			if( ableNum > 1)
			{
				numInrow += 2;
				displayed += 2;
				ableNum -= 2;
			}
		}
		else
		{
			if( ableNum >  0)
			{
				numInrow ++;
				displayed ++;
				ableNum -= 1;
			}	
			
		}
		//如果显示够了就退出
		if( displayed >= cnt->len)
			break;
		//如果本行已经没有空间了，就显示本行
		if( ableNum == 0)
			break;
		//如果本行还能显示一个字符，看看下一个是不是能放进去了
		if( ableNum == 1)
		{		
			c = cnt->data[ displayed];
			if( !IS_CHINESE( c))
			{
				displayed ++;
				numInrow ++;
			}
			break;
		}
			
	}
		
	return numInrow;

}

static void GhTxt_Draw( Glyph *self, dspContent_t *cnt, dspArea_t *area)
{
	I_dev_lcd *lcd;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
//	if( cnt->bkc != ERR_COLOUR && cnt->bkc != area->curScInfo->scBkc )
		lcd->BKColor( cnt->bkc);
	
	if( cnt->subType == TEXT_ST_LABLE)
	{
		lcd->label( cnt->data, cnt->len,&area->useArea, cnt->font,cnt->colour, area->ali);
		
	}
	else
	{
		lcd->wrString( cnt->data, cnt->len, area->useArea.x1, area->useArea.y1, cnt->font,cnt->colour);
		
	}
	if( cnt->bkc != ERR_COLOUR && cnt->bkc != area->curScInfo->scBkc )
		lcd->BKColor( area->curScInfo->scBkc);	//将背景色改回屏幕的背景色，避免影响后面要显示的内容
//	I_dev_lcd *lcd;
//	char		*oldp = cnt->data;
//	int16_t		oldx1 = area->x1;
//	int16_t		oldy1 = area->y1;
//	uint16_t	oldlen = cnt->len;
//	
//	uint8_t		row = 0;
//	uint8_t		num = 0;
//	int16_t		offset = 0;
//	
//	Dev_open( LCD_DEVID, (void *)&lcd);
//	
//	//设置行和列
//	if( area->curScInfo->rowSize < area->sizeY)
//		area->curScInfo->rowSize = area->sizeY;
//	if( area->curScInfo->colSize < area->sizeY)
//		area->curScInfo->colSize = area->sizeY;
//	
//	
//	
//	//一行行显示
//	//计算每行能显示的字符数
//	offset = 0;
//	for( row = 0; row < area->numRow; row++)
//	{
//		cnt->data += offset;
//		
//		num = CalNumDisplayInthisRow( cnt, area, row);
//		
//		//设置背景色
//		
//		lcd->BKColor( cnt->bgC);
//		//显示字符
//		
//		lcd->wrString( cnt->data, num, area->x1, area->y1, cnt->font, cnt->colour);
//		
//		offset += num;
//		
//		area->x1 = area->curScInfo->xLimit;
//		area->y1 += area->curScInfo->rowSize;
//		
//		
//	}
//	
//	
//	cnt->data =	oldp;
//	area->x1 =	oldx1;
//	area->y1 =	oldy1;
//	cnt->len = 	oldlen;
	

}

//static void GhTxt_Flush( Glyph *self, int x, int y)
//{
//	I_dev_lcd *lcd;
//	
//	GhTxt *cthis = ( GhTxt *)self;
//	char *p;
//	int txtLen = CQ_GetPtr( &cthis->cq, &p, CQ_LENALL);
//	
//	Dev_open( LCD_DEVID, (void *)&lcd);
	
//	lcd->wrString( p, txtLen, x, y, self->font, self->colour);
	
//}
static int GhTxt_GetSize(Glyph *self, int font, uint16_t *x, uint16_t *y)
{
	I_dev_lcd *lcd;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
	return lcd->getStrSize( font, x, y);
	
}
static int GhTxt_GetWidth(Glyph *self)
{
//	GhTxt *cthis = ( GhTxt *)self;
	uint16_t	width = 0;
	I_dev_lcd *lcd;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
//	lcd->getStrSize( self->font, &width, NULL);
	
	return width;
	
}
//static int GhTxt_GetHeight(Glyph *self)
//{
//	GhTxt *cthis = ( GhTxt *)self;
//	uint16_t	height = 0;
//	I_dev_lcd *lcd;
//	
//	Dev_open( LCD_DEVID, (void *)&lcd);
//	
//	lcd->getStrSize( self->font, NULL, &height);
//	
//	return height;
//}

//static int GhTxt_GetNum(Glyph *self)
//{
//	GhTxt *cthis = ( GhTxt *)self;
//	
//	int txtLen = CQ_Len( &cthis->cq);
//	
//	
//	
//	return txtLen;
//}






CTOR( GhTxt)
SUPER_CTOR( Glyph);

FUNCTION_SETTING( Glyph.draw, GhTxt_Draw);
//FUNCTION_SETTING( Glyph.insert, GhTxt_Insert);
//FUNCTION_SETTING( Glyph.draw, GhTxt_Draw);


//FUNCTION_SETTING( Glyph.flush, GhTxt_Flush);


FUNCTION_SETTING( Glyph.getSize, GhTxt_GetSize);

//FUNCTION_SETTING( Glyph.getWidth, GhTxt_GetWidth);
//FUNCTION_SETTING( Glyph.getHeight, GhTxt_GetHeight);
//FUNCTION_SETTING( Glyph.getNum, GhTxt_GetNum);

END_CTOR
