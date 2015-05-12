/****************************************************************
 *
 * www.payitforward.edu.vn
 *
 ****************************************************************/
/****************************************************************
 *
 * PIF TIVA FW2 : State - Machine
 *
 ****************************************************************/

/****************************************************************
 *
 *	Module		: main.c
 *	Description	:
 *  Tool		: CCS 6.
 *	Chip		: TIVA C TM4C123G
 * 	History		: 2 / 5/2015
 *  Version     : 1.0
 *
 *	Author		: N.T.Khoa, Pay it Forward Club
 *	Notes		: 4 States
 *	____________________________________
 *	State    	|	Led 1	|	Led 2
 *	____________|___________|___________
 *	Soft-start	|Fast Blinky|	Off
 *	Idle    	|Slow Blinky|	Off
 *	Run	    	|Always On	|PWM Control
 *	Error    	|Fast Blinky|Fast Blinky
 *	____________|___________|___________
 *
 *								(flagError = Flase)				Push + Hold Button for 1s
 *	POWER ON  --->  Soft-start  ------------------->  Idle   <---------------------------->  RUN
 *														|									  |
 *						|	    						|Push							Push  |
 *	(flagError = True ) |								|Button2					  Button2 |
 *						|								|									  |
 *						v								|									  |
 *														|									  |
 *					  Error	  <-------------------------/									  |
 *					  		  <---------------------------------------------------------------/
 ****************************************************************/

#include "StateMachine.h"
#include "driverlib/fpu.h"
#include "driverlib/eeprom.h"
#include "CmdList.h"
#include "cmdline.h"
//*****************************************************************************
//
// Input buffer for the command line interpreter.
//
//*****************************************************************************
static char g_cInput[APP_INPUT_BUF_SIZE];
int32_t Eeprom_err; // error Eeprom in tiva C, not flagError

void Basic_Config(void){
	//
	// Enable lazy stacking for interrupt handlers.  This allows floating-point
	// instructions to be used within interrupt handlers, but at the expense of
	// extra stack usage.
	//

	FPUEnable();
	FPULazyStackingEnable();

	// Set the clocking to run from the PLL.

	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
			SYSCTL_OSC_MAIN);	// 200 : 5 = 40Mhz

	// Enable EEPROM

	SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);

	Eeprom_err =  EEPROMInit();

	// Enable Uart
	ConfigureUART();

	//Enable led1
	ConfigLED1();

	//Enable  PWM
	ConfigurePWM();

	//
	// Initialize the buttons
	//
	ButtonsInit();

	//Configure State Mode
	g_sAppState.ui32Mode = APP_MODE_SOFT_START;

	//Configure Status Button
	g_sAppState.ui32Buttons = 0;

	//Configure Duty Cycle of PWM
	ulDutyCycle = 0;

	g_sAppState.ui32ModeFree = 0;
	Do_one  = 1;
	//
	// Initialize the RGB LED. AppRainbow typically only called from interrupt
	// context. Safe to call here to force initial color update because
	// interrupts are not yet enabled.
	//
	//	RGBInit(0);
	//	RGBIntensitySet(g_sAppState.fIntensity);
	//AppRainbow(1);
	//	RGBEnable();

	//
	// Initialize the buttons
	//
	ButtonsInit();

	//
	// Initialize the SysTick interrupt to process colors and buttons.
	//
	SysTickPeriodSet(SysCtlClockGet() / APP_SYSTICKS_PER_SEC);
	SysTickEnable();
	SysTickIntEnable();
	IntMasterEnable();
}


int main(void) {
	int32_t i32CommandStatus;

	Basic_Config();

	UARTprintf("\n       TUTORIAL FW_2 : State - Machine %s \n");

	UARTprintf("## Eeprom Tiva C : eeprom status %s, sizes %d bytes, blocks %d\n",
			Eeprom_err?"Bad":"Ok",  EEPROMSizeGet(),  EEPROMBlockCountGet());
	UARTprintf("Type 'help' for a list of commands\n");

	UARTprintf("Enter MODE_SOFT_START\n");
	//	pui32Data[0] = Status_ERROR;
	//	EEPROMProgram(pui32Data, 0x00, sizeof(pui32Data));
	delay_ms(10000);
	// Check flagError
	if(g_sAppState.ui32Mode == APP_MODE_SOFT_START){
		EEPROMRead(pui32Read, 0x000, sizeof(pui32Read));

		switch (pui32Read[0]){
		case Status_ERROR:
			// Enter Mode ERROR
			UARTprintf("> flagError = 1\n");
			UARTprintf("> Enter MODE_ERROR\n");
			g_sAppState.ui32Mode = APP_MODE_ERROR;
			break;
		case Status_OK:
			// Enter Mode IDLE
			UARTprintf("> flagError = 0\n");
			UARTprintf("> Enter MODE_IDLE\n");
			g_sAppState.ui32Mode = APP_MODE_IDLE;
			break;
		default:
			UARTprintf("> Don't Know , eeprom have not written yet\n");
			break;
		}
	}
	//	UARTprintf("\n## %s - %s \n",
	//			pui32Data[0],pui32Read[0]);
	while(1) {
		UARTprintf("\n>");
		//
		// Peek to see if a full command is ready for processing
		//
		while(UARTPeek('\r') == -1)
		{
			//
			// millisecond delay.  A SysCtlSleep() here would also be OK.
			//
			SysCtlDelay(SysCtlClockGet() / (1000 / 3));
			//
			// Check for change of mode and enter hibernate if requested.
			// all other mode changes handled in interrupt context.
			//
		}

		//
		// a '\r' was detected get the line of text from the user.
		//
		UARTgets(g_cInput,sizeof(g_cInput));
		i32CommandStatus = CmdLineProcess(g_cInput);

		switch (i32CommandStatus) {
		case CMDLINE_BAD_CMD:
			UARTprintf("Bad command!\n");
			break;
		case CMDLINE_TOO_MANY_ARGS:
			UARTprintf("Too many arguments for command processor!\n");
			break;
		case CMDLINE_TOO_FEW_ARGS:
			UARTprintf("Too few arguments for command processor!\n");
			break;
		case CMDLINE_INVALID_ARG:
			UARTprintf("Invalid argument for command processor!\n");
			break;
		}
	}
}
