/*==================================================
//----------- ADC battery check functions ----------
//==================================================*/

/*---------------------- Function template ---------------------------------
// Function:
// Input:
// Return:
//--------------------------------------------------------------------------*/
/*---------------------- Custom Icludes ------------------------------------*/
#ifndef ADC_LIB_H
#define ADC_LIB_H

#define TARGET_IS_TM4C123_RB1
#define PART_TM4C123GH6PM
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/eeprom.h"
#include "driverlib/adc.h"
/* Include of standard C Headerfiles */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "adc_lib.h"
#include "config.h"
#include "utilities.h"

extern uint32_t ADC_BUFFER[];

/*---------------------- Custom variables ----------------------------------*/


/*---------------------- #1 ADC fuctions -----------------------------*/

void ADC_Init(void){
	uint8_t delay = 10;
	/* -7- Enable System clock for ADC*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	MKS_DELAY (delay);
	
	ADCSequenceDisable(ADC0_BASE, 0);
	MKS_DELAY (delay);
	
	/* -1- Configure adc trigger source and priority
		 --- ADC0_BASE -> adress of ADC module
		 --- 0 -> sample sequenser number
		 --- ADC_TRIGGER_PROCESSOR -> trigger master for ADC
		 --- 0 -> highest priority*/
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	MKS_DELAY (delay);
	
	ADCHardwareOversampleConfigure(ADC0_BASE, 8);
	MKS_DELAY (delay);
	
	/* -2- Configure adc trigger source and priority
		 --- ADC0_BASE -> adress of ADC module
		 --- 0 -> sample sequenser number
		 --- 0:5 -> step to be conigured
		 --- ADC_CTL_END -> last in the sequence
		 --- ADC_CTL_CH0 -> channel to be sampled*/
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_IE | ADC_CTL_CH1);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_IE | ADC_CTL_CH2);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_IE | ADC_CTL_CH3);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_IE | ADC_CTL_CH4);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_IE | ADC_CTL_CH5 | ADC_CTL_END);
	MKS_DELAY (delay);
	
	/* -3- Init ADC module
	--- ADC0_BASE -> adress of ADC module
	--- 0 -> sample sequenser number*/
	ADCSequenceEnable(ADC0_BASE, 0);
	MKS_DELAY (delay);
	
	/* -4- Clear the interrupt status flag (just to be sure it's cleared)*/
	ADCIntClear(ADC0_BASE, 0);
	MKS_DELAY (delay);	
}

/* Function: read battery charge sampled data
// Input:	none
// Return: battery charge in mV */
uint32_t GetADCVal(uint8_t ain_ch){
	//uint32_t adc_buf[10]={0};
	uint32_t adc_value=0;
	ADCProcessorTrigger(ADC0_BASE, 0);
	//MKS_DELAY (8000);
	MKS_DELAY (ADC_DELAY);
	while (!ADCIntStatus(ADC0_BASE, 0, false))
	{
	};
	ADCIntClear(ADC0_BASE, 0);
	ADCSequenceDataGet(ADC0_BASE, 0, ADC_BUFFER);
	/* 2^12=4096 ~ 3v on ADC */
  /* Convert to mV */
	adc_value=ADC_BUFFER[ain_ch];
	ADC_BUFFER[ain_ch] = 0;
	adc_value = (uint32_t)(((float)adc_value * 3300.0L) / 4096.0L);
	return adc_value;
}


//uint8_t BatteryGetMessage(uint8_t* message){
//	uint8_t length = 0;
//	uint32_t batcharge = 0;
//	uint32_t batterytriggervalue = 0;
//	uint8_t cursor = 0;
//	message[length++] = 0x41;
//	batcharge = getBatCharge();
//	if (batcharge < 1000){
//		batcharge = 0;
//	}
//	batterytriggervalue = GetEventConfig(bat_ch)&0x00FFFFFF;
//	if (batterytriggervalue > batcharge){//trigger!
//		message[length++] = '!';
//	}
//	cursor = length;
//	cursor = printpoint(batcharge, message, 6, 2) + length;//add XX.XX to first one of two symbols
//	cursor = StrCat(message, "V", cursor);//Add V symbol at the last of the string
//	return cursor;
//}

//void BatteryCheck(void){
//	uint32_t batcharge = 0;
//	uint32_t batterytriggervalue = 0;
//	uint8_t message[20] = {0x00};
//	uint8_t length = 0;
//	if (IsEventActive(bat_ch)){
//		batcharge = getBatCharge();
//		if (batcharge < 1000){
//			batcharge = 0;
//		}
//		batterytriggervalue = GetEventConfig(bat_ch)&0x00FFFFFF;
//		if (batterytriggervalue > batcharge){//trigger!
//			SetEventDeactive(bat_ch);//deactive future states
//			length = BatteryGetMessage(message);
//			SetNewEvent(message, length);
//		}
//	}
//}

#endif
