//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "stm32f10x.h"
#include "stm32f10x_pwr.h"
#include "power.h"
#include "system.h"
#include "sdhDef.h"
#include "device.h"
#include "glyph.h"
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


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------



//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int PVD_Init(void)
{
//	FlagStatus	s;
//	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef exti_param;

	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//设置优先级分组形式1，即抢占级占一位，优先级占3位
//	
//	
//	
//		/* PVD 优先级最高 
//		
//		*/
//	NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
//    NVIC_Init(&NVIC_InitStructure);
	
	
	
	 
	
	EXTI_DeInit();
	
	EXTI_StructInit(&exti_param);
	exti_param.EXTI_Line = EXTI_Line16; 
	exti_param.EXTI_Mode = EXTI_Mode_Interrupt; //??????
	exti_param.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //电压从高下降到低于设定阀值时产生中断
	exti_param.EXTI_LineCmd = ENABLE; // ?????
	EXTI_ClearITPendingBit(EXTI_Line16);
	EXTI_Init(&exti_param); // ??
	
	
	PWR_PVDLevelConfig(PWR_PVDLevel_2V9); 
	PWR_PVDCmd(ENABLE);
	
//	s = PWR_GetFlagStatus(PWR_FLAG_WU);
//	s = PWR_GetFlagStatus(PWR_FLAG_SB);
//	s = PWR_GetFlagStatus(PWR_FLAG_PVDO);
	
	return RET_OK;
	
}

void PVD_IRQHandler(void)
{
//	I_dev_lcd 				*tdd_lcd;

//	Dev_open(LCD_DEVID, (void *)&tdd_lcd);
//	tdd_lcd->Clear( COLOUR_YELLOW);
	phn_sys.sys_flag |= SYSFLAG_POWERON;
		
//	System_power_off();
	EXTI_ClearITPendingBit(EXTI_Line16);
	
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
