#ifndef __INC_Modle_H_
#define __INC_Modle_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
#include "arithmetic/list.h"
#include "basis/macros.h"
#include "lw_oopc.h"

#include "sdhDef.h"
#ifdef NO_ASSERT
#include "basis/assert.h"
#else
#include "assert.h"
#endif
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
INTERFACE( Observer)
{
	int ( *update)( Observer *self);
	
	
};

ABS_CLASS( Model)
{
	List_T tObs;
	void	*coreData;
	void	*dataSource;
	Model	*teamMdl;
	short	crDt_len;
	char	none[2];
	
	abstract int (*init)( Model *self, IN void *arg);
	
	void (*attach)( Model *self, IN Observer *s);
	void (*detach)( Model *self, IN Observer *s);
	int	(*getMdlData)( Model *self, IN int aux, OUT void *arg);
	int	(*setMdlData)( Model *self, IN int aux, IN void *arg);
	int	(*addTmMdl)( Model *self, Model *m);
	int (*delTmMdl)( Model *self, Model *m);
	
//	int (*installDataSource)( Model *self, void *dsr);
	
	void (*notify)( Model *self);
	
};

//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern const Except_T model_Failed;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------







#endif
