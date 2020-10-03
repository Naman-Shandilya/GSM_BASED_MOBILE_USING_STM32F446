/*
 * STM32F446_I2C_LCD_Driver.c
 *
 *  Created on: 03-Oct-2020
 *      Author: NAMAN
 */
#include"STM32F446_I2C_LCD_Driver.h"
#include"stm32f4xx.h"

/*I2C Init structure*/
I2C_InitTypeDef i2c_init;

/***************** Private function for I2C initialization *******************/

// Configure the gpio pins for i2c
static void GPIO_Config(void)
{
	//1.enable the port clocks
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

	 //GPIO Init type def
	 GPIO_InitTypeDef i2c_pin;

	 //GPIO Pins Configuration
	 i2c_pin.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	 i2c_pin.GPIO_Mode = GPIO_Mode_AF;
	 i2c_pin.GPIO_OType = GPIO_OType_OD;
	 i2c_pin.GPIO_PuPd = GPIO_PuPd_UP;
	 i2c_pin.GPIO_Speed = GPIO_Fast_Speed;
	 GPIO_Init(GPIOB, &i2c_pin);

	 //Gpio pin alternate function setting
	 GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);
	 GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_I2C1);



}

// Configure the I2C peripheral
static void I2C_Config(void)
{
	//enable i2c periphal clock
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);

	 i2c_init.I2C_Mode = I2C_Mode_I2C;
	 i2c_init.I2C_Ack = I2C_Ack_Enable;
	 i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	 i2c_init.I2C_ClockSpeed = 100000; //standard mode 100KHz
	 i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
	 I2C_Init(I2C1, &i2c_init);
	 I2C_Cmd(I2C1, ENABLE);

	 //enable the clock stretching mode
	 I2C_StretchClockCmd(I2C1, ENABLE);

}

