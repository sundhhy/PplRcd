/* $Id: H:/drh/idioms/book/RCS/list.doc,v 1.11 1997/02/21 19:46:01 drh Exp $ */
#ifndef INC_crc_INCLUDED
#define INC_crc_INCLUDED
#include <stdint.h>

uint16_t Crc16(uint8_t *puchMsg, uint16_t usDataLen);
uint16_t getCRC_CCITT( uint8_t data[], int len);
#endif
