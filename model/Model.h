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
//aux : 0 ~ 0x10  公用的功能
//0x20 ~ 0x40 :给通道用的 其他不要使用
#define AUX_DATA			0
#define AUX_UNIT			1
#define AUX_ALARM			2
#define AUX_PERCENTAGE		3	
#define AUX_SIGNALTYPE		4	


#define MDLID_TEST			0
#define MDLID_CHN(n)				(8 + n)			// 8 ~ 13
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
INTERFACE( Observer)
{
	int ( *update)( Observer *self, void *p_srcMdl);
	
	
};

ABS_CLASS( Model)
{
	List_T tObs;
	void	*coreData;
	void	*dataSource;
	Model	*teamMdl;
	short	crDt_len;
	char	uint;
	uint8_t	mdl_id;
	
	abstract int (*init)( Model *self, IN void *arg);
	
	void (*attach)( Model *self, IN Observer *s);
	void (*detach)( Model *self, IN Observer *s);
	int	(*getMdlData)( Model *self, IN int aux, OUT void *arg);
	int	(*setMdlData)( Model *self, IN int aux, IN void *arg);
	int	(*addTmMdl)( Model *self, Model *m);
	int (*delTmMdl)( Model *self, Model *m);
	int (*set_by_string)( Model *self,IN int aux, void *arg);
//	int (*installDataSource)( Model *self, void *dsr);
	
	void (*notify)( Model *self);
	
	char *(*to_string)( Model *self,IN int aux, void *arg);		//数值转换成字符串
	int  (*to_percentage)( Model *self, void *arg);		//数值转换成百分比
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
extern const	char	g_moth_day[12];
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------



void Mdl_unit_to_string( char unit, char *buf, int len);



#endif
