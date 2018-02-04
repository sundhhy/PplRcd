#include "Gh_Rectangle.h"
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

static GhRectangle *signalGhRectangle;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void GhRec_vDraw( Glyph *self, dspContent_t *cnt, vArea_t *area);


//static void GhRec_Draw( Glyph *self, dspContent_t *cnt, dspArea_t *area);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

GhRectangle *Get_GhRectangle(void)
{
//	Glyph *gh;
	
	if( signalGhRectangle == NULL)
	{
		signalGhRectangle = GhRectangle_new();
		if(signalGhRectangle  == NULL) while(1);
//		gh = ( Glyph *)signalGhRectangle;

//		gh->setFont( gh, DEF_FONT);
//		gh->setClu( gh, DEF_COLOUR);
		
		
	}
	return signalGhRectangle;
}

CTOR( GhRectangle)
SUPER_CTOR( Glyph);
//FUNCTION_SETTING( Glyph.draw, GhRec_Draw);
FUNCTION_SETTING( Glyph.vdraw, GhRec_vDraw);


END_CTOR


//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{













static void GhRec_vDraw( Glyph *self, dspContent_t *cnt, vArea_t *area)
{
	I_dev_lcd *lcd;
	uint8_t	c = cnt->colour;
	uint8_t	bkc = cnt->bkc;
	uint8_t	grap_x = 0;
	uint8_t	grap_y = 0;
	Dev_open( LCD_DEVID, (void *)&lcd);
	
	if(GP_CKECK_EFF( cnt->effects , EFF_FOCUS))
	{
		c = ColorInvert( c);
		bkc  = ColorInvert( bkc);
	}
	
	if(cnt->subType == GMT_LINE) {
		
		lcd->Box(area->x0, area->y0, area->x1, area->y1, LINE, c);
	}
	else if(cnt->bkc == ERR_COLOUR)
	{
		lcd->Box(area->x0, area->y0, area->x1, area->y1, EMPTY_RECTANGLE, c);
	}
	else if(area->grap || area->offset_x || area->offset_y) {
		lcd->Box(area->x0, area->y0, area->x1, area->y1, EMPTY_RECTANGLE, c);
		if(area->grap) {
			grap_x = area->grap;
			grap_y = area->grap;
		} else {
			grap_x = area->offset_x;
			grap_y = area->offset_y;
			
		}
		lcd->Box(area->x0 + grap_x, area->y0 + grap_y, area->x1 - grap_x, area->y1 - grap_y, FILLED_RECTANGLE, bkc);
	} else {
		lcd->Box(area->x0, area->y0, area->x1, area->y1, FILLED_RECTANGLE, c);
	}
//	else if(cnt->bkc == cnt->colour)
//	{
//		if(area->grap) {
//			lcd->Box(area->x0, area->y0, area->x1, area->y1, EMPTY_RECTANGLE, c);
//			lcd->Box(area->x0 + area->grap, area->y0 + area->grap, area->x1 - area->grap, area->y1 - area->grap, FILLED_RECTANGLE, bkc);
//		} else {
//			lcd->Box(area->x0, area->y0, area->x1, area->y1, FILLED_RECTANGLE, c);
//		}
//	}
//	else
//	{
//		lcd->Box(area->x0, area->y0, area->x1, area->y1, EMPTY_RECTANGLE, c);
//		lcd->Box(area->x0 + 2, area->y0 + 2, area->x1 - 2, area->y1 - 2, FILLED_RECTANGLE, bkc);
//		
//	}


}






//static void GhRec_Draw( Glyph *self, dspContent_t *cnt, dspArea_t *area)
//{
//	I_dev_lcd *lcd;
////	short x2, y2;
////	GhRectangle *cthis = ( GhRectangle *)self;
//	
//	Dev_open( LCD_DEVID, (void *)&lcd);
//	if( cnt->bkc == ERR_COLOUR)
//	{
//		lcd->Box( area->useArea.x1, area->useArea.y1, area->useArea.x2, area->useArea.y2, 0, cnt->colour);
//	}
//	else
//	{
//		lcd->Box( area->useArea.x1, area->useArea.y1, area->useArea.x2, area->useArea.y2, 1, cnt->bkc);
//		
//	}
//	
//}











