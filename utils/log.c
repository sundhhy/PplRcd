//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include <stdint.h>
#include <string.h>

#include "os/os_depend.h"
#include "sdhDef.h"


#include "Storage.h"
#include "log.h"
#include "system.h"
#include "time_func.h"

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
struct {
	sem_t		log_sem;
	uint16_t	num_log;
	uint16_t	max_num;
	
	uint16_t	cur_read_num;
	
}log_mgr_t;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int LOG_Read_flash(uint32_t addr, void *buf, int len);
static int LOG_Write_flash(uint32_t addr, void *buf, int len);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int LOG_Init(void)
{
	
	
	log_mgr_t.log_sem = Alloc_sem();
	Sem_init(&log_mgr_t.log_sem);
	Sem_post(&log_mgr_t.log_sem);

	
	LOG_Read_flash(0, &log_mgr_t.num_log, sizeof(log_mgr_t.num_log));
	if(log_mgr_t.num_log == 0xffff)
	{
		log_mgr_t.num_log = 0;
		LOG_Write_flash(0, &log_mgr_t.num_log, sizeof(log_mgr_t.num_log));
	}
	log_mgr_t.max_num = STG_LOG_FILE_SIZE / sizeof(rcd_log_t);
	log_mgr_t.cur_read_num = 0;
	return RET_OK;
}

void LOG_Add(uint8_t	event)
{
	rcd_log_t log;
	
	log.log_time_s = SYS_time_sec();
	log.log_type = event;
	log.log_flag = 1;
	LOG_Write_flash(log_mgr_t.num_log * sizeof(rcd_log_t) + 2, &log, sizeof(log));
	log_mgr_t.num_log ++;
	
	if(log_mgr_t.num_log == log_mgr_t.max_num)
		log_mgr_t.num_log = 0;
	LOG_Write_flash(0, &log_mgr_t.num_log, sizeof(log_mgr_t.num_log));
}

int LOG_Read(char *buf, int buf_size)
{
	
	rcd_log_t 			log;
	int					read_byte = 0;
	int					total_byte = 0;
	struct tm			t;
	char				tmp_buf[32];
	
	while(1)
	{
		
		LOG_Read_flash(log_mgr_t.cur_read_num * sizeof(rcd_log_t) + 2, &log, sizeof(log));
		if(log.log_flag == 0xff)
			break;
		Sec_2_tm(log.log_time_s, &t);
		sprintf(tmp_buf, "%2d/%02d/%02d,%02d:%02d:%02d,%x\r\n", \
			t.tm_year,t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, log.log_type);
		read_byte = strlen(tmp_buf);
		if((total_byte + read_byte) > buf_size)
			break;
		
		memcpy(buf + total_byte, tmp_buf, read_byte);
		log_mgr_t.cur_read_num ++;
		total_byte += read_byte;
		
		if(log_mgr_t.cur_read_num == log_mgr_t.max_num)
			log_mgr_t.cur_read_num = 0;
	}
	
	
	return total_byte;
	
}

int LOG_Set_read_position(int pos)
{
	if(pos > log_mgr_t.max_num)
		return ERR_OUT_OF_RANGE;
	log_mgr_t.cur_read_num = pos;
	return RET_OK;
}

int	LOG_Get_total_num(void)
{
	
	return log_mgr_t.num_log;
}

int LOG_Get_read_num(void)
{
	return log_mgr_t.cur_read_num;
	
}


//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static int LOG_Read_flash(uint32_t addr, void *buf, int len)
{
	Storage						*stg = Get_storage();
	STG_Set_file_position(STG_LOG, STG_DRC_READ, addr);
	return stg->rd_stored_data(stg, STG_LOG, buf, len);
}
static int LOG_Write_flash(uint32_t addr, void *buf, int len)
{
	Storage						*stg = Get_storage();
	STG_Set_file_position(STG_LOG, STG_DRC_WRITE, addr);
	return stg->wr_stored_data(stg, STG_LOG, buf, len);
}


