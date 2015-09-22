#define TARGET_IS_TM4C123_RB1
#define PART_LM4F120H5QR
#define PART_TM4C123GH6PM TARGET_IS_BLIZZARD_RB1

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/eeprom.h"
#include "utils/uartstdio.h"
#include "config.h"
#include "utilities.h"
#include "uart_lib.h"
#include "adc_lib.h"
#include "eeprom_lib.h"


uint8_t UART_DATA_IN[256]={0};
uint8_t UART_DATA_OUT[256]={0};
uint32_t ADC_BUFFER[10] ={0};
uint32_t ADC0_VAL = 0;
uint32_t ADC1_VAL = 0;
uint32_t ADC2_VAL = 0;
uint32_t ADC3_VAL = 0;
uint32_t ADC4_VAL = 0;
uint32_t ADC5_VAL = 0;

uint32_t stepWatch = 0;
uint32_t stepWatchOld = 0;
uint32_t stepAngle = 0;
uint32_t stepAngleOld = 0;
uint32_t light=0;
uint32_t stepMinPos = 0;
uint32_t stepMaxPos = 0;

uint32_t thPos[1] = {0};
uint32_t thMax[1] = {0};
uint32_t thMin[1] = {0};
uint32_t configMode = 0;

// --- State variables ----

uint32_t stateConfig = 0;

struct stateStr program;
struct stateStr* progPtr;

int main(void)
{		
		progPtr = &program;
	
		ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);

    StepPortsInit();

		UART_Init();
	
		ADC_Init();
	
		ROM_EEPROMInit();
	
			StepEn_Stop();
		//StepEn_Go();
	
		if (GetThPos() == 0xFFFFFFFF){
			stepWatch = 0x00000000;
			SaveThPos();
		}
		
		if (GetThMin() == 0xFFFFFFFF){
			stepWatch = 0x00000000;
			stepMinPos = stepWatch;
			SaveThMin();
		} else {
			stepMinPos = GetThMin();
		}
		
		if (GetThMax() == 0xFFFFFFFF){
			stepWatch = 0x00000000;
			stepMaxPos = stepWatch;
			SaveThMax();
		} else {
			stepMaxPos = GetThMax();
		}
		
		stepWatch = GetThPos();
		
		sprintf(UART_DATA_OUT, "|-->>>>>  INITIAL SEQUENCE  <<<<<--|\n\r");
		UARTSend(UART_DATA_OUT);
		sprintf(UART_DATA_OUT, "|--STEP CHECK--| Pos = %i || Angle= %i %%\n\r", stepWatch, calcStepAngle());
		UARTSend(UART_DATA_OUT);
		ADC1_VAL = GetADCVal(AIN1);
		sprintf(UART_DATA_OUT, "|---ADC CHECK--| ADC_1 = %i mV\n\r", ADC1_VAL);
		UARTSend(UART_DATA_OUT);
		ADC1_VAL = GetADCVal(AIN1);
		sprintf(UART_DATA_OUT, "|---ADC CHECK--| ADC_1 = %i mV\n\r", ADC1_VAL);
		UARTSend(UART_DATA_OUT);
		ADC1_VAL = GetADCVal(AIN1);
		sprintf(UART_DATA_OUT, "|---ADC CHECK--| ADC_1 = %i mV\n\r", ADC1_VAL);
		UARTSend(UART_DATA_OUT);
		sprintf(UART_DATA_OUT, "| ============ = END = ============ |\n\r");
		UARTSend(UART_DATA_OUT);
		
		if ((stepWatch>stepMinPos) && (stepWatch<=stepMaxPos)){
				sprintf(UART_DATA_OUT, "|--- RETURNING TO INITIAL POINT...  ---|\n\r");
				UARTSend(UART_DATA_OUT);
				StepEn_Go();
				while (stepWatch > stepMinPos){
					Th_BWD();
				}
				StepEn_Stop();
		}
		
		//program.mode = initial;
		program.mode = normal;
		program.modeState = modeOFF;
		
		while(1){
		
		
				switch(program.mode){
					case initial:
							program.modeState = modeOFF;
							sprintf(UART_DATA_OUT, " --- ZER: init MODE ---\n\r");
							UARTSend(UART_DATA_OUT);
							sprintf(UART_DATA_OUT, "|--INITIAL--| Pos = %i || Angle= %i %%\n\r", stepWatch, calcStepAngle());
							UARTSend(UART_DATA_OUT);
							sprintf(UART_DATA_OUT, "|--INITIAL--| MIN = %i || MAX= %i \n\r", GetThMin(), GetThMax());
							UARTSend(UART_DATA_OUT);
							while((program.modeState == modeOFF) && (program.mode == initial)){
									
							}
							break;
					case config:
							program.modeState = modeON;
							program.configMode = configSTOP;
							sprintf(UART_DATA_OUT, " --- ZER: config MODE ---\n\r");
							UARTSend(UART_DATA_OUT);
							while(program.modeState == modeON){
									switch (program.configMode){
										case configSTOP:
												sprintf(UART_DATA_OUT, " --- ZER: please set the MIN throttle position by pushing <B> ---\n\r");
												UARTSend(UART_DATA_OUT);
												while (program.configMode != configBWD){
														ButtonsOperation();
												}
												SaveThMin();
											break;
										case configBWD:
												stepWatch = 0;
												sprintf(UART_DATA_OUT, " --- ZER: please set the MAX throttle position by pushing <F> ---\n\r");
												UARTSend(UART_DATA_OUT);
												while (program.configMode != configFWD){
														ButtonsOperation();
												}
												SaveThMax();
												
												sprintf(UART_DATA_OUT, " --- ZER: Config is done! ---\n\r");
												UARTSend(UART_DATA_OUT);
												sprintf(UART_DATA_OUT, " --- ZER: MIN = %i || MAX = %i ---\n\r", GetThMin(), GetThMax());
												UARTSend(UART_DATA_OUT);
												sprintf(UART_DATA_OUT, " --- ZER: <exit> to change mode ---\n\r");
												UARTSend(UART_DATA_OUT);
												sprintf(UART_DATA_OUT, " --- ZER: <config> to reconfigure MIN & MAX ---\n\r");
												UARTSend(UART_DATA_OUT);
											break;
										case configFWD:
												
											break;
									}
							}
							break;
					case manual:
							program.modeState = modeON;
							sprintf(UART_DATA_OUT, " --- ZER: manual MODE ---\n\r");
							UARTSend(UART_DATA_OUT);
							while(program.modeState == modeON){
									ButtonsOperation();
							}
							break;
					case adc_read:
							program.modeState = modeON;
							sprintf(UART_DATA_OUT, " --- ZER: adc_read MODE ---\n\r");
							UARTSend(UART_DATA_OUT);
							while(program.modeState == modeON){
									MKS_DELAY (1000000);
									ADC1_VAL = GetADCVal(AIN1);
									sprintf(UART_DATA_OUT, "ADC_1 = %i mV\n\r", ADC1_VAL);
									UARTSend(UART_DATA_OUT);
							}
							break;
					case normal:
							program.modeState = modeON;
							sprintf(UART_DATA_OUT, " --- ZER: normal MODE ---\n\r");
							UARTSend(UART_DATA_OUT);
							while(program.modeState == modeON){
									light = 0;
									ADC1_VAL = GetADCVal(AIN1);
									if((ADC1_VAL>2400) && (stepWatch != stepMaxPos)) {
											StepEn_Go();
											Th_FWD();
									} else if((ADC1_VAL<2074) && (ADC1_VAL>1057) && (stepWatch != stepMinPos)) {
											StepEn_Go();
											Th_BWD();
									} else {
											StepEn_Stop();
									}
									GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN, light);
									if (stepWatch != stepWatchOld) stepWatchOld = stepWatch;
								}
									
							}
					}
				
			}
/*
		if(program.mode == initial){
				sprintf(UART_DATA_OUT, "|--INITIAL MODE--|\n\r");
				UARTSend(UART_DATA_OUT);
		} else {
				sprintf(UART_DATA_OUT, "|--INITIAL MODE--| %i\n\r", program.mode);
				UARTSend(UART_DATA_OUT);
		}
*/		
      //SysCtlDelay(500000);
			
/*==========| ADC Working copy as an example |========================
			
				UARTCheck();
		if (UartIsNotEmpty(UART_DATA_IN)){
		UART_DATA_OUT[0]='>';
		UART_DATA_OUT[1]='>';
		UART_DATA_OUT[2]='\n';
		UART_DATA_OUT[3]='\r';
			UARTSend(UART_DATA_OUT);
			ADC0_VAL = GetADCVal(AIN0);
			sprintf(UART_DATA_OUT, "ADC_0 = %i mV\n\r", ADC0_VAL);
			UARTSend(UART_DATA_OUT);
			ADC1_VAL = GetADCVal(AIN1);
			sprintf(UART_DATA_OUT, "ADC_1 = %i mV\n\r", ADC1_VAL);
			UARTSend(UART_DATA_OUT);
			UART_In_Clear();
		}
=============================||========================================*/
	

