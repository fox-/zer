/*
CONFIGURATION VARIABLES
*/
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "config.h"
#define TARGET_IS_TM4C123_RB1

//==============================================================

#define STEP_OUT_PORT 	GPIO_PORTA_BASE //[1] Port A --> Stepper motor driver port
#define STEP_OUT1 			GPIO_PIN_2 // [2] Pin A2 --> OUT1
#define STEP_OUT2 			GPIO_PIN_3 // [3] Pin A3 --> OUT2
#define STEP_OUT3				GPIO_PIN_4// [4] Pin A4 --> OUT3
#define STEP_OUT4				GPIO_PIN_5// [5] Pin A5 --> OUT4

#define STEP_OUT5				GPIO_PIN_6// [5] Pin A5 --> OUT4
#define STEP_OUT6				GPIO_PIN_7// [5] Pin A5 --> OUT4

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

#define BUTTON_1 GPIO_PIN_0
#define BUTTON_2 GPIO_PIN_4

#define AIN0						0
#define AIN1						1
#define AIN2						2
#define AIN3						3
#define AIN4						4
#define AIN5						5
//=============================================================

#define STEP_DELAY			600 // 3000us=3ms
#define ONE_STEP				50
#define SIGNALS_DELAY 	10
#define ADC_DELAY 			3000

//===== Configuring strusture =================================
#define initial		 999 
#define config 		13
#define manual 		17
#define normal 		19
#define adc_read	21
#define rf				23
#define neuro 		27
#define neuro2		29

#define modeON		23
#define modeOFF		25

#define configFWD		119	
#define configBWD		127
#define configSTOP	131

struct stateStr {
		int mode;				// can be set to: 1) initial 2)config 3) manual 4) normal 5) adc_read 
		int modeState;	// can be set to: 1) modeON	 2)modeOFF
		int configMode; // can be set to: 1) configFWD 2) configBWD 3) configSTOP
};


#define REFERENCE_VOLTAGE_MV                (3275L)
#define NUMBER_OF_STEPS_12_BIT_RESOLUTION   (4096L)

#endif

