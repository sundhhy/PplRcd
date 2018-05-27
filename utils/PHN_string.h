//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_PHN_string_H
#define __INC_PHN_string_H
#include <stdint.h>
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern const int	arr_baud[7] ;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//system.c
void Print_sys_param(void *p_data, char	*p_s, int len, int aux);
void Str_set_sys_param(void *p, char	*p_s, int aux, int op, int val);

int Get_str_data(char *s_data, char* separator, int num, uint8_t	*err);

int Str_Password_match(char *p_s_psd, uint8_t *psd);
void Password_modify(char	*p_s_psd, int idx, int op);
void Str_set_password(char	*p_s_psd, uint8_t *psd);
int Password_iteartor(char	*p_time_text, int idx, int director);
void Clone_psd(uint8_t *src, uint8_t *dst);
//model.c
void Print_unit( char unit, char *buf, int len);
void Print_float(int data, short int_len, short prec, char *str);
void Print_frefix_float(int data, int prec, char *prefix, char *str);

//mode_channel
void Print_singnaltype(int sgt, char *str);
void Print_touch_spot(int spot, char *str);

//ModelTime
extern int MdlTime_text_iteartor(char	*p_time_text, int idx, int director);
extern void MdlTime_text_modify(char	*p_time_text, int idx, int op);

//HMI_comm.c
void Str_Calculations(char *p_str, int len,  int op, int val, int rangel, int rangeh);


#endif
