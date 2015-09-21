/*==================================================
//------------ ADC battery check header ------------
//==================================================*/


/**
 *  \brief Init battery pinout and ADC
 *  
 *  \return none
 */
void ADC_Init(void);

/**
 *  \brief return battery charge in mV
 *  
 *  \return battery charge in mV

 */
uint32_t GetADCVal(uint8_t ain_ch);

/**
 *  \brief Form message event 
 *  
 *  \param [in] message Parameter_Description
 *  \return message length

 */
//uint8_t BatteryGetMessage(uint8_t* message);

/**
 *  \brief Function for periodic battery status checking
 *  
 *  \return none

 */
//void BatteryCheck(void);


