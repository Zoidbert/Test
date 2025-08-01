#include "ADS1X15.h"

#ifndef __CONFIG_H__
    #include "Config.h"    
#endif

#ifndef __POWERBOARD_H__
    extern ADS1115 PB_ADS;
#endif

void Init_PowerBoard();
void Read_ADS1115();
int16_t TemperatureFromADCValue(int16_t ADCvalue);