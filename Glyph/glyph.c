#include <stdint.h>
#include "dev_LCD/UsartGpu/dev_LcdUsartGpu.h"
#include "testGlyph.h"
#include "Glyph.h"
void View_init(void)
{
	Dev_UsartInit();
	
}

void View_test(void)
{
	TestViewShow();
	
}

ABS_CTOR( Glyph)

END_ABS_CTOR



