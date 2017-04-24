#include "stm32f10x.h"

#include "pinmux.h"
#include "hardwareConfig.h"



void Pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = pin_DugUartTx.pin;        //tx
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( pin_DugUartTx.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = pin_DugUartRx.pin;                   //rx
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( pin_DugUartRx.Port, &GPIO_InitStructure);
	
	
}
