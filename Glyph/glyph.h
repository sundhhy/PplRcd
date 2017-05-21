#ifndef __GLYPH_H_
#define __GLYPH_H_
#include "lw_oopc.h"
#include <stdint.h>
#include "device.h"

#define LCD_DEVID		DEVID_USARTGPULCD

ABS_CLASS( Glyph)
{
	
	uint16_t	font;
	uint16_t	colour;
	I_dev_lcd	*myLcd ;
	int (*init)( Glyph *self, I_dev_lcd *lcd);
	int (*setFont)( Glyph *self, int font);
	int (*setClu)( Glyph *self, int c);
	//abs func
	//
	abstract int ( *getSize)(Glyph *self, int font, uint16_t *size_x, uint16_t *size_y);
	
	//---------------------------------------
	abstract int (*clean)( Glyph *self);
	abstract void (*insert)( Glyph *self, void *context, int len);
	abstract void (*draw)( Glyph *self, int x, int y, int len);
	abstract void (*flush)( Glyph *self, int x, int y);
	abstract int ( *getWidth)(Glyph *self);
	abstract int ( *getHeight)(Glyph *self);
	abstract int ( *setWidth)(Glyph *self, uint16_t wd);
	abstract int ( *setHeight)(Glyph *self, uint16_t he);
	abstract int ( *getNum)(Glyph *self);
	
	
};
void View_test(void);
#endif
