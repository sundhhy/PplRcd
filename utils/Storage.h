#ifndef __INC_Storage_H
#define __INC_Storage_H
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include "basis/macros.h"
#include "utils/time.h"
#include <stdint.h>
#include "ModelFactory.h"
#include "system.h"

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define CFG_CHN_CONF(n)					(n)



#define CFG_TYPE_SYSTEM						0x10
#define	IS_SYS_CONF(type)					(type == 0x10)
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef struct {
	uint32_t				rcd_maxcount;
	uint32_t				rcd_count;
	
}rcd_mgr_t;

typedef struct
{
	mdl_chn_save_t		mdlchn[NUM_CHANNEL];
	system_conf_t			sys_cfg;
	
	
}conf_file_t;

CLASS(Storage)
{
	IMPLEMENTS( Observer);

	int					rcd_fd;
	int					rcd_mgr_fd;
	rcd_mgr_t		arr_rcd_mgr[NUM_CHANNEL];
	int			(*init)(Storage *self);
	int			(*rd_stored_data)(Storage *self, uint8_t	cfg_type, void *buf);
	int			(*wr_stored_data)(Storage *self, uint8_t	cfg_type, void *buf);
	void		(*shutdown)(Storage *self);
};
//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
Storage		*Get_storage();
#endif
