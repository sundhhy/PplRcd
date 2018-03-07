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
		if(signal_modeTest == NULL)
			while(1);
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
	cthis->alarm_buf = CALLOC(1,8);
	cthis->range = 100;
	self->mdl_id = MDLID_TEST;
	
	cthis->i_rand = rand()%cthis->range;

	
	return RET_OK;
}

static int MdlTests_getData(  Model *self, IN int aux, void *arg) 
{
	ModelTest		*cthis = SUB_PTR( self, Model, ModelTest);
	
	if( arg) {
		
		cthis->range = *(int *)arg;
	}
	
	switch(aux) {
		case AUX_PERCENTAGE:
			cthis->i_rand = rand()%cthis->range;
			return self->to_percentage(self, arg); 
		default:
			break;
	}
		
	cthis->i_rand = rand()%cthis->range;
	self->notify(self);
	return cthis->i_rand ;
	
}

static char* MdlTest_to_string( Model *self, IN int aux, void *arg)
{
	ModelTest		*cthis = SUB_PTR( self, Model, ModelTest);
	char		*p = NULL;
	int 			hh = cthis->range * 80 / 100;
	int 			hi = cthis->range * 60 / 100;
	int 			li = cthis->range * 40 / 100;
	int 			ll = cthis->range * 20 / 100;
	
	
	
	switch(aux) {
		case AUX_DATA:
			if( arg) {
				p = (char *)arg;		
			} else {
				p = cthis->str_buf;
			}
//			if(cthis->range == 1000)
//				sprintf( p, "%d.%d", cthis->i_rand/10, cthis->i_rand%10);
//			else
			sprintf( p, "%3d", cthis->i_rand);
			return p;
		case AUX_UNIT:
			if( arg) {
				p = (char *)arg;		
			} else {
				p = cthis->unit_buf;
			}
			Print_unit( self->uint, p, 8);
			return p;
		case AUX_ALARM:
			if( arg) {
				p = (char *)arg;		
			} else {
				p = cthis->alarm_buf;
			}
		
			memset(p, 0, 8);
			if(cthis->i_rand > hh) {
				strcat(p, "HH ");
			}
			
			if(cthis->i_rand > hi) {
				strcat(p, "Hi ");
			}
			
			if(cthis->i_rand < li) {
				strcat(p, "Li ");
			}
			
			if(cthis->i_rand < ll) {
				strcat(p, "LL ");
			}
			
			//把空间填满，来达到清除掉上一次的显示的目的
			if( strlen( p) < 4) {
				strcat(p, "   ");
			}
			return p;
		case AUX_PERCENTAGE:
			if( arg) {
				p = (char *)arg;		
			} else {
				p = cthis->str_buf;
			}
			if(cthis->range == 1000)
				sprintf( p, "%d.%d", cthis->i_rand/10, cthis->i_rand%10);
			else
				sprintf( p, "%d", cthis->i_rand);
			return p;
		default:
			break;
			
		
	}
	return NULL;
}

static int  MdlTest_to_percentage( Model *self, void *arg)
{
	ModelTest		*cthis = SUB_PTR( self, Model, ModelTest);
	
	
	
	return cthis->i_rand ;
}

