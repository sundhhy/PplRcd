#ifndef __HARDWARECONFIG_H__
#define __HARDWARECONFIG_H__
#include "stm32f10x_conf.h"

#define	DEBUG_COM			2
#if DEBUG_COM == 1
#define DEBUG_USART	USART1
#elif DEBUG_COM == 2
#define DEBUG_USART	USART2
#elif DEBUG_COM == 3
#define DEBUG_USART	USART3
#endif

#define NUM_SPIS        2

//各种外设的引脚配置
#define RCC_UART1_TX                                    RCC_APB2Periph_GPIOB		 
#define GPIO_PORT_UART1TX                               GPIOB    					 
#define GPIO_PIN_UART1TX                                GPIO_Pin_6					 

#define RCC_UART1_RX                                    RCC_APB2Periph_GPIOB		 
#define GPIO_PORT_UART1RX                               GPIOB    					 
#define GPIO_PIN_UART1RX                                GPIO_Pin_7	

#define RCC_UART2_TX                                    RCC_APB2Periph_GPIOA		 
#define GPIO_PORT_UART2TX                               GPIOA    					 
#define GPIO_PIN_UART2TX                                GPIO_Pin_2					 

#define RCC_UART2_RX                                    RCC_APB2Periph_GPIOA		 
#define GPIO_PORT_UART2RX                               GPIOA    					 
#define GPIO_PIN_UART2RX                                GPIO_Pin_3

#define RCC_UART3_TX                                    RCC_APB2Periph_GPIOB		 
#define GPIO_PORT_UART3TX                               GPIOB    					 
#define GPIO_PIN_UART3TX                                GPIO_Pin_10					 

#define RCC_UART3_RX                                    RCC_APB2Periph_GPIOB		 
#define GPIO_PORT_UART3RX                               GPIOB    					 
#define GPIO_PIN_UART3RX                                GPIO_Pin_11

//*********keyboard	********************************
#define RCC_KEY_RIGHT                                    RCC_APB2Periph_GPIOD		 
#define GPIO_PORT_KEY_RIGHT                              GPIOD    					 
#define GPIO_PIN_KEY_RIGHT                               GPIO_Pin_0
#define GPIO_PORTSOURCE_RIGHT                              GPIO_PortSourceGPIOD    					 
#define GPIO_PINSOURCE_RIGHT                               GPIO_PinSource0

#define RCC_KEY_LEFT                                  RCC_APB2Periph_GPIOD		 
#define GPIO_PORT_KEY_LEFT                             GPIOD    					 
#define GPIO_PIN_KEY_LEFT                               GPIO_Pin_1
#define GPIO_PORTSOURCE_LEFT                               GPIO_PortSourceGPIOD    					 
#define GPIO_PINSOURCE_LEFT                                GPIO_PinSource1

#define RCC_KEY_UP                                    RCC_APB2Periph_GPIOD	
#define GPIO_PORT_KEY_UP                              GPIOD    					 
#define GPIO_PIN_KEY_UP                               GPIO_Pin_2
#define GPIO_PORTSOURCE_UP                              GPIO_PortSourceGPIOD    					 
#define GPIO_PINSOURCE_UP                               GPIO_PinSource2

#define RCC_KEY_DOWN                                    RCC_APB2Periph_GPIOD		 
#define GPIO_PORT_KEY_DOWN                              GPIOD    					 
#define GPIO_PIN_KEY_DOWN                              GPIO_Pin_3
#define GPIO_PORTSOURCE_DOWN                              GPIO_PortSourceGPIOD    					 
#define GPIO_PINSOURCE_DOWN                               GPIO_PinSource3

#define RCC_KEY_ENTER                                    RCC_APB2Periph_GPIOD		 
#define GPIO_PORT_KEY_ENTER                             GPIOD    					 
#define GPIO_PIN_KEY_ENTER                               GPIO_Pin_4
#define GPIO_PORTSOURCE_ENTER                              GPIO_PortSourceGPIOD    					 
#define GPIO_PINSOURCE_ENTER                              GPIO_PinSource4

#define RCC_KEY_ESC                                    RCC_APB2Periph_GPIOD		 
#define GPIO_PORT_KEY_ESC                             GPIOD    					 
#define GPIO_PIN_KEY_ESC                              GPIO_Pin_5
#define GPIO_PORTSOURCE_ESC                              GPIO_PortSourceGPIOD    					 
#define GPIO_PINSOURCE_ESC                               GPIO_PinSource5
//*********usb spi1	********************************
#define GPIO_PORT_SPI1                              	GPIOA    					 
#define GPIO_PIN_SPI1_MOSI                               GPIO_Pin_7
#define GPIO_PIN_SPI1_MISO                               GPIO_Pin_6
#define GPIO_PIN_SPI1_SCK                               GPIO_Pin_5
#define GPIO_PIN_SPI1_NSS                               GPIO_Pin_4
#define GPIO_PORT_USBRESET                              GPIOD    					 
#define GPIO_PIN_USBRESET                              GPIO_Pin_8
#define GPIO_PORT_USBINT                              GPIOA    					 
#define GPIO_PIN_USBINT                              GPIO_Pin_10
#define GPIO_PORTSOURCE_USBINT                              GPIO_PortSourceGPIOA    					 
#define GPIO_PINSOURCE_USBINT                               GPIO_PinSource10
/////stm32的外设的DMA请求与DMA通道的对应关系是固定的，不是随便配置的。参考STM32的参考手册
typedef struct 
{
	
	DMA_Channel_TypeDef		*dma_rx_base;
	int32_t					dma_rx_flag;
	int32_t 				dma_rx_irq;
	
	
	DMA_Channel_TypeDef		*dma_tx_base;
	int32_t					dma_tx_flag;
	int32_t 					dma_tx_irq;
	
	
	

}Dma_source;

typedef struct {
	//本串口属于芯片的第几个串口，从0开始
	int						uartNum;
	USART_InitTypeDef		*cfguart;
	Dma_source				*dma;
	
}CfgUart_t;

typedef struct {
    char                   work_mode;  //0 主1 从
    char                   mode; //0 - 3
    /*
        Mode 0 CPOL=0, CPHA=0 
        Mode 1 CPOL=0, CPHA=1
        Mode 2 CPOL=1, CPHA=0 
        Mode 3 CPOL=1, CPHA=1
    */
//		char                    dri_mode;		//驱动模式:0 cpu轮询， 1 中断模式 2 DMA模式
         
    char                    nss;			//0 软件nss模式 1 硬件nss 模式
		char										datasize_bit;		// 8 or 16
		int											baud;
}spi_conf_t;


extern CfgUart_t g_confUart2, g_confUart1;

#define GPIO_DIR_IN			0
#define GPIO_DIR_OUT		1

#define GPIO_IRQ_RISING		1
#define GPIO_IRQ_FAILING	2
#define GPIO_IRQ_BOTHEDGE	3


typedef struct 
{
	GPIO_TypeDef	*Port;
	uint8_t			pin;
	uint8_t			direction;

	//etti
	uint8_t			extiLine;
	uint8_t			irqType;	
	uint8_t			portSource;
	uint8_t			pinSource;
	
	uint8_t			none[2];
}gpio_pins;

extern gpio_pins Dir_485_pin1 , Dir_485_pin2;

extern gpio_pins pin_keyRight;
extern gpio_pins pin_keyLeft;
extern gpio_pins pin_keyUp;
extern gpio_pins pin_keyDown;
extern gpio_pins pin_keyEnter;
extern gpio_pins pin_keyEsc,;
extern gpio_pins pin_UsbInt;
extern spi_conf_t arr_conf_spi[2];

#endif
