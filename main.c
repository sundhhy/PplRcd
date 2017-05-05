/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include <stdio.h>
#include <string.h>
#include "stm32f10x_usart.h"

#include "hardwareConfig.h"
#include "mySystemClock.h"
#include "pinmux.h"
#include "intrInit.h"

#include "basis/marco.h"
#include "basis/sdhError.h"
#include "nokia_5110.h"
#include "device.h"

#include "TDD.h"
#include "glyph.h"

//系统版本号
//主板本号自己制定
//系统编译时的月份和日作为子版本
short  g_majorVer = 1;		
short  g_childVer;

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData( DEBUG_USART, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus( DEBUG_USART, USART_FLAG_TC) == RESET)
    {
    }

    return ch;
}

int fgetc(FILE *f /*stream*/)
{
	 /* Loop until rx not empty */
    while (USART_GetFlagStatus( DEBUG_USART, USART_FLAG_RXNE) == RESET)
    {
    }
	
	return USART_ReceiveData( DEBUG_USART);
}


/*
 * main: initialize and start the system
 */

//tdd data
#if TDD_ON == 1
char	appBuf[ 64];
#endif
#if TDD_DEV_UART2 == 1

I_dev_Char *I_uart2;

#endif

int main (void) {
	USART_InitTypeDef USART_InitStructure;
	
  osKernelInitialize ();                    // initialize CMSIS-RTOS

  // initialize peripherals here
	OpenPrpClock();
	Pin_init();
	NVIC_Configuration();
	
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( DEBUG_USART, &USART_InitStructure);
	USART_Cmd( DEBUG_USART, ENABLE);
	g_childVer = GetCompileMoth() << 8 | GetCompileDay();
	printf("\r\n ############("__DATE__ " - " __TIME__ ")############");
	printf("\n sytem ver : %x %x \n", g_majorVer, g_childVer);
	
	LCD_init();
    LCD_clear();
	sprintf( appBuf, "VER : %x %x", g_majorVer, g_childVer);
	LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_LOGO) *6)/2,2, appBuf);
	
	
	
#if TDD_LCD == 1

	View_test();

#endif
	
#if TDD_DEV_UART2 == 1	
	
	
	sprintf( appBuf, "uart 2 testing ...");
	LCD_clear();
	LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_LOGO) *6)/2,1, appBuf);
	
	Dev_open( DEVID_UART2, ( void *)&I_uart2);
	
	if( I_uart2->test(appBuf, 64) == RET_OK)
	{
		sprintf( appBuf, "succeed!");
		
	}
	else 
	{
		sprintf( appBuf, "failed!");
	}
	
	
	
	
	LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_LOGO) *6)/2,23, appBuf);
#endif	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);

  osKernelStart ();                         // start thread execution 
}
