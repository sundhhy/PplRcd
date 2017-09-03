#include "ModelTest.h"
#include <string.h>
#include <stdlib.h>
#include "mem/CiiMem.h"
#include "sdhDef.h"

//#include "utils/rtc.h"
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


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
/* Cycle/Sync Callback functions */
static int MdlTest_init( Model *self, IN void *arg);
static int MdlTests_getData(  Model *self, IN int aux, void *arg) ;

static char* MdlTest_to_string( Model *self, IN int aux, void *arg);
static int  MdlTest_to_percentage( Model *self, void *arg);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
ModelTest *Get_ModeTest(void)
{
	static ModelTest *signal_modeTest = NULL;
	Model	*p_mdl;
	if( signal_modeTest == NULL) {
		signal_modeTest = ModelTest_new();
		p_mdl = SUPER_PTR( signal_modeTest, Model);
		p_mdl->init( p_mdl, NULL);
	}
	
	return signal_modeTest;
}

CTOR( ModelTest)
SUPER_CTOR( Model);
FUNCTION_SETTING( Model.init, MdlTest_init);
FUNCTION_SETTING( Model.getMdlData, MdlTests_getData);
FUNCTION_SETTING( Model.to_string, MdlTest_to_string);
FUNCTION_SETTING( Model.to_percentage, MdlTest_to_percentage);
END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


static int MdlTest_init( Model *self, IN void *arg)
{
	ModelTest		*cthis = SUB_PTR( self, Model, ModelTest);
	
	cthis->str_buf = CALLOC(1,8);
	cthis->unit_buf = CALLOC(1,8);
	return RET_OK;
}

static int MdlTests_getData(  Model *self, IN int aux, void *arg) 
{
	ModelTest		*cthis = SUB_PTR( self, Model, ModelTest);
	if( aux == 0)
		aux = 1000;
	cthis->i_rand = rand()%aux;
	return cthis->i_rand ;
}

static char* MdlTest_to_string( Model *self, IN int aux, void *arg)
{
	ModelTest		*cthis = SUB_PTR( self, Model, ModelTest);
	if( aux == 0) {
		sprintf( cthis->str_buf, "%d.%d", cthis->i_rand/10, cthis->i_rand%10);
		return cthis->str_buf;
	} else {
		Mdl_unit_to_string( self->uint, cthis->unit_buf, 8);
		return cthis->unit_buf;
	}
	
	
}

static int  MdlTest_to_percentage( Model *self, void *arg)
{
	ModelTest		*cthis = SUB_PTR( self, Model, ModelTest);
	return cthis->i_rand ;
}

