/*
HEADER FILE

utilities.h --> utilities.c

*/
#ifndef UTILITIES_H
#define UTILITIES_H

void MKS_DELAY (uint32_t ms);

//void ConfigureUART(void);
void StepPortsInit(void);

void StepGo(uint32_t steps);

void StepEn_Go(void);

void StepEn_Stop(void);

void StepDir_F(uint32_t steps);

void StepDir_B(uint32_t steps);

uint8_t UartIsNotEmpty(uint8_t *array);

uint8_t BUTTON1_ON (void);

uint8_t BUTTON2_ON (void);

void But1_Forward(void);

void But2_Backward(void);

void ButtonsOperation(void);

void Th_FWD(void);

void Th_BWD(void);

void goToPos(int nPos);

void initGoToMin(void);

void initGoToMax(void);

uint32_t calcPosToGo(uint32_t adcValue);

#endif