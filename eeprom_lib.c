#ifndef H_EEPROM_LIB
#define H_EEPROM_LIB

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
#include "driverlib/eeprom.h"
#include "utilities.h"
#include "config.h"
#include "uart_lib.h"
#include "eeprom_lib.h"


extern uint32_t thPos[];
extern uint32_t stepWatch;
extern uint32_t thMax[];
extern uint32_t thMin[];


void SaveThPos(void){
	thPos[0] = stepWatch;
	ROM_EEPROMProgram(thPos, 0x04, 4);
}

uint32_t GetThPos(void){
	thPos[0] = 0;
	ROM_EEPROMRead(thPos, 0x04, 4);
	stepWatch = thPos[0];
	return thPos[0];
}

void SaveThMin(void){
	thMin[0] = stepWatch;
	ROM_EEPROMProgram(thMin, 0x08, 4);
}

uint32_t GetThMin(void){
	thMin[0] = 0;
	ROM_EEPROMRead(thMin, 0x08, 4);
	return thMin[0];
}

void SetThMin(void){
		stepWatch = 0;
		SaveThPos();
		SaveThMin();
}
	
void SaveThMax(void){
	thMax[0] = stepWatch;
	ROM_EEPROMProgram(thMax, 0x12, 4);
}

uint32_t GetThMax(void){
	thMax[0] = 0;
	ROM_EEPROMRead(thMax, 0x12, 4);
	return thMax[0];
}	

void SetThMax(void){
		SaveThMax();
}

 #endif

