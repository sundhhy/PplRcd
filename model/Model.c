#include "Model.h"
#include "sdhDef.h"

#include <string.h>
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
const Except_T model_Failed = { "MDL Failed" };
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
static void DoUpdate(void **x, void *cl)
{
	Observer *s = ( Observer *)*x;
	s->update(s, cl);
	
}

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//


















void Mdl_attach(  Model *self, Observer *s)
{
	
	self->tObs = List_push( self->tObs, s);
	
	
}

void Mdl_detach(  Model *self, Observer *s)
{
	
	
	
}

int Mdl_getData(  Model *self, IN int aux, void *arg) 
{
	memcpy( self->coreData, arg, self->crDt_len);
	return RET_OK;
}

int Mdl_setData(  Model *self, IN int aux,  void *arg) 
{
	memcpy( arg, self->coreData, self->crDt_len);
	return RET_OK;
}


int	Mdl_addTmMdl( Model *self, Model *m)
{
	self->teamMdl = m;
	return RET_OK;

}	

int Mdl_delTmMdl( Model *self, Model *m)
{
	self->teamMdl = NULL;
	return RET_OK;
}
//int Mdl_installDataSource( Model *self, void *dsr)
//{
//	self->dataSource = dsr;
//	return RET_OK;
//}

void Mdl_notify (Model *self)
{
	
	
	
	List_map(self->tObs, DoUpdate, self);
}



//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

ABS_CTOR( Model)
FUNCTION_SETTING( attach, Mdl_attach);
FUNCTION_SETTING( detach, Mdl_detach);
FUNCTION_SETTING( getMdlData, Mdl_getData);
FUNCTION_SETTING( setMdlData, Mdl_setData);
FUNCTION_SETTING( addTmMdl, Mdl_addTmMdl);
FUNCTION_SETTING( delTmMdl, Mdl_delTmMdl);
//FUNCTION_SETTING( installDataSource, Mdl_installDataSource);
FUNCTION_SETTING( notify, Mdl_notify);

END_ABS_CTOR


