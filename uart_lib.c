//*****************************************************************************
//
// uart_echo.c - Example for reading data from and writing data to the UART in
//               an interrupt driven fashion.
//
//*****************************************************************************
#ifndef H_UART_LIB
#define H_UART_LIB

#define TARGET_IS_TM4C123_RB1
#define PART_TM4C1233H6PM

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utilities.h"
#include "config.h"
#include "uart_lib.h"
#include "eeprom_lib.h"


extern uint8_t UART_DATA_IN[];
extern uint8_t UART_DATA_OUT[];
extern uint32_t stepWatch;
extern uint32_t stepAngle;
extern uint32_t stepMinPos;
extern uint32_t stepMaxPos;
extern uint32_t programMode;


extern struct stateStr program;
//*****************************************************************************
//!
//! This example application utilizes the UART to echo text.  The first UART
//! (connected to the USB debug virtual serial port on the evaluation board)
//! will be configured in 115,200 baud, 8-n-1 mode.  All characters received on
//! the UART are transmitted back to the UART.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void UART_In_Clear(void){
		uint32_t i = 0;
		for(i=0;i<128;i++){
			UART_DATA_IN[i]= 0;
		}
}

void UART_Out_Clear(void){
		uint32_t i = 0;
		for(i=0;i<128;i++){
			UART_DATA_OUT[i] = 0;
		}
}

void UARTCheck(void)
{
		uint32_t i = 0;
    //uint32_t ui32Status;
    //

    // Loop while there are characters in the receive FIFO.
    //
		i = 0;
    while(ROM_UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        UART_DATA_IN[i] = ROM_UARTCharGetNonBlocking(UART0_BASE);
        //
        // Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
				i++;
    }
}

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void UARTSend(const uint8_t *pui8Buffer)
{
		uint8_t i = 0;
		uint8_t selfcount = 0;
		uint8_t temp_arr[128]={0};
		for (i=0;i<128;i++){
			temp_arr[i] = pui8Buffer[i];
			selfcount = i+1;
			if (temp_arr[i] == '\r') break;
		}
		
	//uint8_t delay = 10;
    //
    // Loop while there are more characters to send.
    //
    while(selfcount--)
    {
        //
        // Write the next character to the UART.
        //
        //UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
				UARTCharPut(UART0_BASE, *pui8Buffer++);
				//MKS_DELAY (delay);
    }
		UART_Out_Clear();
}

void UART_from_RF_Send(const uint8_t *pui8Buffer)
{
		uint8_t i = 0;
		uint8_t selfcount = 0;
		uint8_t temp_arr[128]={0};
		for (i=0;i<128;i++){
			temp_arr[i] = pui8Buffer[i];
			selfcount += 1;
			if (temp_arr[i] == 0x00) break;
		}
		selfcount -= 1;
	//uint8_t delay = 10;
    //
    // Loop while there are more characters to send.
    //
		i=0;
    while(selfcount--)
    {
        //
        // Write the next character to the UART.
        //
        //UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
				UARTCharPut(UART0_BASE, temp_arr[i]);
				i++;
				//MKS_DELAY (delay);
    }
		UART_Out_Clear();
}

//*****************************************************************************
//
// This example demonstrates how to send a string of data to the UART.
//
//*****************************************************************************
void UART_Init(void)
{
    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    ROM_FPUEnable();
    ROM_FPULazyStackingEnable();
    //
    // Enable the peripherals used by this example.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();
		ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
		UARTIntRegister(UART0_BASE,UART0IntHandler);
    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    //ROM_IntEnable(INT_UART0);
    //ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
		//
    // Enable the UART interrupt.
    //
}

void UART0IntHandler(void){
		uint32_t ui32Status;
		programMode=program.mode;
    //
    // Get the interrrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);
    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART0_BASE, ui32Status);
    //
    // Loop while there are characters in the receive FIFO.
    //
    UARTCheck();
	
		if ((program.modeState == modeOFF) && (program.mode == initial)) {
					if (!(strcmp(UART_DATA_IN, "init"))){
							programMode = initial;
							program.mode = initial;

					} else if (!(strcmp(UART_DATA_IN, "config"))){
							programMode = config;
							program.mode = config;

					} else if (!(strcmp(UART_DATA_IN, "manual"))){
							programMode = manual;
							program.mode = manual;

					} else if (!(strcmp(UART_DATA_IN, "adc_read"))){
							programMode = adc_read;
							program.mode = adc_read;
						
					} else if (!(strcmp(UART_DATA_IN, "normal"))){
							programMode = normal;
							program.mode = normal;

					} else if (!(strcmp(UART_DATA_IN, "neuro"))){
							programMode = neuro;
							program.mode = neuro;

					} else if (!(strcmp(UART_DATA_IN, "neuro2"))){
							programMode = neuro2;
							program.mode = neuro2;

					} else if (!(strcmp(UART_DATA_IN, "rf"))) {
							programMode = rf;
							program.mode = rf;
						
					} else if (!(strcmp(UART_DATA_IN, "clean"))) {
							stepWatch = 0;
							calcStepAngle();
							SaveThPos();
							SaveThMin();
							SaveThMax();
							stepMinPos = 0;
							stepMaxPos = 0;
							sprintf(UART_DATA_OUT, "|--INTERNAL VARIABLES RESET--|\n\r");
							UARTSend(UART_DATA_OUT);
							sprintf(UART_DATA_OUT, "|--INITIAL--| Pos = %i || Angle= %i %%\n\r", stepWatch, calcStepAngle());
							UARTSend(UART_DATA_OUT);
							sprintf(UART_DATA_OUT, "|--INITIAL--| MIN = %i || MAX= %i \n\r", GetThMin(), GetThMax());
							UARTSend(UART_DATA_OUT);
					} else if (!(strcmp(UART_DATA_IN, "check"))) {
						
							sprintf(UART_DATA_OUT, "|--INITIAL--| Pos = %i || Angle= %i %%\n\r", stepWatch, calcStepAngle());
							UARTSend(UART_DATA_OUT);
							sprintf(UART_DATA_OUT, "|--INITIAL--| MIN = %i || MAX= %i \n\r", GetThMin(), GetThMax());
							UARTSend(UART_DATA_OUT);
						
					} else {
							sprintf(UART_DATA_OUT, " -!!!- UNKNOWN COMMAND -!!!-\n\r");
							UARTSend(UART_DATA_OUT);
					};
		} else if ((program.modeState == modeON) && (program.mode != initial)) {
					if (!(strcmp(UART_DATA_IN, "exit"))){
							program.mode = initial;
							program.modeState = modeOFF;
							sprintf(UART_DATA_OUT, " --- ZER: EXIT ---\n\r");
							UARTSend(UART_DATA_OUT);
					} else if (program.mode == config){ 
							switch (program.configMode){
								case configSTOP:
										if (!(strcmp(UART_DATA_IN, "set"))){
												program.configMode = configBWD;
												sprintf(UART_DATA_OUT, " --- ZER: Throttle MIN position is SET ---\n\r");
												UARTSend(UART_DATA_OUT);
										} else {
												sprintf(UART_DATA_OUT, " -!!!- UNKNOWN COMMAND -!!!-\n\r");
												UARTSend(UART_DATA_OUT);
											};
									break;
								case configBWD:
										if (!(strcmp(UART_DATA_IN, "set"))){
												program.configMode = configFWD;
												sprintf(UART_DATA_OUT, " --- ZER: Throttle MAX position is SET ---\n\r");
												UARTSend(UART_DATA_OUT);
										} else {
												sprintf(UART_DATA_OUT, " -!!!- UNKNOWN COMMAND -!!!-\n\r");
												UARTSend(UART_DATA_OUT);
											};
									break;
								case configFWD:
										if (!(strcmp(UART_DATA_IN, "config"))){
												program.configMode = configSTOP;
										} else {
												sprintf(UART_DATA_OUT, " -!!!- UNKNOWN COMMAND -!!!-\n\r");
												UARTSend(UART_DATA_OUT);
											};
									break;
							}
					} else if (program.mode == rf) {
								RFSend(UART_DATA_IN);
								program.modeState = modeON;
								program.mode = rf;

					} else {
							sprintf(UART_DATA_OUT, " -!!!- UNKNOWN COMMAND -!!!-\n\r");
							UARTSend(UART_DATA_OUT);
						};
		}
/*
		sprintf(UART_DATA_OUT, "Pointer to config strusture gives -> %i\n\r", program.mode);
		UARTSend(UART_DATA_OUT);
		 
*/	
/*		
		if (!(strcmp(UART_DATA_IN, "init"))){
			sprintf(UART_DATA_OUT, "String parsing!\n\r");
			UARTSend(UART_DATA_OUT);
		}
*/		
/*	
		sprintf(UART_DATA_OUT, "UART Receive Interrupt works!\n\r");
		UARTSend(UART_DATA_OUT);
		switch (UART_DATA_IN[0]){
			case 'F':
				StepDir_F(2000);
				break;
			case 'B':
				StepDir_B(2000);
				break;
		}
*/
		UART_In_Clear();
		UART_Out_Clear();
};
 #endif
	
