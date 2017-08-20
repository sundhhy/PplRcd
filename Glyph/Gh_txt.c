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

static void GhTxt_Draw( Glyph *self, dspContent_t *cnt, dspArea_t *area);
static int GhTxt_GetSize(Glyph *self, int font, uint16_t *x, uint16_t *y);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

GhTxt *Get_GhTxt(void)
{
	if( signalGhTxt == NULL)
	{
		signalGhTxt = GhTxt_new();
		
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
	uint8_t	c = cnt->colour;
	if( GP_CKECK_EFF( cnt->effects , EFF_HIDE))
		return;
	
	if( GP_CKECK_EFF( cnt->effects , EFF_FOCUS))
		c = ColorInvert( c);
	
	Dev_open( LCD_DEVID, (void *)&lcd);
//	if( cnt->bkc != ERR_COLOUR  )
//		lcd->Box( area->x0, area->y0, area->x1, area->y1, 1, cnt->bkc);
		lcd->BKColor( cnt->bkc);
	
	if( cnt->subType == TEXT_ST_LABLE)
	{
		lcd->label( cnt->data, cnt->len,( scArea_t *)area, cnt->font,cnt->colour, area->alix);
		
	}
	else
	{
		lcd->wrString( cnt->data, cnt->len, area->x0, area->y0, cnt->font,cnt->colour);
		
	}
//	if( cnt->bkc != ERR_COLOUR && cnt->bkc != area->curScInfo->scBkc )
//		lcd->BKColor( area->curScInfo->scBkc);	//将背景色改回屏幕的背景色，避免影响后面要显示的内容


}



//static void GhTxt_Draw( Glyph *self, dspContent_t *cnt, dspArea_t *area)
//{
//	I_dev_lcd *lcd;
//	
//	Dev_open( LCD_DEVID, (void *)&lcd);
//	
//	lcd->BKColor( cnt->bkc);
//	
//	if( cnt->subType == TEXT_ST_LABLE)
//	{
//		lcd->label( cnt->data, cnt->len,&area->useArea, cnt->font,cnt->colour, area->ali);
//		
//	}
//	else
//	{
//		lcd->wrString( cnt->data, cnt->len, area->useArea.x1, area->useArea.y1, cnt->font,cnt->colour);
//		
//	}
//	if( cnt->bkc != ERR_COLOUR && cnt->bkc != area->curScInfo->scBkc )
//		lcd->BKColor( area->curScInfo->scBkc);	//将背景色改回屏幕的背景色，避免影响后面要显示的内容


//}


static int GhTxt_GetSize(Glyph *self, int font, uint16_t *x, uint16_t *y)
{
	I_dev_lcd *lcd;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
	return lcd->getStrSize( font, x, y);
	
}








