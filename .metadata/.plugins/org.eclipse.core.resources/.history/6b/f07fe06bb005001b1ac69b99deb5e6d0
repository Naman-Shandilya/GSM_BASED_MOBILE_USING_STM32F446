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
			

int main(void)
{


	for(;;);
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
