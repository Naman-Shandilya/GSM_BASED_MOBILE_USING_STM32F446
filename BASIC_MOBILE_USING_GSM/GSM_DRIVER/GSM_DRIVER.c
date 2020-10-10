/*
 * GSM_DRIVER.c
 *
 *  Created on: 07-Oct-2020
 *      Author: NAMAN
 */

#include"GSM_DRIVER.h"
#include"string.h"
#include"stdint.h"

USART_InitTypeDef usart_init;
DMA_InitTypeDef dma_init;

#define UART2_DR_ADDRESS	((uint32_t)0x40000000 + 0x4400 + 0x04)

//RX buffer to store incoming data from GSM module
char RX_Buffer[RX_BUFFER_SIZE];

char TX_DMA[] = {'0'};
char RX_DMA[20];



//to store incomming SMS number
char Incoming_SMS_Phone_Num[13] = {'\0'};

//to store SMS recieved
char Incoming_SMS_Message[100] = {'\0'};

//-------------Private functions for UART and DMA configuration

/**
  *@brief    Configure UART
  *@param    None
  *@retval   None
*/
void UART_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	//GPIO PIN CONFIGURATION
	GPIO_InitTypeDef uart_pin;

	uart_pin.GPIO_Pin = GPIO_Pin_2;
	uart_pin.GPIO_Mode = GPIO_Mode_AF;
	uart_pin.GPIO_OType = GPIO_OType_PP;
	uart_pin.GPIO_PuPd = GPIO_PuPd_UP;
	uart_pin.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOA, &uart_pin);


	uart_pin.GPIO_Pin = GPIO_Pin_2;
		uart_pin.GPIO_Mode = GPIO_Mode_AF;
		uart_pin.GPIO_Speed = GPIO_Fast_Speed;
		GPIO_Init(GPIOA, &uart_pin);



	//setting gpio pin to alternate mode
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	//enable the usart oversampling by 8
	USART_OverSampling8Cmd(USART2, ENABLE);

	 /* USARTx configured as follows:
	        - BaudRate = 5250000 baud
			   - Maximum BaudRate that can be achieved when using the Oversampling by 8
			     is: (USART APB Clock / 8)
				 Example:
				    - (USART3 APB1 Clock / 8) = (42 MHz / 8) = 5250000 baud
				    - (USART1 APB2 Clock / 8) = (84 MHz / 8) = 10500000 baud
			   - Maximum BaudRate that can be achieved when using the Oversampling by 16
			     is: (USART APB Clock / 16)
				 Example: (USART3 APB1 Clock / 16) = (42 MHz / 16) = 2625000 baud
				 Example: (USART1 APB2 Clock / 16) = (84 MHz / 16) = 5250000 baud
	        - Word Length = 8 Bits
	        - one Stop Bit
	        - No parity
	        - Hardware flow control disabled (RTS and CTS signals)
	        - Receive and transmit enabled
	  */

	//UART configuration
	usart_init.USART_BaudRate = 115200;
	usart_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	usart_init.USART_WordLength = USART_WordLength_8b;
	usart_init.USART_StopBits = USART_StopBits_1;
	usart_init.USART_Parity = USART_Parity_No;
	usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	//config uasrt2
	USART_Init(USART2, &usart_init);

	//enable usart2
	USART_Cmd(USART2, ENABLE);

	//Enable RXNE interrupt
	//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);





	//Enable USART glabal interrupt
	//NVIC_EnableIRQ(USART2_IRQn);
	//NVIC_SetPriority(USART2_IRQn, 2);



}

void DMA_Config(void)
{
	//Enabling the DMA2 peripheral clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	//Configuring the DMA2 channel0 to transfer, in circular mode, the data recieved in USART2 data register to the
	//RX_Buffer variable
	dma_init.DMA_Channel = DMA_Channel_4;//this mapping is for usart2 tx
	dma_init.DMA_PeripheralBaseAddr = UART2_DR_ADDRESS;
	dma_init.DMA_Memory0BaseAddr = (uint32_t)&TX_DMA;
	dma_init.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_init.DMA_BufferSize = sizeof(TX_DMA);
	dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_init.DMA_MemoryInc =  DMA_MemoryInc_Enable;
	dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_init.DMA_Mode = DMA_Mode_Normal;
	dma_init.DMA_Priority = DMA_Priority_High;
	dma_init.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dma_init.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	dma_init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	//DMA_Init(DMA1_Stream6, &dma_init);// this enables the dma for usart2 tx only

	dma_init.DMA_Channel = DMA_Channel_4;//this mapping is for usart2 rx
	dma_init.DMA_PeripheralBaseAddr = UART2_DR_ADDRESS;
	dma_init.DMA_Memory0BaseAddr = (uint32_t)&RX_Buffer;
	dma_init.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma_init.DMA_BufferSize = RX_BUFFER_SIZE;
	dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_init.DMA_MemoryInc =  DMA_MemoryInc_Enable;
	dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_init.DMA_Mode = DMA_Mode_Normal;
	dma_init.DMA_Priority = DMA_Priority_High;
	dma_init.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dma_init.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	dma_init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;


	DMA_Init(DMA1_Stream5, &dma_init);  // this enables the dma for usart2 rx only
	//enable DMA2 Stream6

	DMA_Cmd(DMA1_Stream5, ENABLE);
}

void DMA_Tx(void)
{
	//enable DMA usart tx stream
	DMA_Cmd(DMA1_Stream6, ENABLE);

	//enable usart dma tx requests
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);

	//waiting the end of data transfer
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
	while(DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6)==RESET);

	//Now clear the DMA Transfer complete flag
	DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);
	USART_ClearFlag(USART2, USART_FLAG_TC);
}

void DMA_Rx(void)
{
	//enable DMA usart rx stream
	DMA_Cmd(DMA1_Stream5, ENABLE);

	//enable usart dma rx requests
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

	//waiting the end of data receive
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
	while(DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG_TCIF5)==RESET);

	//Now clear the DMA Transfer complete flag
	DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);
	USART_ClearFlag(USART2, USART_FLAG_TC);


}



/**********************************GSM APIs**************************/

/**
  *@brief    Compares the GSM-data strings with given String
  *@param    string : Character pointer to sting to be compared
  *@retval   if given string present in the GSM_String, returns 1
	           else returns 0.
*/

int GSM_Compare_GSMData_With(const char* string)
{
	char* ptr= NULL;
	//compare the given string with GSM data
	ptr = strstr(RX_Buffer, string);
	//if ptr equal to NULL, then no match found
	if(ptr != NULL)
		return 1;
	else
		return 0;


}

/**
  *@brief    Send AT Command to GSM A6 Module
  *@param    AT_CMD : command string to be transmitted
  *@retval   None
*/

void GSM_Send_AT_Command(char* AT_CMD)
{
	USART_SendData(USART2, (uint16_t*)AT_CMD);
}

/**
  *@brief    Receive Call. when this API is called its receive call
             by sending "ATA" command to GSM module
  *@param    None
  *@retval   None
*/

void GSM_Receive_Call(void)
{
	Delay_ms(1000);
	GSM_Send_AT_Command("ATA\r"); //ATA-Accept call command

	Delay_ms(100);
}

/**
  *@brief    Hang up the Call. when this API is called it terminates call
             by sending "ATH" command to GSM module
  *@param    None
  *@retval   None
*/
void GSM_HangUp_Call(void)
{
	HAL_Delay(500);
	GSM_Send_AT_Command("ATH\r");  //ATH - Hang Up call command
	HAL_Delay(500);
}

/**
  *@brief    Make a call to given phone number
  *@param    phone_number
  *@retval   None
*/
void GSM_Make_Call(char* phone_number)
{
	char AT_CMD[16] = "ATD+91";     //at command with +91(india code)
	char CR[]="\r";   //carriage return
	strcat(AT_CMD, phone_number);  //concatinate AT_CMD and phone number
	strcat(AT_CMD, CR);
	GSM_Send_AT_Command(AT_CMD);   //Send command to make a call
}

/**
  *@brief    Send Text Mesaage
  *@param    Message:  Text message to be sent
  *@param    phone_number : Phone number
  *@retval   None
*/
void GSM_Send_SMS(char* Message, char* phone_number)
{
	char SMS_AT_CMD1[] = "AT+CMGF=1\r";
	char SMS_AT_CMD2[21] = "AT+CMGS=+91";
	char CR[] = "\r";
	uint8_t MSG_END[] = {26};

	strcat(SMS_AT_CMD2, phone_number);
	strcat(SMS_AT_CMD2, CR);

	GSM_Send_AT_Command(SMS_AT_CMD1);
	Delay_ms(750);
	GSM_Send_AT_Command(SMS_AT_CMD2);
	Delay_ms(750);
	GSM_Send_AT_Command(Message);
	Delay_ms(500);
	GSM_Send_AT_Command((char*)MSG_END);

}

/**
  *@brief    Receive SMS , Stores Message sender number in "Incoming_SMS_Phone_Num" and
	           Incoming Message in "Incoming_SMS_Message"
  *@param    None
  *@retval   None
*/
void GSM_Receive_SMS(void)
{
	char temp_buffer[RX_BUFFER_SIZE];
	int i=0,j=0,k=0,l=0,m=0;

	//Store RX buffer value into temp buffer
	while(i<=127)
	{
		temp_buffer[i] = RX_Buffer[i];
		i++;
	}

	//string pattern to detect start of phone number
	char phone_pattern[2] = {'"',"+"};

	//search for phone number
	char* ptr = strstr(temp_buffer, phone_pattern);

	//store phone number
	for(j=0;j<13;j++)
		Incoming_SMS_Phone_Num[j] = ptr[1+j];

	//string pattern to detect start of message
	char sms_pattern[2] = {'6', '"'};

	//search for message
	ptr = strstr(temp_buffer, sms_pattern);

	for(k=0;k<4;k++)
	{
		ptr+=1; //increment pointer
		//check if it is pointing to end of buffer
		if(ptr == &temp_buffer[127])
		{
			if(k == 3)
			{
				Incoming_SMS_Message[l] = *(ptr+m);
				l++;
			}

			ptr = &temp_buffer[0]; //if yes,goto start of buffer
		}
	}

	//store message untill "\r" is found, which indicates the end of SMS
	while(*(ptr+m) !='\r')
	{
		if(ptr+m == &temp_buffer[127])
		{
			Incoming_SMS_Message[l] = *(ptr+m);
			ptr = &temp_buffer[0];
			m = 0;
		}
		else
		{
			Incoming_SMS_Message[l] = *(ptr+m);
			l++;
			m++;
		}
	}
}

/**
  *@brief    Initialize the GSM A6 Module
  *@param    None
  *@retval   None
*/
void GSM_Init(void)
{
	//initialize uart
	UART_Config();

	//initialize DMA
	DMA_Config();

	//start receiving data
	DMA_Rx();

	//wait 15-20 sec- let the gsm module get connected to network
	Delay_ms(20000);

	//send basic at command
	GSM_Send_AT_Command("AT\r");
	Delay_ms(300);

	//select sms message formate as text
	GSM_Send_AT_Command("AT+CMGF=1\r");
	Delay_ms(500);

	//specify how newly arrived sms message should be handled
	GSM_Send_AT_Command("AT+CNMI=1,2,0,0,0\r");
	Delay_ms(1000);

	GSM_Clear_RX_buffer();
}


/**
  *@brief    Clear GSM RX_buffer
  *@param    None
  *@retval   None
*/
void GSM_Clear_RX_buffer(void)
{
	int i;
	for(i=0;i<RX_BUFFER_SIZE;i++)
	{
		RX_Buffer[i] = '\r';
	}
}






















