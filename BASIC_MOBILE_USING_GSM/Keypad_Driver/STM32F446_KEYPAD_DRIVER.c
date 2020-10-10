/*
 * STM32F446_KEYPAD_DRIVER.c
 *
 *  Created on: 06-Oct-2020
 *      Author: NAMAN
 */

#include"STM32F446_KEYPAD_DRIVER.h"
#include<stdint.h>
#include<stdio.h>

/* Row pins are output*/
#define SET_ROW_1_HIGH		GPIO_SetBits(KEYPAD_ROW_1_PORT, KEYPAD_ROW_1_PIN)
#define SET_ROW_2_HIGH		GPIO_SetBits(KEYPAD_ROW_2_PORT, KEYPAD_ROW_2_PIN)
#define SET_ROW_3_HIGH		GPIO_SetBits(KEYPAD_ROW_3_PORT, KEYPAD_ROW_3_PIN)
#define SET_ROW_4_HIGH		GPIO_SetBits(KEYPAD_ROW_4_PORT, KEYPAD_ROW_4_PIN)
#define SET_ROW_1_LOW		GPIO_ResetBits(KEYPAD_ROW_1_PORT, KEYPAD_ROW_1_PIN)
#define SET_ROW_2_LOW		GPIO_ResetBits(KEYPAD_ROW_2_PORT, KEYPAD_ROW_2_PIN)
#define SET_ROW_3_LOW		GPIO_ResetBits(KEYPAD_ROW_3_PORT, KEYPAD_ROW_3_PIN)
#define SET_ROW_4_LOW		GPIO_ResetBits(KEYPAD_ROW_4_PORT, KEYPAD_ROW_4_PIN)

/* COLUMN PINS ARE INPUT, SO READ THE PINS
 */
#define READ_COLUMN_1		GPIO_ReadInputDataBit(KEYPAD_COLUMN_1_PORT, KEYPAD_COLUMN_1_PIN)
#define READ_COLUMN_2		GPIO_ReadInputDataBit(KEYPAD_COLUMN_2_PORT, KEYPAD_COLUMN_2_PIN)
#define READ_COLUMN_3		GPIO_ReadInputDataBit(KEYPAD_COLUMN_3_PORT, KEYPAD_COLUMN_3_PIN)
#define READ_COLUMN_4		GPIO_ReadInputDataBit(KEYPAD_COLUMN_4_PORT, KEYPAD_COLUMN_4_PIN)

/* Keypad Button Values*/
const char Keypad_Button_Values[4][4] =  {   {'1', '2', '3', 'A'},
	                                     	 {'4', '5', '6', 'B'},
											 {'7', '8', '9', 'C'},
											 {'*', '0', '#', 'D'},
                                         };

/* SET sPECIFIC KEYPAD ROW TO LOW*/
static void Set_Keypad_Row(uint8_t Row)
{
	//Make all row high
	SET_ROW_1_HIGH;
	SET_ROW_2_HIGH;
	SET_ROW_3_HIGH;
	SET_ROW_4_HIGH;

	//MAKE THE SPECIFIC ROW LOW BASED ON "ROW" VALUE
	if(Row == 1)
		SET_ROW_1_LOW;

	if(Row == 2)
		SET_ROW_2_LOW;

	if(Row == 3)
		SET_ROW_3_LOW;

	if(Row == 4)
		SET_ROW_4_LOW;
}

/*For specific row value sent, check all the columns*/
static char Check_Keypad_Column(uint8_t Row)
{
	if(!READ_COLUMN_1)									//if column 1 is low
	{
		while(!READ_COLUMN_1);							//wait till key is pressed
		return Keypad_Button_Values[Row-1][0];
	}

	if(!READ_COLUMN_2)									//if column 2 is low
		{
			while(!READ_COLUMN_2);							//wait till key is pressed
			return Keypad_Button_Values[Row-1][1];
		}

	if(!READ_COLUMN_3)									//if column 3 is low
		{
			while(!READ_COLUMN_3);							//wait till key is pressed
			return Keypad_Button_Values[Row-1][2];
		}

	if(!READ_COLUMN_4)									//if column 4 is low
		{
			while(!READ_COLUMN_4);							//wait till key is pressed
			return Keypad_Button_Values[Row-1][3];
		}

	return KEYPAD_NOT_PRESSED;
}

//Initialize the keypad
void KEYPAD_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	//GPIO Port Clock Enable GPIOB & GPIOE
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	//Configure Row pins output level- RESET all pins

	SET_ROW_1_LOW;
	SET_ROW_2_LOW;
	SET_ROW_3_LOW;
	SET_ROW_4_LOW;

	GPIO_InitStruct.GPIO_Pin = KEYPAD_ROW_1_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(KEYPAD_ROW_1_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = KEYPAD_ROW_2_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(KEYPAD_ROW_2_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = KEYPAD_ROW_3_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(KEYPAD_ROW_3_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = KEYPAD_ROW_4_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(KEYPAD_ROW_4_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = KEYPAD_COLUMN_1_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(KEYPAD_COLUMN_1_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = KEYPAD_COLUMN_2_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(KEYPAD_COLUMN_2_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = KEYPAD_COLUMN_3_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(KEYPAD_COLUMN_3_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = KEYPAD_COLUMN_4_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(KEYPAD_COLUMN_4_PORT, &GPIO_InitStruct);

}

//READ THE KEYPAD
char KEYPAD_Read(void)
{
	char check;
	//set row 1 low and scan all columns
	Set_Keypad_Row(1);
	check = Check_Keypad_Column(1);
	if(check);
	return check;

	//set row 2 low and scan all columns
	Set_Keypad_Row(2);
	check = Check_Keypad_Column(2);
	if(check);
	return check;

	//set row 3 low and scan all columns
	Set_Keypad_Row(3);
	check = Check_Keypad_Column(3);
	if(check);
	return check;

	//set row 4 low and scan all columns
	Set_Keypad_Row(4);
	check = Check_Keypad_Column(4);
	if(check);
	return check;

	return KEYPAD_NOT_PRESSED;

}







