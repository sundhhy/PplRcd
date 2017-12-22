#ifndef __INC_W25Q_H
#define __INC_W25Q_H
                     // RTOS object definitions

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include "hardwareConfig.h"
#include "system.h"
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------



#define SPI_MASTER_INSTANCE          (0)                 /*! User change define to choose SPI instance */
#define TRANSFER_SIZE               (64)
#define TRANSFER_BAUDRATE           (5000000U)           /*! Transfer baudrate - 1000k 这个波特率下，波形还算可以*/
#define MASTER_TRANSFER_TIMEOUT     (5000U)             /*! Transfer timeout of master - 5s */

#define PAGE_SIZE						256
#define SECTOR_SIZE					4096
#define BLOCK_SIZE					65536
#define SECTOR_HAS_PAGES		16
#define BLOCK_HAS_SECTORS		16


#define    SPIFLASH_CMD_LENGTH        0x03

//erase,program instructions
#define W25Q_INSTR_WR_ENABLE								0x06
#define W25Q_INSTR_ENWR_FOR_VlatlReg				0x50
#define W25Q_INSTR_WR_DISABLE								0x04
#define W25Q_INSTR_RD_Status_Reg1						0x05
#define W25Q_INSTR_RD_Status_Reg2						0x35
#define W25Q_INSTR_WR_Status_Reg						0x01
#define W25Q_INSTR_Page_Program							0x02
#define W25Q_INSTR_Sector_Erase_4K					0x20
#define W25Q_INSTR_BLOCK_Erase_32K					0x52
#define W25Q_INSTR_BLOCK_Erase_64K					0xD8
#define W25Q_INSTR_Chip_Erase_C7						0xC7
#define W25Q_INSTR_Chip_Erase_60						0x60
#define W25Q_INSTR_Erase_Program_Sup				0x75
#define W25Q_INSTR_Erase_Program_Res				0x7A
#define W25Q_INSTR_Page_Down								0xB5
#define W25Q_INSTR_ContinuousRD_Reset				0xff

#define W25Q_INSTR_READ_DATA								0x03


#define W25Q_STATUS1_BUSYBIT								0x01
#define W25Q_STATUS1_WEL									0x02
#define W25Q_STATUS1_BP0									0x04
#define W25Q_STATUS1_BP1									0x08
#define W25Q_STATUS1_BP2									0x10
#define W25Q_STATUS1_TB										0x20
#define W25Q_STATUS1_SEC									0x40
#define W25Q_STATUS1_SRP0									0x80


#define W25Q_STATUS2_SPR1								0x01
#define W25Q_STATUS2_QE									0x02
#define W25Q_STATUS2_RESERVED									0x04
#define W25Q_STATUS2_LB1									0x08
#define W25Q_STATUS2_LB2									0x10
#define W25Q_STATUS2_LB3									0x20
#define W25Q_STATUS2_CMP									0x40
#define W25Q_STATUS2_SUS									0x80
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef struct {
	uint8_t		id[2];
	uint8_t		w25q_flag;
	uint8_t 	none;
	int			page_num;	
	int 		sector_num;
	int 		block_num;
	
	
	
}w25q_instance_t;


typedef fsh_info_t	w25qInfo_t;
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------


int w25q_init(void);


#endif

