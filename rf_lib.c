//*****************************************************************************
//
// uart_echo.c - Example for reading data from and writing data to the UART in
//               an interrupt driven fashion.
//
//*****************************************************************************
#ifndef H_RF_LIB
#define H_RF_LIB

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
#include "rf_lib.h"
#include "eeprom_lib.h"


extern uint8_t RF_DATA_IN[];
extern uint8_t RF_DATA_OUT[];
extern uint32_t stepWatch;
extern uint32_t stepAngle;
extern uint32_t stepMinPos;
extern uint32_t stepMaxPos;


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
void RF_In_Clear(void){
		uint32_t i = 0;
		for(i=0;i<128;i++){
			RF_DATA_IN[i]= 0;
		}
}

void RF_Out_Clear(void){
		uint32_t i = 0;
		for(i=0;i<128;i++){
			RF_DATA_OUT[i] = 0;
		}
}

void RFCheck(void)
{
		uint32_t i = 0;
		//uint8_t temp_arr[256]={0};
		//uint8_t selfcount = 0;
    //uint32_t ui32Status;
    //

    // Loop while there are characters in the receive FIFO.
    //
		i = 0;
    while(ROM_UARTCharsAvail(UART4_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        RF_DATA_IN[i] = ROM_UARTCharGetNonBlocking(UART4_BASE);
        //
        // Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
				i++;
    }
	/*	i=0;
		for (i=0;i<256;i++){
			if (temp_arr[i] == 0x00) break;
			RF_DATA_IN[i] = temp_arr[i];
			//if ((temp_arr[i] == '\r')||(temp_arr[i] == 0x00)) break;
		//	if (temp_arr[i] == 0x00) break;
			//selfcount = i+1;
		}
		///RF_DATA_IN[selfcount] = '\r';
		*/
}

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void RFSend(const uint8_t *pui8Buffer)
{
		uint32_t i = 0;
		uint32_t selfcount = 0;
		uint8_t temp_arr[256]={0};
		for (i=0;i<256;i++){
			temp_arr[i] = pui8Buffer[i];
			if ((temp_arr[i] == '\r')||(temp_arr[i] == 0x00)) break;
			selfcount = i+1;
		}
		
		temp_arr[selfcount] = '\r';
		selfcount++;
		
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
				UARTCharPut(UART4_BASE, temp_arr[selfcount-(selfcount-i)]);
				i++;
				//MKS_DELAY (delay);
    }
		RF_Out_Clear();
		RF_In_Clear();
		UART_In_Clear();
		UART_Out_Clear();
		program.modeState = modeON;
		program.mode = rf;
}

//*****************************************************************************
//
// This example demonstrates how to send a string of data to the UART.
//
//*****************************************************************************
void RF_Init(void)
{
					//
					// Enable lazy stacking for interrupt handlers.  This allows floating-point
					// instructions to be used within interrupt handlers, but at the expense of
					// extra stack usage.
					//
    //ROM_FPUEnable();
    //ROM_FPULazyStackingEnable();
					//
					// Enable the peripherals used by this example.
					//
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    //
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();
		ROM_IntEnable(INT_UART4);
    ROM_UARTIntEnable(UART4_BASE, UART_INT_RX | UART_INT_RT);
		UARTIntRegister(UART4_BASE,RFIntHandler);
    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PC4_U4RX);
    GPIOPinConfigure(GPIO_PC5_U4TX);
    ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART4_BASE, ROM_SysCtlClockGet(), 9600,
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

void RFIntHandler(void){
    uint32_t ui32Status;
    //
    // Get the interrrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART4_BASE, true);
    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART4_BASE, ui32Status);
    //
    // Loop while there are characters in the receive FIFO.
    //
    RFCheck();
		
		UART_from_RF_Send(RF_DATA_IN);
		
		RF_In_Clear();
		RF_Out_Clear();
};
 #endif
	
