#include "Model.h"
#include "sys_cmd.h"



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


//static void DoUpdate(void **x, void *cl);
//在模型数据更新失败的时候，注册到定时任务中执行，用于重新显示
static void MDL_Retry(void *arg);	
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//


















int Mdl_attach(  Model *self, mdl_observer *s)
{
	
//	self->tObs = List_push( self->tObs, s);
	int i = 0;
	while(i < MDL_OBS_NUM)
	{
		if(self->arr_mdl_obs[i] == NULL)
		{
			
			self->arr_mdl_obs[i] = s;
			return i;
		}
		
		i ++;
		
	}
		
	return -1;
	
}

void Mdl_detach(  Model *self, int fd)
{
	
	if(fd < MDL_OBS_NUM)
	{
		
		self->arr_mdl_obs[fd] = NULL;
	}
	
}

void Mdl_notify (Model *self)
{
	
	int i = 0;
	
	
	for(i = 0; i < MDL_OBS_NUM; i++)
	{
		if(self->arr_mdl_obs[i] == NULL)
			continue;
		if(self->arr_mdl_obs[i]->update(self->arr_mdl_obs[i], self) != RET_OK)
		{
//			self->retry_time_tsk_fd =  Cmd_Rgt_time_task(MDL_Retry, self, 3);
			Cmd_Rgt_time_task(MDL_Retry, self, 2);
			
		}
		
	}
	
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

//static void DoUpdate(void **x, void *cl)
//{
//	mdl_observer *s = ( mdl_observer *)*x;
//	s->update(s, cl);
//	
//}

static void MDL_Retry(void *arg)
{
	
	
	Model		*p_mdl = (Model *)arg;

	
	
	p_mdl->notify(p_mdl);
	
	
}

