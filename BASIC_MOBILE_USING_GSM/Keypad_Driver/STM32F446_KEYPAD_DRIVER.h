/*
 * STM32F446_KEYPAD_DRIVER.h
 *
 *  Created on: 06-Oct-2020
 *      Author: NAMAN
 */

#ifndef KEYPAD_DRIVER_STM32F446_KEYPAD_DRIVER_H_
#define KEYPAD_DRIVER_STM32F446_KEYPAD_DRIVER_H_

#include"stm32f4xx.h"


/***********------------------------Define your GPIO pins here-------------------****************/

/*
 * Keypad		STM32F446			Description				I/O
 * R1			PB15				Row 1					OUTPUT
 * R2			PB13				Row 2					OUTPUT
 * R3			PB11				Row 3					OUTPUT
 * R4			PE15				Row 4					OUTPUT
 *
 * C1			PE13				COL 1					INPUT
 * C2			PE11				COL 2					INPUT
 * C3			PE9					COL 3					INPUT
 * C4			PE7					COL 4					INPUT
 */

/* ROWS */
/* ROW 1 DEFAULT */
#define KEYPAD_ROW_1_PORT				GPIOB
#define KEYPAD_ROW_1_PIN				GPIO_Pin_15

/* ROW 2 DEFAULT */
#define KEYPAD_ROW_2_PORT				GPIOB
#define KEYPAD_ROW_2_PIN				GPIO_Pin_13

/* ROW 3 DEFAULT */
#define KEYPAD_ROW_3_PORT				GPIOB
#define KEYPAD_ROW_3_PIN				GPIO_Pin_11

/* ROW 4 DEFAULT */
#define KEYPAD_ROW_4_PORT				GPIOE
#define KEYPAD_ROW_4_PIN				GPIO_Pin_15

/* COLUMNS */
/* COLUMN 1 DEFAULT */
#define KEYPAD_COLUMN_1_PORT			GPIOE
#define KEYPAD_COLUMN_1_PIN				GPIO_Pin_13

/* COLUMN 2 DEFAULT */
#define KEYPAD_COLUMN_2_PORT			GPIOE
#define KEYPAD_COLUMN_2_PIN				GPIO_Pin_11

/*COLUMN 3 DEFAULT */
#define KEYPAD_COLUMN_3_PORT			GPIOE
#define KEYPAD_COLUMN_3_PIN				GPIO_Pin_9

/* COLUMN 4 DEFAULT */
#define KEYPAD_COLUMN_4_PORT			GPIOE
#define KEYPAD_COLUMN_4_PIN				GPIO_Pin_7



/*------------------------Define your GPIO pins here - END----------------------------*/
/* Keypad NOT pressed */
#define NULL_CHARACTER              '\0'
#define KEYPAD_NOT_PRESSED			NULL_CHARACTER

/**
 * @brief  Initializes keypad functionality
 * @param  none
 * @retval None
 */
void KEYPAD_Init(void);

/**
 * @brief  Reads keypad data
 * @param  None
 * @retval Button status. This parameter will be a value of KEYPAD_Button_t enumeration
 */
char KEYPAD_Read(void);


#endif /* KEYPAD_DRIVER_STM32F446_KEYPAD_DRIVER_H_ */
