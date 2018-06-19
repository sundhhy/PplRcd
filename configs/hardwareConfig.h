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

#define NUM_SPIS        3
#define NUM_IICS        2

#define UART_MODE_INTR		0
#define UART_MODE_DMA		1
#define UART_MODE_CPU		2
//各种外设的引脚配置

/************* PWR	*********************************************/
#define PORT_PWR                               GPIOA    					 
#define PIN_PWR                                GPIO_Pin_9	
#define GPIO_PORTSOURCE_PWR                              GPIO_PortSourceGPIOA					 
#define GPIO_PINSOURCE_PWR                               GPIO_PinSource9

/************* spi flash ****************************************/
#define PORT_FSH_nWP                               GPIOD    					 
#define PIN_FSH_nWP                                GPIO_Pin_10	

#define PORT_W25Q_nCS                               GPIOB    					 
#define PIN_W25Q_nCS                                GPIO_Pin_12	
#define PORT_W25Q_SO                               	GPIOB    					 
#define PIN_W25Q_SO                                	GPIO_Pin_14	
#define PORT_W25Q_SI                               	GPIOB    					 
#define PIN_W25Q_SI                                	GPIO_Pin_15	
#define PORT_W25Q_SCK                               GPIOB    					 
#define PIN_W25Q_SCK                                GPIO_Pin_13	



#define PORT_FM25_nCS                               GPIOA    					 
#define PIN_FM25_nCS                                GPIO_Pin_8
#define PORT_FM25_SO                               	GPIOB    					 
#define PIN_FM25_SO                                	GPIO_Pin_14	
#define PORT_FM25_SI                               	GPIOB    					 
#define PIN_FM25_SI                                	GPIO_Pin_15	
#define PORT_FM25_SCK                               GPIOB    					 
#define PIN_FM25_SCK                                GPIO_Pin_13



/*************************************************************************/

#define GPIO_PORT_UART1TX                               GPIOB    					 
#define GPIO_PIN_UART1TX                                GPIO_Pin_6					 

#define GPIO_PORT_UART1RX                               GPIOB    					 
#define GPIO_PIN_UART1RX                                GPIO_Pin_7	

#define GPIO_PORT_UART2TX                               GPIOA    					 
#define GPIO_PIN_UART2TX                                GPIO_Pin_2					 

#define GPIO_PORT_UART2RX                               GPIOA    					 
#define GPIO_PIN_UART2RX                                GPIO_Pin_3

//#define GPIO_PORT_UART2TX                               GPIOC    					 
//#define GPIO_PIN_UART2TX                                GPIO_Pin_10					 

//#define GPIO_PORT_UART2RX                               GPIOC    					 
//#define GPIO_PIN_UART2RX                                GPIO_Pin_11


#define GPIO_PORT_UART3TX                               GPIOB    					 
#define GPIO_PIN_UART3TX                                GPIO_Pin_10					 

#define GPIO_PORT_UART3RX                               GPIOB    					 
#define GPIO_PIN_UART3RX                                GPIO_Pin_11

#define GPIO_PORT_UART4TX                               GPIOC    					 
#define GPIO_PIN_UART4TX                                GPIO_Pin_10					 

#define GPIO_PORT_UART4RX                               GPIOC    					 
#define GPIO_PIN_UART4RX                                GPIO_Pin_11
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


#define GPIO_PORT_BEEP                             GPIOC					 
#define GPIO_PIN_BEEP                              GPIO_Pin_6

//*********usb spi1	********************************
#define GPIO_PORT_SPI1                              	GPIOA    					 
#define GPIO_PIN_SPI1_MOSI                               GPIO_Pin_7
#define GPIO_PIN_SPI1_MISO                               GPIO_Pin_6
#define GPIO_PIN_SPI1_SCK                               	GPIO_Pin_5
#define GPIO_PIN_SPI1_NSS                               GPIO_Pin_4

#define GPIO_PORT_USBRESET                              GPIOD    					 
#define GPIO_PIN_USBRESET                              	GPIO_Pin_8

#define GPIO_PORT_POWER                              GPIOE    					 
#define GPIO_PIN_POWER                              	GPIO_Pin_14

#define GPIO_PORT_USBINT                              	GPIOA    					 
#define GPIO_PIN_USBINT                              GPIO_Pin_10
#define GPIO_PORTSOURCE_USBINT                              GPIO_PortSourceGPIOA    					 
#define GPIO_PINSOURCE_USBINT                               GPIO_PinSource10

//********************* PCF 8563 IIC *************************************************
#define GPIO_PORT_IIC_SDA                              	GPIOB    					 
#define GPIO_PIN_IIC_SDA                              GPIO_Pin_9
//#define GPIO_PORTSOURCE_IIC_SDA                              GPIO_PortSourceGPIOB   					 
//#define GPIO_PINSOURCE_IIC_SDA                              GPIO_PinSource9

#define GPIO_PORT_IIC_SCL                              	GPIOB    					 
#define GPIO_PIN_IIC_SCL                              GPIO_Pin_8


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
	
	USART_InitTypeDef		*cfguart;
	Dma_source				*dma;
	//本串口属于芯片的第几个串口，从0开始
	char					uartNum;
	char					opt_mode;		//0 中断收发， 1 DMA收发， 2 CPU收发
	char					none[2];
	
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

typedef struct {
	
//	char						i2c_mode;
		/*
		0 	从发送器模式
		1	从接收器模式
		2	主发送器模式
		3	主接收器模式
	
	*/
	
		uint32_t							speed;				// < 400K										
    char                  own_addr;  
		char									ack_enbale;
		char									addr_bits;		//寻址模式,7 或10 
	char									duty_cycle;		//SCL的占空比0 2:1  其他 69:9
}iic_conf_t;



#define GPIO_DIR_IN			0
#define GPIO_DIR_OUT		1

#define GPIO_IRQ_RISING		1
#define GPIO_IRQ_FAILING	2
#define GPIO_IRQ_BOTHEDGE	3
#define GPIO_IRQ_DISABLE	4

typedef struct 
{
	GPIO_TypeDef	*Port;
	uint16_t			pin;
	uint8_t			direction;

	//etti
	//中断线配置参考STM32中文手册 8.2.5
	uint8_t			extiLine;
	uint8_t			irqType;	
	uint8_t			portSource;
	uint8_t			pinSource;
	
	uint8_t			none;
}gpio_pins;


extern CfgUart_t g_confUart1, g_confUart2, g_confUart3, g_confUart4;
extern iic_conf_t arr_conf_IIC[2];
//extern gpio_pins Dir_485_pin1 , Dir_485_pin2;

extern gpio_pins pin_keyRight;
extern gpio_pins pin_keyLeft;
extern gpio_pins pin_keyUp;
extern gpio_pins pin_keyDown;
extern gpio_pins pin_keyEnter;
extern gpio_pins pin_keyEsc;
extern gpio_pins pin_UsbInt;
extern gpio_pins pin_pwr;
extern spi_conf_t arr_conf_spi[NUM_SPIS];

#endif
