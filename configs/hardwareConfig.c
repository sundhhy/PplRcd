/**
* @file 		hardwareConfig.c
* @brief		系统的硬件配置
* @details		1. gprs开机引脚配置
* @author		sundh
* @date		16-09-15
* @version	A001
* @par Copyright (c): 
* 		XXX公司
* @par History:         
*	version: author, date, desc
*	A001:sundh,16-09-15,gprs的开机引脚配置
*/
//注意：SIM800A开发板的：HSE_VALUE=12000000 在配置部分增加宏来实现配置的
#include "hardwareConfig.h"
#include "sdhDef.h"
#include "stm32f10x_exti.h"
//*dma_rx_base;
//dma_rx_flag;
//dma_rx_irq;
//	
//*dma_tx_base;
//dma_tx_flag;
//dma_tx_irq;
Dma_source g_DmaUart2 = {
	DMA1_Channel6,
	DMA1_FLAG_GL6,
	DMA1_Channel6_IRQn,
	
	DMA1_Channel7,
	DMA1_FLAG_GL7,
	DMA1_Channel7_IRQn,
	
};



USART_InitTypeDef g_Cfg_Uart2 = {
		115200,
		USART_WordLength_8b,
		USART_StopBits_1,
		USART_Parity_No,
		USART_Mode_Rx | USART_Mode_Tx,
		USART_HardwareFlowControl_None,
};




CfgUart_t g_confUart2 = {
	1,
	&g_Cfg_Uart2,
	&g_DmaUart2
};

Dma_source g_DmaUart1 = {
	DMA1_Channel5,
	DMA1_FLAG_GL5,
	DMA1_Channel5_IRQn,
	
	DMA1_Channel4,
	DMA1_FLAG_GL4,
	DMA1_Channel4_IRQn,
	
};

//串口的方向控制引脚，如果不需要的话就设置成0
gpio_pins Dir_485_pin1 = {0,0};
gpio_pins Dir_485_pin2 = {0,0};

USART_InitTypeDef g_Cfg_Uart1 = {
		115200,
		USART_WordLength_8b,
		USART_StopBits_1,
		USART_Parity_No,
		USART_Mode_Rx | USART_Mode_Tx,
		USART_HardwareFlowControl_None,
};

CfgUart_t g_confUart1 = {
	0,
	&g_Cfg_Uart1,
	&g_DmaUart1
};




gpio_pins pin_keyRight = {
	GPIO_PORT_KEY_RIGHT,
	GPIO_PIN_KEY_RIGHT,
	0,
	0,
	GPIO_IRQ_BOTHEDGE
};

gpio_pins pin_keyLeft = {
	GPIO_PORT_KEY_LEFT,
	GPIO_PIN_KEY_LEFT,
	0,
	1,
	GPIO_IRQ_BOTHEDGE
	
};

gpio_pins pin_keyUp = {
	GPIO_PORT_KEY_UP,
	GPIO_PIN_KEY_UP,
	0,
	2,
	GPIO_IRQ_BOTHEDGE
	
};

gpio_pins pin_keyDown = {
	GPIO_PORT_KEY_DOWN,
	GPIO_PIN_KEY_DOWN,
	0,
	3,
	GPIO_IRQ_BOTHEDGE
	
};

gpio_pins pin_keyEnter = {
	GPIO_PORT_KEY_ENTER,
	GPIO_PIN_KEY_ENTER,
	0,
	4,
	GPIO_IRQ_BOTHEDGE
	
};

gpio_pins pin_keyEsc = {
	GPIO_PORT_KEY_ESC,
	GPIO_PIN_KEY_ESC,
	0,
	5,
	GPIO_IRQ_BOTHEDGE
	
};



