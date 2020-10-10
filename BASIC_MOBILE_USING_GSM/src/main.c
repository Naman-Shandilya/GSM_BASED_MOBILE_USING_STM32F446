/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "STM32F446_I2C_LCD_Driver.h"

#include"STM32F446_KEYPAD_DRIVER.h"
#include"GSM_DRIVER.h"

//Incoming SMS are stored in "Incoming_SMS_Message", which is declared in GSM driver
extern char Incoming_SMS_Message[100];

//Function prototypes of mobile phone specific functions
void Phone_Home_Display(void);
void Phone_Make_Call(void);
void Phone_Send_SMS(void);
void Phone_Receive_SMS(void);
void Phone_Receive_Call(void);
void Store_Phone_Number(char First_KeyPress_Val);

//Function prototypes related to mcu
void systickInit(uint16_t freq);
void Delay_ms(uint32_t time);

//variable declaration
volatile uint32_t ticks=0;

//char array to store the phone number entered via keypad
char phone_num[10];


int main(void)
{
	SystemInit();

	for(;;);
}


/********************************** Mobile Phone Specific APIs - Start *************************************/


/**
 * @brief  Displays the start/home screen on LCD
 * @param  none
 * @retval None
 */
void Phone_Home_Display(void)
{
	LCD_Clear_Then_Display("Press 1 to Call");
	LCD_Send_String_On_Line2("Press 2 to SMS");
}

/**
 * @brief  This functiond Handles the "Making a call" functionality
 * @param  none
 * @retval None
 */
void Phone_Make_Call(void)
{
	//Pattern match for call receive & call end, these string pattern indicates whether call is ended or received
	char call_received[5] ={'R','"',',','0'};
	char call_received_test2[8] ={'C','A','L','L','"',',','l'};
	char call_end[6] ="ERROR";

	LCD_Clear_Then_Display("Enter Phone no:");
	LCD_Send_String_On_Line2("or Press B:Exit");

	char key= KEYPAD_NOT_PRESSED;
	//check key press continuously

	while(key == KEYPAD_NOT_PRESSED)
	{
		key = KEYPAD_Read();
		Delay_ms(100);
	}

	//Now check which key is pressed
	if(key == 'B')
	{
		//If so, exit
		GSM_Clear_RX_buffer();
		Phone_Home_Display();
		return;
	}
	else
	{
		//store phone number
		Store_Phone_Number(key);
		//ask user "Press C" in order to Call
		key = KEYPAD_NOT_PRESSED;

		LCD_Send_String_On_Line2("Press C to call");

		while(key == KEYPAD_NOT_PRESSED)
		{
			key = KEUPAD_Read();
			Delay_ms(100);
		}

		//if c is pressed make a call else exit
		if(key == 'C')
		{
			LCD_Clear_Then_Display("Calling........");
			LCD_Send_String_On_Line2(phone_num);

			//CALL GSM_Make_Call function with phone number as parameter
			GSM_Make_Call(phone_num);

			//to display counting integer- time elapsed(second)
			int i=0, num=0, j, k;
			int digit[4] ={0};
			char characters[5] ="0000";

			char key = KEYPAD_NOT_PRESSED;
			delay_ms(250);
			while(1)
			{
				//check whether call received, and call is in progress
				if(GSM_Compare_GSMData_With(call_received) && GSM_Compare_GSMData_With(call_received_test2))
				{
					//if yes, the display integer as time elapsed
					i=num;
					LCD_Clear_Then_Display(characters);
					j=3,k=0;
					while(i>0)
					{
						digit[j--] = i%10;
						i = i/10;
					}
					while(k<4)
					{
						characters[k] = digit[k]+'0';
						k++;
					}

					num++;
					Delay_ms(600);
				}
				/*If at any momeny, "B" is pressed then cut the call*/
				key = KEYPAD_Read();
				if(key != KEYPAD_NOT_PRESSED)
				{
					if(key == 'B')
				{
					 GSM_HangUp_Call();
					 LCD_Clear_Then_Display("Call Ended!!");
					 GSM_Clear_RX_buffer();
					 Delay_ms(1500);
					  Phone_Home_Display();
					  return;
								 }

							 }
				/*Check if call endede, if yes display same and exit*/
				if(GSM_Compare_GSMData_With(call_end))
				{
					LCD_Clear_Then_Display("Call Ended!!");
					GSM_Clear_RX_buffer();
					Delay_ms(1500);
					Phone_Home_Display();
					return;
				}
				Delay_ms(100);
			}

	 }
	 else
	 {
			LCD_Clear_Then_Display("Invalid input");
			Phone_Home_Display();
			return;
	  }




	}
}

/**
 * @brief  This function handles sending an sms
 * @param  none
 * @retval None
 */
void Phone_Send_SMS(void)
{
	LCD_Clear_Then_Display("Enter Phone no:");
	LCD_Send_String_On_Line2("or Press B:Exit");

	char key = KEYPAD_NOT_PRESSED;

	/*Check Key press continoulsy */
	while(key == KEYPAD_NOT_PRESSED)
	{
		key = KEYPAD_Read();
		Delay_ms(100);
	}

	/* Now Check which key is pressed */
	if(key == 'B')
	{
		/*If yes, the exit*/
		Phone_Home_Display();
		return;
	}
	else
		{
			/*Store Phone number */
			 Store_Phone_Number(key);

			/* ASk user "Press C" in order to Send SMS */
			key = KEYPAD_NOT_PRESSED;

			LCD_Send_String_On_Line1("Press C to Send");
			LCD_Send_String_On_Line2("Message :-)");

			while(key == KEYPAD_NOT_PRESSED)
			{
				key = KEYPAD_Read();
				Delay_ms(100);
			}

			/* If C is pressed Send SMS*/
			if(key == 'C')
			{
				LCD_Clear_Then_Display("Sending SMS......");
				LCD_Send_String_On_Line2("Test Message");
				GSM_Send_SMS("Test Message",phone_num); //As of now hardcoded SMS is sent.
				Delay_ms(1000);
				LCD_Clear_Then_Display("Message Sent");
				Delay_ms(900);
				GSM_Clear_RX_buffer();
				Phone_Home_Display();
				return;
			}
			else
					{
						LCD_Clear_Then_Display("Invalid input");
						Phone_Home_Display();
						return;
					}
		}
}


/**
 * @brief  This function handles the "Call receiving" Functionality
 * @param  none
 * @retval None
 */
void Phone_Receive_Call(void)
{
	/*Pattern match for whether call is received successfully, call ended*/
	char call_received[8] ="CONNECT";
	char call_end[6]="ERROR";

	LCD_Clear_Then_Display("Incoming Call...");
	LCD_Send_String_On_Line2("C:Recieve B:Hang");

	char key = KEYPAD_NOT_PRESSED;

	/*Check Key press continoulsy */
	while(key == KEYPAD_NOT_PRESSED)
	{
		key = KEYPAD_Read();
		Delay_ms(100);
	}

	while(1)
	{
		/* Now Check which key is pressed */
	  if(key == 'B')
	  {
			GSM_HangUp_Call();
		  GSM_Clear_RX_buffer();
		  Phone_Home_Display();
		  return;
 	  }

    if(key == 'C')
		{
			GSM_Receive_Call();
		  /* To display counting integer - time elapsed(seconnds)*/
		  int i=0,num=0,j,k;
		  int digit[4] ={0};
	    char character[5]= "0000";

		  char key = KEYPAD_NOT_PRESSED;
		  Delay_ms(250);

		  while(1)
			{
				/*Check if call received or not*/
			  if(GSM_Compare_GSMData_With(call_received))
				{
					/*If yes, then display time elapsed */
				  i = num;
				  LCD_Clear_Then_Display(character);

				  j=3,k=0;
				  while(i>0)
					{
						digit[j--] = i%10;
					  i = i/10;
				  }

				  while(k<4)
				  {
						character[k] = digit[k]+'0';
					  k++;
					}

				  num++;
				  Delay_ms(650);
			  }

			  /*If at any momeny, "B" is pressed then cut the call*/
			  key = KEYPAD_Read();
			  if(key != KEYPAD_NOT_PRESSED)
				{
					if(key == 'B')
					{
						GSM_HangUp_Call();
					  LCD_Clear_Then_Display("Call Ended!!");
				    GSM_Clear_RX_buffer();
				    Delay_ms(1500);
				    Phone_Home_Display();
				    return;
				  }

			  }

			  /*Check if call ended, if so display same and exit*/
			  if(GSM_Compare_GSMData_With(call_end))
				{
					LCD_Clear_Then_Display("Call Ended!!");
				  GSM_Clear_RX_buffer();
				  Delay_ms(1500);
				  Phone_Home_Display();
				  return;
			  }

        Delay_ms(100);

	    }

    }

  }

}


/**
 * @brief  This function handles the "Receiving an Incoming SMS" Functionality
 * @param  none
 * @retval None
 */
void Phone_Receive_SMS(void)
{
	GSM_Receive_SMS();

	LCD_Clear_Then_Display("You have 1 SMS");
	LCD_Send_String_On_Line2("C:Read  B:Exit");

	char key = KEYPAD_NOT_PRESSED;

	/*Check Key press continoulsy */
	while(key == KEYPAD_NOT_PRESSED)
	{
		key = KEYPAD_Read();
		Delay_ms(100);
	}

	/* Now Check which key is pressed */
	if(key == 'B')
	{
		GSM_Clear_RX_buffer();
		Phone_Home_Display();
		return;
	}
	else if(key == 'C')
	{
	 LCD_Display_Long_Message(Incoming_SMS_Message);
	 Delay_ms(4000);
	 GSM_Clear_RX_buffer();
	 Phone_Home_Display();
	 return;
  }
	else
	{
		Phone_Receive_SMS();
	}

}
/***************************************************************************************************/

/**
 * @brief  Storing the phone number
 * @param  none
 * @retval None
 */
void Store_Phone_Number(char First_KeyPress_Val)
{
	  /*Clear Phone_num[] array */
	  for(int i = 0;i<10;i++)
	     phone_num[i] = '\0';

		char key = First_KeyPress_Val;
	  int phone_num_count = 0;

		/* Store the 1st digit */
		phone_num[phone_num_count] = key;

		phone_num_count++;
		LCD_Clear_Then_Display(phone_num);

		/*Now store the rest 9 digits*/
		while(phone_num_count <10)
		{
			key = KEYPAD_Read();
			if(key != KEYPAD_NOT_PRESSED)
			{
				phone_num[phone_num_count] = key;
				phone_num_count++;
				/*LCD_Clear_Then_Display(phone_num);*/
				LCD_Send_Data(key); //Display the key entered
				Delay_ms(100);
			}
			Delay_ms(100);
		}

}

/*
 * Sets system clock frequency to 24MHz and configure the HCLK,PCLK1,PCLK2 prescalers
 */

void SetSysClockTo24(void)
{
	//SYSCLK,HCLK,PCLK1,PCLK2 CONFIGURATION
	//1.RESET THE RCC SYSTEM -HSI:ON,HSE:OFF,PLL:OFF
	RCC_DeInit();

	//2.ENABLE HSE
	RCC_HSEConfig(RCC_HSE_ON);

	//3.WAIT TILL HSE READY
	while(RCC_WaitForHSEStartUp()!=SUCCESS);

	if(RCC_WaitForHSEStartUp())
	{
		//HSE OSC STARTS SUCCESSUFLLY

		//4.NOW TIME TO SET THE FLASH LATENCY
		FLASH_SetLatency(FLASH_Latency_0);

		//5.NOW SET THE HCLK PRESCALER AS SUCH HCLK=SYSCLK(AS PER APPLICATION)
		RCC_HCLKConfig(RCC_HCLK_Div1);

		//6.NOW SET THE PCLK2 PRESCALER AS SUCH PCLK2=SYSCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);

		//7.NOW SET THE PCLK1 PRESCALER AS SUCH PCLK1=SYSCLK
		RCC_PCLK1Config(RCC_HCLK_Div1);

		/*THE POINT TO BE REMEMBER IS THAT IF WE USE PLL AS OUR SYSCLK SOURCE
		 * THEN FIRST CONFIGURE THE PRESCALER PART OR THE CACLCULATION PART
		 * FOR HCLK,PCLK2,PCLK1 BEFORE ENABLING THE CLOCK SOURCE
		 */
		//8.NOW CONFIGURE THE PLLCLK=24MHz ->(8MHz/2)*6
		RCC_PLLConfig(RCC_PLLSource_HSI,4,72,6,4,6);
		//9.NOW ENABLE THE PLL
		RCC_PLLCmd(ENABLE);

		//10.WAIT TILL PLL IS READY
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET)
		{

		}
		//10.NOW THIS IS THE TIME TO SELECT THE SYSTEM CLOCK SOURCE
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		//11.WAIT TILL PLL IS USED AS SYSTEM CLOCK SOURCE
		while(RCC_GetSYSCLKSource()!=0x08)
		{

		}
		}
	else
	{
		//IF HSE FAILS TO START THE APPLICATION WILL HAVE WRONG CLOCK CONFIGURATION
		//GO TO INFINITE LOOP
		while(1);
	}

}



void systickInit(uint16_t freq)
{
	RCC_ClocksTypeDef clk_freq;
	RCC_GetClocksFreq(&clk_freq);
	uint32_t HCLK_Freq = clk_freq.HCLK_Frequency /freq;
	uint32_t SysTick_Config(HCLK_Freq);

}

inline void SysTick_Handler(void)
{
	//when this service routine called the value of tick variable inc. by 1
	ticks++;
}

/*
 * Now you have a variable that increments automatically at a regular interval it is
 * tempting to use it directly when measuring off time. However, it is probably better
 * to write an accessor function that is more explicit in its purpose. Here,
 * the function millis() just returns the tick count because there is a 1:1 correspondence
 * between tick and milliseconds. In another system, you might want to run systick at some
 * other frequency but still have millis() give you the number of milliseconds since reset. So millis() is just:
 */

uint32_t millis(void)
{
	return ticks;
}


// delay function
/*
void Delay_ms(uint32_t time)
{
	uint32_t start, end;
	start = millis();
	end = start + time;
	if(start < end)
	{
		while((millis()>= start) && (millis() < end))
		{
			//do nothing
		}
	}
		else
		{
			while((millis() >= start) || (millis() < end))
			{
				//do nothing
			}
		}

}
*/

// replacement of delay

void Delay_ms(uint32_t time)
{
	uint32_t elapsed;
	uint32_t start = millis();
	do{
		elapsed = millis() - start;
	}while(elapsed < time);

}
