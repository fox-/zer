/*

UTILITY FUNCTIONS FOR TM4C123GH6PM MCU

*/

#ifndef H_UTILITIES
#define H_UTILITIES

#define TARGET_IS_TM4C123_RB1
#define PART_TM4C1233H6PM

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "config.h"
#include "utilities.h"
#include "uart_lib.h"
#include "eeprom_lib.h"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

extern uint8_t UART_DATA_IN[];
extern uint8_t UART_DATA_OUT[];
extern uint32_t stepWatch;
extern uint32_t stepWatchOld;
extern uint32_t stepAngle;
extern uint32_t stepAngleOld;
extern uint32_t light;
extern uint32_t stepMinPos;
extern uint32_t stepMaxPos;
extern uint32_t stepPos;
extern uint32_t nPosOld;

void MKS_DELAY (uint32_t mks){
	ROM_SysCtlDelay((ROM_SysCtlClockGet()/(3000000))* mks) ;  // more accurate
}

void StepPortsInit(void){
	uint8_t ui8LED = 2;
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	
	// Unlock PF0 so we can change it to a GPIO input
	// Once we have enabled (unlocked) the commit register then re-lock it
	// to prevent further changes.  PF0 is muxed with NMI thus a special case.
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, BUTTON_1|BUTTON_2);
	
	GPIOPadConfigSet(GPIO_PORTF_BASE, BUTTON_1|BUTTON_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	
	ROM_GPIOPinTypeGPIOOutput(STEP_OUT_PORT, STEP_OUT1|STEP_OUT2|STEP_OUT3|STEP_OUT4);
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
}

void StepGo(uint32_t steps){
	uint32_t i=0;
	for(i=0; i<steps; i++){
								ROM_GPIOPinWrite(STEP_OUT_PORT, STEP_OUT1, 0xFF);
					
                MKS_DELAY (STEP_DELAY);
									
								ROM_GPIOPinWrite(STEP_OUT_PORT, STEP_OUT1, 0x00);
								
								MKS_DELAY (STEP_DELAY);
					};
}

void StepEn_Go(void){
	ROM_GPIOPinWrite(STEP_OUT_PORT, STEP_OUT3, 0xFF);
	MKS_DELAY (SIGNALS_DELAY);
}

void StepEn_Stop(void){
	ROM_GPIOPinWrite(STEP_OUT_PORT, STEP_OUT3, 0x00);
	MKS_DELAY (SIGNALS_DELAY);
}

void StepDir_F(uint32_t steps){
	ROM_GPIOPinWrite(STEP_OUT_PORT, STEP_OUT2, 0x00);
	MKS_DELAY (SIGNALS_DELAY);
	StepGo(steps);
}

void StepDir_B(uint32_t steps){
	ROM_GPIOPinWrite(STEP_OUT_PORT, STEP_OUT2, 0xFF);
	MKS_DELAY (SIGNALS_DELAY);
	StepGo(steps);
}

uint8_t UartIsNotEmpty(uint8_t *array){
	uint32_t i=0;
	uint8_t res=0;
	for(i=0;i<128;i++){
		if (array[i]!=0){
			res=1;
			break;
		} else {
			res = 0;
		}
	}
	return res;
}

uint8_t BUTTON1_ON (void){
	if (!GPIOPinRead(GPIO_PORTF_BASE, BUTTON_1)){
		return true;
	} else {
		return false;
	}
}

uint8_t BUTTON2_ON (void){
	if (!GPIOPinRead(GPIO_PORTF_BASE, BUTTON_2)){
		return true;
	} else {
		return false;
	}
}

void But1_Forward(void){
	if(BUTTON1_ON()) {
			StepEn_Go();
			while (BUTTON1_ON()){
				Th_FWD();
				GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN, light);
				}
			StepEn_Stop();
			}
			
}

void But2_Backward(void){
	if(BUTTON2_ON()){
			StepEn_Go();
			while (BUTTON2_ON()){
				Th_BWD();
				GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN, light);
				}
			StepEn_Stop();
			}
      
}

void ButtonsOperation(void){
	
	light = 0;
	GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN, light);
	
	But1_Forward();
	
	But2_Backward();
	
	SaveThPos();
}

void Th_FWD(void){
	//StepEn_Go();
	light |= LED_GREEN;
	StepDir_F(ONE_STEP);
	stepWatch += ONE_STEP;
	SaveThPos();
	//StepEn_Stop();
}

void Th_BWD(void){
	//StepEn_Go();
	light |= LED_BLUE;
	StepDir_B(ONE_STEP);	
	stepWatch -= ONE_STEP;
	SaveThPos();
	//StepEn_Stop();
}

void goToPos(int nPos){
	if (nPosOld != nPos) {
		nPosOld = nPos;
		sprintf(UART_DATA_OUT, "|--NEURO--| going to ... -> %i \n\r", nPos);
		UARTSend(UART_DATA_OUT);
		GetThPos();
		calcStepPos(nPos);
		nPos = stepPos;
		
		if (nPos > stepWatch){
			StepEn_Go();
					while (stepWatch < nPos){
						Th_FWD();
						sprintf(UART_DATA_OUT, "|--CONTROL FWD-| Pos = %i || Angle= %i %%\n\r", stepWatch, calcStepAngle());
						UARTSend(UART_DATA_OUT);
					}
					StepEn_Stop();
		} else if (nPos < stepWatch){
			StepEn_Go();
					while (stepWatch > nPos){
						Th_BWD();
						sprintf(UART_DATA_OUT, "|--CONTROL BWD--| Pos = %i || Angle= %i %%\n\r", stepWatch, calcStepAngle());
						UARTSend(UART_DATA_OUT);
					}
					StepEn_Stop();
		}
	}
	
}

 #endif
