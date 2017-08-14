#ifndef _INC_keyboardHMI_H_
#define _INC_keyboardHMI_H_
#include "HMI.h"
#include "ModelFactory.h"
#include "sheet.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define NUM_VIRKEY		2
 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
CLASS( keyboardHMI)
{
	
	EXTENDS( HMI);
	IMPLEMENTS( shtCmd);
	Model			*p_mdlRtc;
	sheet			*p_shtVkey[NUM_VIRKEY];
	sheet			*p_shtvKeyCursor[NUM_VIRKEY];		//虚拟按键上，被选中的按键的光标
	
	sheet			*p_shtInput;				//输入框，由外部传入
	
	uint8_t		curVkey;
	uint8_t		fouseRow;
	uint8_t		fouseCol;
	uint8_t		none;
	
	
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
keyboardHMI *GetkeyboardHMI(void);
#endif
