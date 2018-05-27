//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//


/*
测试整片flash的读写是否正确

*/
#include "cfg_test.h"
#include "unit_test.h"
#if UNT_W25Q == 1
#include <stdio.h>

#include "system.h"
#include "test_w25q.h"


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
static flash_t					*tdd_fsh;
static int 							row_num;
static char*						my_buf;
static int 							my_buf_size;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------



//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//


int Init_test(char	*test_buf, int size)
{
	
	my_buf = (char *)test_buf;
	my_buf_size = size;
	
	tdd_fsh = &phn_sys.arr_fsh[FSH_W25Q_NUM];
	tdd_fsh->fsh_info(&tdd_fsh->fnf);
	
	
	row_num = 0;
	UNT_Disp_text("W25Q 读写测试, 擦除整片",row_num++, 0);
	
	tdd_fsh->fsh_ersse(FSH_OPT_CHIP, 0);
	
	
	return 0;
}

//整片读写测试：以u32写入,数值从0 ~ flash_size / 4。之后依次读出，并进行比较
void Run_test(void)
{
	uint32_t end;
	uint32_t addr;
	uint32_t tdd_u32;
	uint32_t wr_err = 0;
	uint32_t rd_err = 0;

	int			 ret = 0;
	
	end = tdd_fsh->fnf.total_pagenum * tdd_fsh->fnf.page_size;
	
	sprintf(my_buf, "wirte to: %d", end);
	UNT_Disp_text(my_buf,row_num++, 0);
	
	//以u32进行读写测试，从0开始写入
	tdd_u32 = 0;
//	addr = end;
	for(addr = 0; addr < end; addr += 4)
	{
		//对每个寄存器写入地址一样的值
		ret = tdd_fsh->fsh_raw_write((uint8_t *)&tdd_u32, addr, 4);
		if(ret != 4)
		{
			sprintf(my_buf, "WRE %d", addr);
			UNT_Disp_text(my_buf,row_num, 120);
			wr_err ++;
			
		}
		 
			
		if(addr % 10000 == 0)
		{
			sprintf(my_buf, "wirte %.2f", (addr * 100.0 / end));
			UNT_Disp_text(my_buf,row_num, 0);
			
		}
		
		tdd_u32 ++;
	}
	
	
	row_num++;
	sprintf(my_buf, "last wr val %d", tdd_u32);
	UNT_Disp_text(my_buf,row_num++, 0);
	
	
	
	for(addr = 0; addr < end; addr += 4)
	{
		tdd_u32 = 0xffffffff;

		ret = tdd_fsh->fsh_raw_read((uint8_t *)&tdd_u32, addr, 4);
		if(ret != 4)
		{
			sprintf(my_buf, "RDE %d", addr);
			UNT_Disp_text(my_buf,row_num, 0);
			rd_err ++;
			
		}
		else if(tdd_u32 != addr / 4)
		{
			sprintf(my_buf, "CKE %d", addr);
			UNT_Disp_text(my_buf,row_num, 120);
			rd_err ++;
			
		}
		 
		if(addr % 100000 == 0)
		{
			sprintf(my_buf, "read %d", tdd_u32);
			UNT_Disp_text(my_buf, row_num, 0);
			
		}
		
		tdd_u32 ++;
	}
	
	
	row_num++;

	sprintf(my_buf, "rd_err=%d, wr_err=%d", rd_err, wr_err);
	UNT_Disp_text(my_buf, row_num, 0);
	

	
}







//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{



#endif

