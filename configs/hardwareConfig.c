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



Dma_source g_DmaUart1 = {
	DMA1_Channel5,
	DMA1_FLAG_GL5,
	DMA1_Channel5_IRQn,
	
	DMA1_Channel4,
	DMA1_FLAG_GL4,
	DMA1_Channel4_IRQn,
	
};

Dma_source g_DmaUart2 = {
	DMA1_Channel6,
	DMA1_FLAG_GL6,
	DMA1_Channel6_IRQn,
	
	DMA1_Channel7,
	DMA1_FLAG_GL7,
	DMA1_Channel7_IRQn,
	
};

Dma_source g_DmaUart3 = {
	DMA1_Channel3,
	DMA1_FLAG_GL3,
	DMA1_Channel3_IRQn,
	
	DMA1_Channel2,
	DMA1_FLAG_GL2,
	DMA1_Channel2_IRQn,
	
};

Dma_source g_DmaUart4 = {
	DMA2_Channel3,
	DMA2_FLAG_GL3,
	DMA2_Channel3_IRQn,
	
	DMA2_Channel5,
	DMA2_FLAG_GL5,
	DMA2_Channel4_5_IRQn,
	
};



//串口的方向控制引脚，如果不需要的话就设置成0
//gpio_pins Dir_485_pin1 = {0,0};
//gpio_pins Dir_485_pin2 = {0,0};

USART_InitTypeDef g_Cfg_Uart1 = {
		115200,
		USART_WordLength_8b,
		USART_StopBits_1,
		USART_Parity_No,
		USART_Mode_Rx | USART_Mode_Tx,
		USART_HardwareFlowControl_None,
};

USART_InitTypeDef g_Cfg_Uart2 = {
		115200,
		USART_WordLength_8b,
		USART_StopBits_1,
		USART_Parity_No,
		USART_Mode_Rx | USART_Mode_Tx,
		USART_HardwareFlowControl_None,
};

USART_InitTypeDef g_Cfg_Uart3 = {
		19200,
		USART_WordLength_8b,
		USART_StopBits_1,
		USART_Parity_No,
		USART_Mode_Rx | USART_Mode_Tx,
		USART_HardwareFlowControl_None,
};

USART_InitTypeDef g_Cfg_Uart4 = {
		19200,
		USART_WordLength_8b,
		USART_StopBits_1,
		USART_Parity_No,
		USART_Mode_Rx | USART_Mode_Tx,
		USART_HardwareFlowControl_None,
};

CfgUart_t g_confUart1 = {
	&g_Cfg_Uart1,
	&g_DmaUart1,
	0,
	UART_MODE_DMA,
};
CfgUart_t g_confUart2 = {
	
	&g_Cfg_Uart2,
	&g_DmaUart2,
	1,
	UART_MODE_DMA,
};
CfgUart_t g_confUart3 = {
	
	&g_Cfg_Uart3,
	&g_DmaUart3,
	2,
	UART_MODE_DMA,
};


CfgUart_t g_confUart4 = {
	
	&g_Cfg_Uart4,
	&g_DmaUart4,
	3,
	UART_MODE_DMA,
};




iic_conf_t arr_conf_IIC[2] ={
	{400000, 1, 1, 7, 0},
	{400000, 2, 1, 7, 0}
};


spi_conf_t arr_conf_spi[NUM_SPIS] ={
	{
		0,
		0,
		0,
		8,
		
	},
	{
		0,
		3,
		0,
		8,
		
	},
	{
		0,
		0,
		0,
		8,
	}
};








gpio_pins pin_keyRight = {
	GPIO_PORT_KEY_RIGHT,
	GPIO_PIN_KEY_RIGHT,
	GPIO_DIR_IN,
	0,
	GPIO_IRQ_DISABLE,
	GPIO_PORTSOURCE_RIGHT,
	GPIO_PINSOURCE_RIGHT
};

gpio_pins pin_keyLeft = {
	GPIO_PORT_KEY_LEFT,
	GPIO_PIN_KEY_LEFT,
	GPIO_DIR_IN,
	1,
	GPIO_IRQ_DISABLE,
	GPIO_PORTSOURCE_LEFT,
	GPIO_PINSOURCE_LEFT
	
};

gpio_pins pin_keyUp = {
	GPIO_PORT_KEY_UP,
	GPIO_PIN_KEY_UP,
	GPIO_DIR_IN,
	2,
	GPIO_IRQ_DISABLE,
	GPIO_PORTSOURCE_UP,
	GPIO_PINSOURCE_UP
	
};

gpio_pins pin_keyDown = {
	GPIO_PORT_KEY_DOWN,
	GPIO_PIN_KEY_DOWN,
	GPIO_DIR_IN,
	3,
	GPIO_IRQ_DISABLE,
	GPIO_PORTSOURCE_DOWN,
	GPIO_PINSOURCE_DOWN
	
};

gpio_pins pin_keyEnter = {
	GPIO_PORT_KEY_ENTER,
	GPIO_PIN_KEY_ENTER,
	GPIO_DIR_IN,
	4,
	GPIO_IRQ_DISABLE,
	GPIO_PORTSOURCE_ENTER,
	GPIO_PINSOURCE_ENTER
	
};

gpio_pins pin_keyEsc = {
	GPIO_PORT_KEY_ESC,
	GPIO_PIN_KEY_ESC,
	GPIO_DIR_IN,
	5,
	GPIO_IRQ_DISABLE,
	GPIO_PORTSOURCE_ESC,
	GPIO_PINSOURCE_ESC
	
};

gpio_pins pin_UsbInt = {
	GPIO_PORT_USBINT,
	GPIO_PIN_USBINT,
	GPIO_DIR_IN,
	10,
	GPIO_IRQ_FAILING,
	GPIO_PORTSOURCE_USBINT,
	GPIO_PINSOURCE_USBINT
	
};

gpio_pins pin_pwr = {
	PORT_PWR,
	PIN_PWR,
	GPIO_DIR_IN,
	9,
	GPIO_IRQ_FAILING,
	GPIO_PortSourceGPIOD,
	GPIO_PINSOURCE_PWR
	
};


