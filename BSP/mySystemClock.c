#include "mySystemClock.h"
#include "stm32f10x_rcc.h"
#include "hardwareConfig.h"



//打开要使用的外设的时钟
void OpenPrpClock( void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC  | RCC_APB2Periph_GPIOD  | RCC_APB2Periph_GPIOE, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);  

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//外部中断使用
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ??DMA1??
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE); // ??DMA1??

	
	
}





