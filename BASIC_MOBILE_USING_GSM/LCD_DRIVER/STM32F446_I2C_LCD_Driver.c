/*
 * STM32F446_I2C_LCD_Driver.c
 *
 *  Created on: 03-Oct-2020
 *      Author: NAMAN
 */
#include"stm32f4xx.h"
#include "STM32F446_I2C_LCD_Driver.h"

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
	//enable i2c peripheral clock
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

void I2C_Config_DeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  //I2C Peripheral Disable
  I2C_Cmd(I2C1, DISABLE);

  //I2C DeInit (Disables clock)
  I2C_DeInit(I2C1);
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, DISABLE);

  //GPIO configuration
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

}


uint32_t I2C_Master_WrData(uint8_t DevAddr, uint8_t data)
{
	//Write single byte data to the given register address

	//Generate start condition
	I2C_GenerateSTART(I2C1, ENABLE);

	//SendI2C device Address and clear ADDR
	I2C_Send7bitAddress(I2C1, DevAddr, I2C_Direction_Transmitter);
	//I2C1->SR1 &= ~I2C_SR1_ADDR;  // CLearing the address sent flag bit after transmitting the slave addr
	(void)(I2C1->SR2); //// CLearing the address sent flag bit after transmitting the slave addr
	//Send data
	I2C_SendData(I2C1,data);
	//wait till the data is actually send
	//while(I2C_GetFlagStatus(I2C1,I2C_SR1_TXE)!=I2C_SR1_TXE);
	while(I2C_GetFlagStatus(I2C1, I2C_SR1_BTF));
	//generate stop condition
	I2C_GenerateSTOP(I2C1,ENABLE);

	//wait untill bus becomes free
	while(I2C_GetFlagStatus(I2C1,I2C_SR2_BUSY)!=0);


	return 0;

}


uint32_t I2C_Master_RdData(uint8_t DevAddr, uint8_t RegAddr, uint8_t *pbuf,uint32_t cnt)
{
	//Read "cnt" number of data starting from RegAddr

	//Generating start condition
	I2C_GenerateSTART(I2C1, ENABLE);

	//Send I2C device address and clear addr with write command
	I2C_Send7bitAddress(I2C1, DevAddr, I2C_Direction_Transmitter);
	(void)(I2C1->SR2); //// CLearing the address sent flag bit after transmitting the slave addr

	//Send I2C Register Addr
	I2C_SendData(I2C1,RegAddr);

	//Start Receiving Data
	I2C_RdBuf(DevAddr, pbuf, cnt);

	return 0;
}

uint32_t I2C_RdBuf(uint8_t DevAddr, uint8_t *pbuf, uint32_t cnt)
{
	//Generate Start
	I2C_GenerateSTART(I2C1, ENABLE);

	//Send I2C DevAddr with read command
	I2C_Send7bitAddress(I2C1, DevAddr, I2C_Direction_Transmitter);

	if(cnt==1)
	{
		//we are going to read only 1 byte
		//Before clearing Addr bit by reading SR2, we have to cancle (Disable) ack.
		I2C1->CR1 &= ~ I2C_CR1_ACK;

		//clear the Addr flag
		(void)I2C1->SR1;

		//Order a stop condition
		//Note- According to the I2C Spec_p583 this should be done just after clearing the ADDR
		//If it is done before ADDR is set, a stop is generated immediately as the clock is being stretched
		I2C_GenerateSTOP(I2C1, ENABLE);
		//Be careful that till the stop condition is actually transmitted the clock will stay active even if a NACK is generated after the next received byte

		//Read the next byte
		pbuf = I2C_ReceiveData(I2C1);

		//Clearing the stop bit detected by reading the register
		I2C_ReadRegister(I2C1, I2C_Register_SR1);

		//Enable the Acknowledgment again
		I2C1->CR1 |= I2C_CR1_ACK;

	}

	else if(cnt==2)
	{
		//We are going to read 2 bytes
		//Before Clearing Addr, Reset ACK, set POS
		I2C1->CR1 &= ~ I2C_CR1_ACK;
		I2C1->CR1 |= I2C_CR1_POS;

		//Read the SR2 to clear ADDR
		(void)I2C1->SR2;
		//I2C_ReadRegister(I2C1, I2C_Register_SR1);

		//Wait for the next 2 byte to be received (1st in DR, 2nd in the shift register)
		while(I2C_GetFlagStatus(I2C1, I2C_SR1_BTF)!=1);
		//As we don't read anything from dr, the clock is now being stretched

		//order a stop condition(as the clock is being stretched, the stop condition is generated immediatly)
		I2C_GenerateSTOP(I2C1, ENABLE);

		//Read the next two bytes
		pbuf= I2C_ReceiveData(I2C1);
		pbuf++;
		pbuf= I2C_ReceiveData(I2C1);

		//clearing the stop bit by reading the register
		I2C_ReadRegister(I2C1, I2C_Register_SR1);

		//Enable the ack and reset Pos
		I2C1->CR1 |= I2C_CR1_ACK;
		I2C1->CR1 &=~ I2C_CR1_POS;
	}
	else
	{
		//We have more than 2 bytes

		//Read SR1 To clear ADDR
		(void)I2C1->SR1;

		while((cnt--)>3)
		{
			pbuf= I2C_ReceiveData(I2C1);
			pbuf++;
		}

		//3 more bytes to read. wait till the next is actually received
		while(I2C_GetFlagStatus(I2C1, I2C_SR1_BTF)!=1);

		//Here the clock is stretched. One more to read

		//Reset Ack
		I2C1->CR1 &=~ I2C_CR1_ACK;

		//Read N-2
		pbuf=I2C_ReceiveData(I2C1);
		pbuf++;

		//2 more bytes to read. wait till the next is actually received
		while(I2C_GetFlagStatus(I2C1, I2C_SR1_BTF)!=1);
		//Here clock is stretched

		//Generate stop condition
		I2C_GenerateSTOP(I2C1, ENABLE);

		//read last two bytes
		pbuf= I2C_ReceiveData(I2C1);
		pbuf++;
		pbuf= I2C_ReceiveData(I2C1);

		//clearing the stop bit by reading the register
		I2C_ReadRegister(I2C1, I2C_Register_SR1);

		//Enable the ack
		I2C1->CR1 |= I2C_CR1_ACK;

	}
	return 0;
}

void I2C_WrData(uint8_t DevAddr, uint8_t data, uint32_t cnt)
{
	//Write multiple byte of data

	//generate a start condition
	I2C_GenerateSTART(I2C1, ENABLE);

	//Send I2C device address + write command
	I2C_Send7bitAddress(I2C1,DevAddr, I2C_Direction_Transmitter);

	//unstretch the clock by just reading SR1(as the clock is stretched because we have not written anything to the DR yet)
	(void)I2C1->SR1;

	//start writing data
	while(cnt--)
	{
		I2C_SendData(I2C1, data);
	}

	//wait for the data on the shift register to be transmitted
	while(I2C_GetFlagStatus(I2C1,I2C_SR1_BTF)!=1);
	//Here TXE=BTF=1. Therefore the clock stretches again

	//order a stop condition at the end of the current transmission (or if the clock is stretched, generate stop immediatelly)
	I2C_GenerateSTOP(I2C1, ENABLE);

	//Wait to be sure that line is idle
	I2C_ReadRegister(I2C1, I2C_Register_SR1);
}




/**********************************************************************************************/



/*****************************LCD APIs********************************************************/


/*
 * @brief Send Command To LCD
 * @param cmd : LCD commands
 * @retval None
 */
void LCD_Send_Cmd(uint8_t cmd)
{
	uint8_t data_u, data_l;
	uint8_t data_u_tx[2], data_l_tx[2];

	/*Store upper nibble*/
	data_u = (cmd & 0xF0);

	/*Store lower nibble*/
	data_l = ((cmd << 4) & 0xF0);

	/*Construct upper byte-> compatible for LCD*/
	data_u_tx[0] = data_u|BACKLIGHT|PIN_EN;
	data_u_tx[1] = data_u|BACKLIGHT;

	/*Construct lower byte-> compatible for lcd*/
	data_l_tx[0] = data_l|BACKLIGHT|PIN_EN;
	data_l_tx[1] = data_l|BACKLIGHT;

	/*Transmit upper nibble*/
	I2C_WrData(I2C_SLAVE_ADDRESS, data_u_tx, 2);

	/*Provide Delay*/
	Delay_ms(5);

	/*Transmit lower nibble*/
	I2C_WrData(I2C_SLAVE_ADDRESS, data_l_tx,2);

	//Provide delay
	Delay_ms(5);
}


void LCD_Send_Data(uint8_t data)
{
	uint8_t data_u, data_l;
	uint8_t data_u_tx[2], data_l_tx[2];

	/*store upper nibble*/
	data_u = (data & 0xF0);

	/*store lower nibble*/
	data_l=((data << 4) & 0xF0);

	/*construct upper byte ->compatible for LCD*/
	data_u_tx[0] = data_u|BACKLIGHT|PIN_EN|PIN_RS;
	data_u_tx[1] = data_u|BACKLIGHT|PIN_RS;

	/*construct lower byte-> compatible for LCD*/
	data_l_tx[0] = data_l|BACKLIGHT|PIN_EN|PIN_RS;
	data_l_tx[1] = data_l|BACKLIGHT|PIN_RS;

	/*Transmit upper nibble*/
	I2C_WrData(I2C_SLAVE_ADDRESS, data_u_tx,2);

	/*provide delay*/
	Delay_ms(5);

	/*Transmit lower nibble*/
	I2C_WrData(I2C_SLAVE_ADDRESS, data_l_tx,2);

	/*provide delay*/
	Delay_ms(5);

}

/**
 * @brief  Initializes LCD
 * @retval None
 */
void LCD_Init(void)
{
	/*configure the GPIOs*/
	GPIO_Config();

	/*configure the I2C */
	I2C_Config();

	/*Wait of 15ms atleast*/
	Delay_ms(15);

	/*Function set as per HD44780U*/
	LCD_Send_Cmd(LCD_FUNCTION_SET1);

	/*Function set as per HD44780U*/
	LCD_Send_Cmd(LCD_FUNCTION_SET2);

	/*Set 4bit mode and 2 lines */
	LCD_Send_Cmd(LCD_4BIT_2LINE_MODE);

	/*Display on, cursor off*/
	LCD_Send_Cmd(0x0C);

	/* SET Row1 and Col1 (1st Line) */
	LCD_Send_Cmd(0x80);

	/*Clear Display*/
	LCD_Send_Cmd(LCD_CLEAR_DISPLAY);

}


/**
 * @brief Send Strings to LCD
 * @param str: pointer to strings
 * @retval None
 */
void LCD_Send_String(char *str)
{
	while (*str)
	{
		LCD_Send_Data(*str++);
	}
}

/**
 * @brief Clears screen first, then displays the given string
 * @param str: pointer to strings
 * @retval None
 */
void LCD_Clear_Then_Display(char *str)
{
	LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
	LCD_Send_String(str);
}

/**
 * @brief Display the strings on Line1
 * @param str: pointer to strings
 * @retval None
 */
void LCD_Send_String_On_Line1(char *str)
{
	LCD_Send_Cmd(LCD_SET_ROW1_COL1);
	LCD_Send_String(str);
}

/**
 * @brief Display the strings on Line2
 * @param str: pointer to strings
 * @retval None
 */
void LCD_Send_String_On_Line2(char *str)
{
	LCD_Send_Cmd(LCD_SET_ROW2_COL1);
	LCD_Send_String(str);
}

/**
 * @brief Display long messages of any size on LCD
 * @param str: pointer to strings
 * @retval None
 */

void LCD_Display_Long_Message(char *string)
{
	int i =0, count =1, j =0; //here i is the no. of char in string j is the position of char on screen line 1 and line2
	/*clear the display first*/
	LCD_Send_Cmd(LCD_CLEAR_DISPLAY);

	/*Set position to line 1*/
	LCD_Send_Cmd(LCD_SET_ROW1_COL1);

	while(string[i] !='\0')
	{
		LCD_Send_Data(string[i]);

		/*if we reach first line end, the go to second line start*/
		if(j>=15 && (count%2==1))
		{
			count++;
			LCD_Send_Cmd(LCD_SET_ROW2_COL1);
		}

		/*if we reach the second line end, the clear the display and start from line 1 again*/

		if(j>=31 && (count%2 == 0))
		{
			count++;
			j=0;
			LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
			LCD_Send_Cmd(LCD_SET_ROW1_COL1);
		}

		Delay_ms(100);
		i++;
		j++;
	}
}



