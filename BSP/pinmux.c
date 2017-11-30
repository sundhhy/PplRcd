#include "stm32f10x.h"

#include "pinmux.h"
#include "hardwareConfig.h"



void Pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*********** usb 	************************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_USBRESET;                   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIO_PORT_USBRESET, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_POWER;                   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIO_PORT_POWER, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_USBINT;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init( GPIO_PORT_USBINT, &GPIO_InitStructure);
	/*********** usb spi1	************************************/

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SPI1_MOSI;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIO_PORT_SPI1, &GPIO_InitStructure);
//	GPIO_PinRemapConfig(GPIO_Remap_SPI1,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SPI1_MISO;                   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIO_PORT_SPI1, &GPIO_InitStructure);
//	GPIO_PinRemapConfig(GPIO_Remap_SPI1,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SPI1_SCK;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIO_PORT_SPI1, &GPIO_InitStructure);
//	GPIO_PinRemapConfig(GPIO_Remap_SPI1,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SPI1_NSS;                   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIO_PORT_SPI1, &GPIO_InitStructure);
	
	/*********************************************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_UART1TX;        //tx
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init( GPIO_PORT_UART1TX, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_UART1RX;                   //rx
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIO_PORT_UART1RX, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_UART2TX;        //tx
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIO_PORT_UART2TX, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_UART2RX;                   //rx
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIO_PORT_UART2RX, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_UART3TX;        //tx
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIO_PORT_UART3TX, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_UART3RX;                   //rx
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIO_PORT_UART3RX, &GPIO_InitStructure);
	
	
	/*********** key pins	************************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_RIGHT;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init( GPIO_PORT_KEY_RIGHT, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_LEFT;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init( GPIO_PORT_KEY_LEFT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_UP;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init( GPIO_PORT_KEY_UP, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_DOWN;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init( GPIO_PORT_KEY_DOWN, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_ENTER;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init( GPIO_PORT_KEY_ENTER, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_ESC;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init( GPIO_PORT_KEY_ESC, &GPIO_InitStructure);
	
	
	
}
