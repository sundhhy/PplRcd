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
	
	if(cnt->data == NULL)
		return;
	
	
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
	lcd->wrString(m, cnt->data, cnt->len, area->x0, area->y0, cnt->font,c);
		



}




static int GhTxt_GetSize(Glyph *self, int font, uint16_t *x, uint16_t *y)
{
	I_dev_lcd *lcd;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
	return lcd->getStrSize( font, x, y);
	
}








