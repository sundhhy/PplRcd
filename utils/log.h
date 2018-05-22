//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef INC_log_H
#define INC_log_H
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define LOG_Factory_Reset						1
#define LOG_LOST_PWR_ERASE						2
#define LOG_CHN_DATA_AUTO_ERASE(n)				(0x10 | n)
#define LOG_CHN_DATA_DROP(n)					(0x20 | n)
#define LOG_CHN_DATA_HANDLE_ERASE(n)			(0x30 | n)
#define LOG_CHN_ALARM_HANDLE_ERASE(n)			(0x40 | n)
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef struct {
	uint32_t		log_time_s;
	uint8_t			log_type;
	uint8_t			log_flag;
	uint8_t			none[2];
}rcd_log_t;
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
int LOG_Init(void);
void LOG_Add(uint8_t	event);
int LOG_Read(char *buf, int buf_size);
int LOG_Set_read_position(int pos);
int	LOG_Get_total_num(void);
int LOG_Get_read_num(void);
#endif
