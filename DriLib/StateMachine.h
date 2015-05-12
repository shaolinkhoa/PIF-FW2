/*
 * Includess.h
 *
 *  Created on: 02-05-2015
 *      Author: hp
 */

#ifndef INCLUDESS_H_
#define INCLUDESS_H_

/****************************************************************
 * IMPORT
 ****************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"
#include "uartstdio.h"
//#include "rgb.h"
#include "buttons.h"

//*****************************************************************************
//
// Globally defined constants
//
//*****************************************************************************
#define Status_ERROR					420551060
#define Status_OK						67246484

#define APP_SYSTICKS_PER_SEC            32 // tan` so' cua sys tick
#define APP_BUTTON_POLL_DIVIDER          8 // wait (1/APP_SYSTICKS_PER_SEC)*8  = 0.25s
#define APP_NUM_MANUAL_COLORS            7 //  7 mau`
#define APP_AUTO_MODE_TIMEOUT            (APP_SYSTICKS_PER_SEC * 1) // *COUNT(s)

#define APP_MODE_NORMAL                  0
#define APP_MODE_SOFT_START				 1
#define APP_MODE_IDLE					 2
#define APP_MODE_ERROR					 3
#define APP_MODE_RUN					 4

#define APP_INPUT_BUF_SIZE              128

#define BUTTON_1						LEFT_BUTTON
#define BUTTON_2						RIGHT_BUTTON

//******** Define LED_2 **********
#define LED_1_RED 		GPIO_PIN_1
//#define LED_1_BLUE	GPIO_PIN_2
//#define LED_1_GREEN	GPIO_PIN_3
//********************************

//******** Define LED_2 **********
//#define LED_2_RED		GPIO_PIN_1
#define LED_2_BLUE	GPIO_PIN_2
//#define LED_2_GREEN		GPIO_PIN_3
//********************************

//Config PWM
#define PWMFrequency					200 //Hz

#define Period_Led_Toggle_Fast			0.25 //s
#define Period_Led_Toggle_Slow			1 //s

#define Times_Hold_Button				1 //s
#define Push_Button_Counter				2 // counter push to change from mode Error to Idle

#define Led_On(PIN)		GPIOPinWrite(GPIO_PORTF_BASE,PIN,PIN)
#define Led_Off(PIN)	GPIOPinWrite(GPIO_PORTF_BASE,PIN,0)

//*****************************************************************************
//
// Structure typedef to make storing application state data to and from the
// hibernate battery backed memory simpler.
//      ui32Colors:       [R, G, B] range is 0 to 0xFFFF per color.
//      ui32Mode:         The current application mode, system state variable.
//      ui32Buttons:      bit map representation of buttons being pressed
//      ui32ManualIndex:  Control variable for count press time
//		ui32ModeTimer :   Control variable for a peroid pressing
//		ui32Eeprom:		  Eeprom status

//
//*****************************************************************************
typedef struct
{
//	uint32_t ui32Colors[3];
	uint32_t ui32Mode;
	uint32_t ui32Buttons;
	uint32_t ui32ManualIndex;
	uint32_t ui32ModeTimer;
	uint32_t ui32Eeprom;
	uint32_t ui32ModeFree;
//	float fColorWheelPos;
//	float fIntensity;

}tAppState;


//*****************************************************************************
//
// Global variables originally defined in qs-rgb.c that are made available to
// functions in other files.
//
//*****************************************************************************
extern volatile tAppState g_sAppState;
extern uint32_t pui32Data[1];
extern uint32_t pui32Read[1];
extern volatile unsigned long ulDutyCycle;
extern uint8_t Do_one;
//*****************************************************************************
//
// Functions defined in qs-rgb.c that are made available to other files.
//
//*****************************************************************************
extern void AppButtonHandler(void);
//extern void AppRainbow(uint32_t ui32ForceUpdate);
extern void ConfigureUART(void);
extern void ConfigurePWM(void);
extern void SysTickIntHandler(void);
extern void ConfigLED1(void);
extern void delay_ms(uint16_t count);
#endif /* INCLUDESS_H_ */
