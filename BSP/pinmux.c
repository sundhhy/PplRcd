#include "stm32f10x.h"

#include "pinmux.h"
#include "hardwareConfig.h"



void Pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_UART1TX;        //tx
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIO_PORT_UART1TX, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_UART1RX;                   //rx
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIO_PORT_UART1RX, &GPIO_InitStructure);
	
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
	
}
