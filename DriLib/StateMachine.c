/*
 * UserConfig.c
 *
 *  Created on: 03-05-2015
 *      Author: hp
 */

#include "StateMachine.h"
#include "driverlib/rom.h"
#include "driverlib/eeprom.h"

//*****************************************************************************
//
// Application state structure.
//
//*****************************************************************************
volatile tAppState g_sAppState;
uint32_t pui32Data[1];
uint32_t pui32Read[1];
//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************

void ConfigureUART(void)
{
	//
	// Enable the GPIO Peripheral used by the UART.
	//
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	//
	// Enable UART0
	//
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	//
	// Configure GPIO Pins for UART mode.
	//
	ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
	ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//
	// Use the internal 16MHz oscillator as the UART clock source.
	//
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

	//
	// Initialize the UART for console I/O.
	//
	UARTStdioConfig(0, 115200, 16000000);
}

//**************************************************************************
//
//
//**************************************************************************

volatile unsigned long ulDutyCycle = 0;//Duty cycle of PWM
volatile unsigned long ulPWMPeriod;//Period of PWM
volatile unsigned long ulPWMClock;//Period of PWM

void ConfigurePWM(void){

	//Configure PWM clock to match system
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);  //PWM clocks 40MHz : 64 = 625000

	//Enable the peripherals used by this program.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);//Tiva Launchpad has 2 modules (0 and 1) and module 1 covers led pins
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	ulPWMClock = SysCtlClockGet()/64;
	ulPWMPeriod = ( ulPWMClock/PWMFrequency ) - 1;

	//Configure LED pins as PWM
#ifdef LED_2_RED
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);

	//Configure PWM Options
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ulPWMPeriod);//PWM frequency = 200Hz

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ulPWMPeriod*ulDutyCycle/100);

	//Turn on the Output pins
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);

	//Enable the PWM generator
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
#endif
#ifdef LED_2_BLUE
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);

	//Configure PWM Options
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ulPWMPeriod);//PWM frequency = 200Hz

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ulPWMPeriod*ulDutyCycle/100);

	//Turn on the Output pins
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);

	//Enable the PWM generator
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);
#endif
#ifdef LED_2_GREEN
	GPIOPinConfigure(GPIO_PF3_M1PWM7);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);

	//Configure PWM Options
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ulPWMPeriod);//PWM frequency = 200Hz

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ulPWMPeriod*ulDutyCycle/100);

	//Turn on the Output pins
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);

	//Disable the PWM generator
	PWMGenDisable(PWM1_BASE, PWM_GEN_2);
	//PWMGenEnable(PWM1_BASE, PWM_GEN_3);
#endif
}
//*****************************************************************************
//
// Handler to manage the button press events and state machine transitions
// that result from those button events.
//
// This function is called by the SysTickIntHandler if a button event is
// detected. Function will determine which button was pressed and tweak various
// elements of the global state structure accordingly.
//
//*****************************************************************************
uint8_t Do_one ;
void AppButtonHandler(void)
{
	static uint32_t ui32TickCounter;

	ui32TickCounter++;

	//
	// Switch statement to adjust the color wheel position based on buttons
	//
	switch(g_sAppState.ui32Buttons & ALL_BUTTONS)
	{

	case BUTTON_1:
	{
		//**********************
		// MODE IDLE
		//**********************
		if(g_sAppState.ui32Mode == APP_MODE_IDLE){

			//
			// Check if the button has been held enough to peform
			//

			if(g_sAppState.ui32ModeTimer < (Times_Hold_Button*APP_SYSTICKS_PER_SEC))
			{
				g_sAppState.ui32ModeTimer++;
			}
			//			else {
			//				//
			//				// Enter Mode RUN
			//				//
			//				g_sAppState.ui32ModeTimer = 0;
			//				g_sAppState.ui32Mode = APP_MODE_RUN;
			//				UARTprintf("\n> Enter MODE_RUN\n");
			//				UARTprintf(">");
			//				Do_one = 1;
			//			}
		}
		//**********************
		// MODE RUN
		//**********************
		if(g_sAppState.ui32Mode == APP_MODE_RUN){

			//
			// Check if the button has been held enough to peform
			//

			if(g_sAppState.ui32ModeTimer < (Times_Hold_Button*APP_SYSTICKS_PER_SEC))
			{
				g_sAppState.ui32ModeTimer++;
			}
			//			else {
			//				//
			//				// Enter Mode IDLE
			//				//
			//				g_sAppState.ui32ModeTimer = 0;
			//				g_sAppState.ui32Mode = APP_MODE_IDLE;
			//				UARTprintf("\n> Enter MODE_IDLE\n");
			//				UARTprintf(">");
			//				Do_one = 1;
			//			}
		}

		//**********************
		// MODE ERROR
		//**********************
		if(g_sAppState.ui32Mode == APP_MODE_ERROR){

			//
			// Check if the button has been held enough to peform
			//

			if((ui32TickCounter % APP_BUTTON_POLL_DIVIDER) == 0)
			{
				//
				// Perform the increment and
				// Enter Mode IDLE
				//
				g_sAppState.ui32ManualIndex++;
				//				if(g_sAppState.ui32ManualIndex >= Push_Button_Counter)
				//				{
				//					g_sAppState.ui32Mode = APP_MODE_IDLE;
				//					UARTprintf("\n> Enter MODE_IDLE\n");
				//					UARTprintf(">");
				//					Do_one = 1;
				//					//write Data to Eeprom : Status_OK
				//					pui32Data[0] = Status_OK;
				//					EEPROMProgram(pui32Data, 0x00, sizeof(pui32Data));
				//					g_sAppState.ui32ManualIndex = 0;
				//				}
			}
		}
		g_sAppState.ui32ModeFree = 0 ;
		break;
	}
	case BUTTON_2:
	{
		//
		// Check if the button has been held int32_t enough to perform another
		// color wheel decrement.
		//
		if((g_sAppState.ui32Mode == APP_MODE_IDLE)||(g_sAppState.ui32Mode == APP_MODE_RUN)){

			//
			// Check if the button has been held enough to peform
			//

			if((ui32TickCounter % APP_BUTTON_POLL_DIVIDER) == 0)
			{
				//**********************
				// Enter Mode ERROR
				//**********************
				g_sAppState.ui32Mode = APP_MODE_ERROR;
				UARTprintf("\n> Enter MODE_ERROR\n");
				UARTprintf(">");
				ulDutyCycle = 100;

				//write Data to Eeprom : Status_ERROR
				pui32Data[0] = Status_ERROR;
				EEPROMProgram(pui32Data, 0x00, sizeof(pui32Data));
				Do_one = 1;
			}
		}
		g_sAppState.ui32ModeFree = 0 ;
		break;
	}
	default:
		// Mode Error :  after press enough and free the button
		if(g_sAppState.ui32Mode == APP_MODE_ERROR){
			if(g_sAppState.ui32ManualIndex >= Push_Button_Counter)
			{
				g_sAppState.ui32Mode = APP_MODE_IDLE;
				UARTprintf("\n> Enter MODE_IDLE\n");
				UARTprintf(">");
				Do_one = 1;
				//write Data to Eeprom : Status_OK
				pui32Data[0] = Status_OK;
				EEPROMProgram(pui32Data, 0x00, sizeof(pui32Data));
				g_sAppState.ui32ManualIndex = 0;
			}
		}
		// Mode RUN :  after hold button enough and free the button
		else if(g_sAppState.ui32Mode == APP_MODE_RUN) {
			if(g_sAppState.ui32ModeTimer >= (Times_Hold_Button*APP_SYSTICKS_PER_SEC)){
				//
				// Enter Mode IDLE
				//
				g_sAppState.ui32ModeTimer = 0;
				g_sAppState.ui32Mode = APP_MODE_IDLE;
				UARTprintf("\n> Enter MODE_IDLE\n");
				UARTprintf(">");
				Do_one = 1;
			}
		}
		// Mode IDLE :  after hold button enough and free the button
		else if(g_sAppState.ui32Mode == APP_MODE_IDLE) {
			if(g_sAppState.ui32ModeTimer >= (Times_Hold_Button*APP_SYSTICKS_PER_SEC)){
				//
				// Enter Mode RUN
				//
				g_sAppState.ui32ModeTimer = 0;
				g_sAppState.ui32Mode = APP_MODE_RUN;
				UARTprintf("\n> Enter MODE_RUN\n");
				UARTprintf(">");
				Do_one = 1;
			}
		}

		//		//
		//		// reset the tick counter when no buttons are pressed
		//		// this makes the first button reaction speed quicker
		//		//
		if(g_sAppState.ui32ModeFree < APP_AUTO_MODE_TIMEOUT)
		{
			g_sAppState.ui32ModeFree++;
		}
		else
		{
			//**********************
			// MODE ERROR
			//**********************
			if(g_sAppState.ui32Mode == APP_MODE_ERROR){
				g_sAppState.ui32ManualIndex = 0;
			}
			//
			// reset the tick counter when no buttons are pressed
			// this makes the first button reaction speed quicker
			//
			ui32TickCounter = APP_BUTTON_POLL_DIVIDER - 1;
		}
		break;
	}
}
//**************************************************************************
//
//
//**************************************************************************

uint8_t Period_Toggle;

void AppLedHandler(void){
	static uint8_t Status_Led;

	//**********************
	// MODE SOFT_START
	//**********************
	if(g_sAppState.ui32Mode == APP_MODE_SOFT_START){
		//
		// Wait until enough period to toggle Led
		//
		if(Period_Toggle > (Period_Led_Toggle_Fast*APP_SYSTICKS_PER_SEC)){
			// Change Led Status
			Status_Led ^= 1;
			//
			// LED1 FAST BLINKY
			//
#ifdef LED_1_RED
			Status_Led ? Led_On(LED_1_RED)   : Led_Off(LED_1_RED)   ;
#endif
#ifdef LED_1_BLUE
			Status_Led ? Led_On(LED_1_BLUE)  : Led_Off(LED_1_BLUE)  ;
#endif
#ifdef LED_1_GREEN
			Status_Led ? Led_On(LED_1_GREEN) : Led_Off(LED_1_GREEN) ;
#endif
			Period_Toggle = 0;
		}

		//
		// LED2 OFF
		//
		//Config Led 2 to GPIO
		if (Do_one == 1){
			//
			// Turn off PWM
			//
#ifdef LED_2_RED
			PWMGenDisable(PWM1_BASE, PWM_GEN_2);
#endif
#ifdef LED_2_BLUE
			PWMGenDisable(PWM1_BASE, PWM_GEN_3);
#endif
#ifdef LED_2_GREEN
			PWMGenDisable(PWM1_BASE, PWM_GEN_3);
#endif

			ulDutyCycle = 0;
			//
			// Config LED 2 to GPIO
			//
#ifdef LED_2_RED
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_2_RED );
			Led_Off(LED_2_RED);
#endif
#ifdef LED_2_BLUE
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_2_BLUE );
			Led_Off(LED_2_BLUE);
#endif
#ifdef LED_2_GREEN
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_2_GREEN );
			Led_Off(LED_2_GREEN);
#endif
			Do_one = 0;
		}
	}


	//**********************
	// MODE IDLE
	//**********************
	if(g_sAppState.ui32Mode == APP_MODE_IDLE){
		//
		// Wait until enough period to toggle Led
		//
		if(Period_Toggle > (Period_Led_Toggle_Slow*APP_SYSTICKS_PER_SEC)){
			// Change Led Status
			Status_Led ^= 1;
			//
			// LED1 SLOW BLINKY
			//
#ifdef LED_1_RED
			Status_Led ? Led_On(LED_1_RED)   : Led_Off(LED_1_RED)   ;
#endif
#ifdef LED_1_BLUE
			Status_Led ? Led_On(LED_1_BLUE)  : Led_Off(LED_1_BLUE)  ;
#endif
#ifdef LED_1_GREEN
			Status_Led ? Led_On(LED_1_GREEN) : Led_Off(LED_1_GREEN) ;
#endif
			Period_Toggle = 0;
		}

		//
		// LED2 OFF
		//
		if (Do_one == 1){
			//
			// Turn off PWM
			//
#ifdef LED_2_RED
			PWMGenDisable(PWM1_BASE, PWM_GEN_2);
#endif
#ifdef LED_2_BLUE
			PWMGenDisable(PWM1_BASE, PWM_GEN_3);
#endif
#ifdef LED_2_GREEN
			PWMGenDisable(PWM1_BASE, PWM_GEN_3);
#endif

			ulDutyCycle = 0;
			//
			// Config LED 2 to GPIO
			//
#ifdef LED_2_RED
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_2_RED );
			Led_Off(LED_2_RED);
#endif
#ifdef LED_2_BLUE
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_2_BLUE );
			Led_Off(LED_2_BLUE);
#endif
#ifdef LED_2_GREEN
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_2_GREEN );
			Led_Off(LED_2_GREEN);
#endif
			Do_one = 0;
		}
	}


	//**********************
	// MODE ERROR
	//**********************
	if(g_sAppState.ui32Mode == APP_MODE_ERROR){
		// Congfig Led 2 to GPIO
		if (Do_one == 1){
			//
			// Turn off PWM
			//
#ifdef LED_2_RED
			PWMGenDisable(PWM1_BASE, PWM_GEN_2);
#endif
#ifdef LED_2_BLUE
			PWMGenDisable(PWM1_BASE, PWM_GEN_3);
#endif
#ifdef LED_2_GREEN
			PWMGenDisable(PWM1_BASE, PWM_GEN_3);
#endif

			ulDutyCycle = 0;
			//
			// Config LED 2 to GPIO
			//
#ifdef LED_2_RED
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_2_RED );
			Led_Off(LED_2_RED);
#endif
#ifdef LED_2_BLUE
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_2_BLUE );
			Led_Off(LED_2_BLUE);
#endif
#ifdef LED_2_GREEN
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_2_GREEN );
			Led_Off(LED_2_GREEN);
#endif
			Do_one = 0;
		}
		//
		// Wait until enough period to toggle Led
		//
		if(Period_Toggle > (Period_Led_Toggle_Fast*APP_SYSTICKS_PER_SEC)){

			// Change Led Status
			Status_Led ^= 1;

			//
			// LED1 FAST BLINKY
			//
#ifdef LED_1_RED
			Status_Led ? Led_On(LED_1_RED)   : Led_Off(LED_1_RED)   ;
#endif
#ifdef LED_1_BLUE
			Status_Led ? Led_On(LED_1_BLUE)  : Led_Off(LED_1_BLUE)  ;
#endif
#ifdef LED_1_GREEN
			Status_Led ? Led_On(LED_1_GREEN) : Led_Off(LED_1_GREEN) ;
#endif

			Period_Toggle = 0;

			//
			// LED2 FAST BLINKY
			//
#ifdef LED_2_RED
			Status_Led ? Led_On(LED_2_RED)   : Led_Off(LED_2_RED)   ;
#endif
#ifdef LED_2_BLUE
			Status_Led ? Led_On(LED_2_BLUE)  : Led_Off(LED_2_BLUE)  ;
#endif
#ifdef LED_2_GREEN
			Status_Led ? Led_On(LED_2_GREEN) : Led_Off(LED_2_GREEN) ;
#endif
		}

	}


	//**********************
	// MODE RUN
	//**********************
	if(g_sAppState.ui32Mode == APP_MODE_RUN)
	{
		if (Do_one == 1){
			//Configure LED pins as PWM
#ifdef LED_2_RED
			GPIOPinConfigure(GPIO_PF1_M1PWM5);
			GPIOPinTypePWM(GPIO_PORTF_BASE, LED_2_RED);
#endif
#ifdef LED_2_BLUE
			GPIOPinConfigure(GPIO_PF2_M1PWM6);
			GPIOPinTypePWM(GPIO_PORTF_BASE, LED_2_BLUE);
#endif
#ifdef LED_2_GREEN
			GPIOPinConfigure(GPIO_PF3_M1PWM7);
			GPIOPinTypePWM(GPIO_PORTF_BASE, LED_2_GREEN);
#endif
		}

		//
		// LED1 ON
		//
#ifdef LED_1_RED
		Led_On(LED_1_RED);
#endif
#ifdef LED_1_BLUE
		Led_On(LED_1_BLUE);
#endif
#ifdef LED_1_GREEN
		Led_On(LED_1_GREEN);
#endif


		//
		// LED2 PWM
		//
		//Load new value for ulDutyCycle
#ifdef LED_2_RED
		PWMGenDisable(PWM1_BASE, PWM_GEN_2);
#endif
#ifdef LED_2_BLUE
		PWMGenDisable(PWM1_BASE, PWM_GEN_3);
#endif
#ifdef LED_2_GREEN
		PWMGenDisable(PWM1_BASE, PWM_GEN_3);
#endif
		if(ulDutyCycle>=100)
		{
			ulDutyCycle=0;
		}
		else ulDutyCycle = ulDutyCycle + 1;
#ifdef LED_2_RED
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,ulPWMPeriod*ulDutyCycle/100);
		PWMGenEnable(PWM1_BASE, PWM_GEN_2);
#endif
#ifdef LED_2_BLUE
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,ulPWMPeriod*ulDutyCycle/100);
		PWMGenEnable(PWM1_BASE, PWM_GEN_3);
#endif
#ifdef LED_2_GREEN
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,ulPWMPeriod*ulDutyCycle/100);
		PWMGenEnable(PWM1_BASE, PWM_GEN_3);
#endif
	}
}


//*****************************************************************************
//
// Called by the NVIC as a result of SysTick Timer rollover interrupt flag
//
// Checks buttons and calls AppButtonHandler to manage button events.
// Tracks time and auto mode color stepping.  Calls AppRainbow to implement
// RGB color changes.
//
//*****************************************************************************

void
SysTickIntHandler(void)
{
	Period_Toggle++;

	g_sAppState.ui32Buttons = ButtonsPoll(0,0);
	AppButtonHandler();
	AppLedHandler();
}

//**************************************************************************
//
//
//**************************************************************************
void ConfigLED1(void){
	//config led1
#ifdef LED_1_RED
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_1_RED );
#endif
#ifdef LED_1_BLUE
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_1_BLUE );
#endif
#ifdef LED_1_GREEN
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_1_GREEN);
#endif
}

//**************************************************************************
//
//
//**************************************************************************
void delay_ms(uint16_t count){
	ROM_SysCtlDelay(count*SysCtlClockGet()/3000);
}
