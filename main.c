#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include "stm32f4xx_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f4xx_spi.h"              // Keil::Device:StdPeriph Drivers:SPI
#include <stdio.h>

GPIO_InitTypeDef GPIO_InitTypeStructure;
SPI_InitTypeDef SPI_InitTypeStructure;

//eksenler
int x;
int y;
int z;

void spi_config()
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); //apb1 hatti spi_init için aktif edildi..
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
	//CS pini için gereken gpio_init
	GPIO_InitTypeStructure.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitTypeStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitTypeStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_Init(GPIOE, &GPIO_InitTypeStructure);

	// spi config
	
	GPIO_InitTypeStructure.GPIO_Pin=GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitTypeStructure.GPIO_Mode=GPIO_Mode_AF; // Connect the pin to the desired peripherals' Alternate Function (AF) 
	GPIO_InitTypeStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitTypeStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	
	//SPI_Init()
	
	SPI_InitTypeStructure.SPI_Mode=SPI_Mode_Master;
	SPI_InitTypeStructure.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitTypeStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI_InitTypeStructure.SPI_CPOL=SPI_CPOL_High;
	SPI_InitTypeStructure.SPI_CPHA=SPI_CPHA_2Edge;
	SPI_InitTypeStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
	SPI_InitTypeStructure.SPI_NSS=SPI_NSS_Soft| SPI_NSSInternalSoft_Set;
	SPI_InitTypeStructure.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_Init(SPI1, &SPI_InitTypeStructure);
	SPI_Cmd(SPI1,ENABLE);
	GPIO_SetBits(GPIOE,GPIO_Pin_3);	
}

 USART_InitTypeDef USART_InitTypeStructure;

void usart_config()	
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	// GPIO 
	
	GPIO_InitTypeStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitTypeStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitTypeStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitTypeStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitTypeStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitTypeStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	
	// USART init
	
	USART_InitTypeStructure.USART_BaudRate=9600;
	USART_InitTypeStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitTypeStructure.USART_StopBits=USART_StopBits_1;
	USART_InitTypeStructure.USART_Parity=USART_Parity_No;
	USART_InitTypeStructure.USART_Mode=USART_Mode_Tx;
	USART_InitTypeStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	
	USART_Init(USART2, &USART_InitTypeStructure);
	USART_Cmd(USART2, ENABLE);	
}


char str[50];
uint32_t i = 0;

void USART_Puts(USART_TypeDef* USARTx, volatile char *s)
{
while(*s)
{
while(!(USARTx ->SR & 0x00000040));
USART_SendData(USARTx, *s);
*s++;
}
}


uint8_t SPI_Rx(uint8_t adress)
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_3); // CS pini resetlendi. suan için okuma yazma aktif
	adress=0x80|adress; 
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1,adress);
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
	SPI_I2S_ReceiveData(SPI1);
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1,0x00);
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
	
	
	GPIO_SetBits(GPIOE,GPIO_Pin_3); //CS pini set edildi. spi kapatildi.
	return SPI_I2S_ReceiveData(SPI1);
}

int main()
{
	usart_config();
	spi_config();
	
	while(1)
	{
		x=SPI_Rx(0x29);
		y=SPI_Rx(0x2B);
		z=SPI_Rx(0x2D);
		sprintf(str,"x=%d , y=%d , z=%d\n", x, y, z);
		USART_Puts(USART2,str);
		
	}
}





