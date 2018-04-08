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
#define PWR_PIN_CODE		0xbb
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
//static void PWR_pin_irq( void *arg, int type, int encode);


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int PVD_Init(void)
{
//	I_dev_Char		*gpio_pwr;
//	int			pwr_code = PWR_PIN_CODE;
	
	EXTI_InitTypeDef exti_param;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	
//	Dev_open(DEVID_GPIO_PWR, ( void *)&gpio_pwr);
//	gpio_pwr->ioctol(gpio_pwr, DEVCMD_SET_IRQHDL, PWR_pin_irq, (void *)gpio_pwr);
//	gpio_pwr->ioctol(gpio_pwr, DEVGPIOCMD_SET_ENCODE, pwr_code);
//	gpio_pwr->ioctol(gpio_pwr,DEVCMD_ENABLE_IRQ);
	EXTI_DeInit();
	EXTI_StructInit(&exti_param);
	exti_param.EXTI_Line = EXTI_Line16; 
	exti_param.EXTI_Mode = EXTI_Mode_Interrupt; //??????
	exti_param.EXTI_Trigger = EXTI_Trigger_Rising; //电压从高下降到低于设定阀值时产生中断
	exti_param.EXTI_LineCmd = ENABLE; // ?????
	EXTI_ClearITPendingBit(EXTI_Line16);
	EXTI_Init(&exti_param); // ??
	
	PWR_PVDLevelConfig(PWR_PVDLevel_2V9); 
	PWR_PVDCmd(ENABLE);
//	

	
//	s = PWR_GetFlagStatus(PWR_FLAG_WU);
//	s = PWR_GetFlagStatus(PWR_FLAG_SB);
//	s = PWR_GetFlagStatus(PWR_FLAG_PVDO);
	
	return RET_OK;
	
}

//extern void System_power_on(void);
void PVD_IRQHandler(void)
{
//	I_dev_lcd 				*tdd_lcd;

//	Dev_open(LCD_DEVID, (void *)&tdd_lcd);
//	tdd_lcd->Clear( COLOUR_YELLOW);
	
	
//	if(PWR_GetFlagStatus(PWR_FLAG_PVDO) == 0)
//		phn_sys.sys_flag |= SYSFLAG_POWEON;
//	else
		phn_sys.sys_flag |= SYSFLAG_POWEROFF;
	
	
	if(phn_sys.sys_flag & SYSFLAG_POWEON)		//上过电 才认为需要保存，否则可能是假掉电
		System_power_off();
		
	
	EXTI_ClearITPendingBit(EXTI_Line16);
	
//	System_power_on();
	
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

//static void PWR_pin_irq( void *arg, int type, int encode)
//{
//	
//	if(encode == PWR_PIN_CODE)
//	{
//		
//		phn_sys.sys_flag |= SYSFLAG_POWEROFF;
//	}
//	
//}

