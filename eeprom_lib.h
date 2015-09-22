#ifndef EEPROM_LIB_H
#define EEPROM_LIB_H

void SaveThPos(void);

uint32_t GetThPos(void);

void SaveThMin(void);

uint32_t GetThMin(void);
	
void SaveThMax(void);

uint32_t GetThMax(void);

uint32_t calcStepAngle(void);

#endif
