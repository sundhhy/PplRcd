#include "Gh_txt.h"
#include "device.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

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
static GhTxt *signalGhTxt;


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static void GhTxt_vDraw( Glyph *self, dspContent_t *cnt, vArea_t *area);

	/*          170805         @Deprecated   */

//static void GhTxt_Draw( Glyph *self, dspContent_t *cnt, dspArea_t *area);
static int GhTxt_GetSize(Glyph *self, int font, uint16_t *x, uint16_t *y);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

GhTxt *Get_GhTxt(void)
{
	if( signalGhTxt == NULL)
	{
		signalGhTxt = GhTxt_new();
		if(signalGhTxt  == NULL) while(1);
	}
	

	return signalGhTxt;
}

CTOR( GhTxt)
SUPER_CTOR( Glyph);

FUNCTION_SETTING( Glyph.vdraw, GhTxt_vDraw);


//FUNCTION_SETTING( Glyph.draw, GhTxt_Draw);
//FUNCTION_SETTING( Glyph.insert, GhTxt_Insert);
//FUNCTION_SETTING( Glyph.draw, GhTxt_Draw);


//FUNCTION_SETTING( Glyph.flush, GhTxt_Flush);


FUNCTION_SETTING( Glyph.getSize, GhTxt_GetSize);

//FUNCTION_SETTING( Glyph.getWidth, GhTxt_GetWidth);
//FUNCTION_SETTING( Glyph.getHeight, GhTxt_GetHeight);
//FUNCTION_SETTING( Glyph.getNum, GhTxt_GetNum);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{








static void GhTxt_vDraw( Glyph *self, dspContent_t *cnt, vArea_t *area)
{
	I_dev_lcd *lcd;
	char	m = 0xff;
	uint8_t	c = cnt->colour;
	uint8_t	bkc = cnt->bkc;
//	uint8_t	i ;
	
//	uint16_t tmp_x0;
	
	if( GP_CKECK_EFF( cnt->effects , EFF_HIDE))
		return;
	
	if( GP_CKECK_EFF( cnt->effects , EFF_FOCUS)) {
		c = ColorInvert( c);
		bkc  = ColorInvert( c);
	}
	

	Dev_open( LCD_DEVID, (void *)&lcd);

	if( GP_CKECK_EFF( cnt->effects , EFF_BKPIC)) {
		m = cnt->bkc;
		
	} else {
		lcd->BKColor( bkc);
	}
	
//	if( cnt->subType == TEXT_ST_LABLE)
//	{
//		lcd->label( cnt->data, cnt->len,( scArea_t *)area, cnt->font,c, area->alix);
//		
//	}
//	else 
		
//	if( cnt->subType == TEXT_ST_UNTIL)
//	{
//		//需要显示上标的单位
//			//目前只考虑m2/m3这两种上标显示
//			
//		for(i = 0 ; i < cnt->len; i++) {
//			if(cnt->data[i] == 'm')		//因为系统中m之后必定跟着一个上标，所以就简化判断
//				break;
//			
//		}
//		
//		//没有找到m3/m2，就直接显示
//		if(i == cnt->len) {
//			
//			lcd->wrString( m, cnt->data, cnt->len, area->x0, area->y0, cnt->font,c);
//		} else {
//			//显示m3/m2之前的内容(包括m）
//			lcd->wrString( m, cnt->data, i + 1, area->x0, area->y0, cnt->font,c);
////			lcd->wrString( m, cnt->data, i + 1, area->x0, area->y0, FONT_16,c);
//			//显示上标2、3
//				//上标比正常字体小1号
//				//因为gpu不支持12的字体，因此单位显示时应用24号字体，这样上标用16的字体
//				//所以上标就选16字体，正常字体在设计时应该保证是24或以上的，暂时就只考虑24的字体
//			tmp_x0 = area->x0 + i * 12 + 12;
//			lcd->wrString(m, cnt->data + i + 1, 1, tmp_x0, area->y0,FONT_16,c);
//			//显示上标2/3之后的内容
//			tmp_x0 += 6;
//			lcd->wrString(m, cnt->data + i + 2, cnt->len - i - 1, tmp_x0, area->y0,cnt->font,c);
////			lcd->wrString(m, cnt->data + i + 2, cnt->len - i - 1, tmp_x0, area->y0,FONT_16,c);

////			lcd->done();
//		}
//		lcd->label( cnt->data, cnt->len,( scArea_t *)area, cnt->font,c, area->alix);
//		
//	}
//	else
	{
		
		lcd->wrString(m, cnt->data, cnt->len, area->x0, area->y0, cnt->font,c);
		
	}
//	if( cnt->bkc != ERR_COLOUR && cnt->bkc != area->curScInfo->scBkc )
//		lcd->BKColor( area->curScInfo->scBkc);	//将背景色改回屏幕的背景色，避免影响后面要显示的内容


}




static int GhTxt_GetSize(Glyph *self, int font, uint16_t *x, uint16_t *y)
{
	I_dev_lcd *lcd;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
	return lcd->getStrSize( font, x, y);
	
}








