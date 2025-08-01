#include "PowerBoard.h"

ADS1115 PB_ADS(0x48);

void Init_PowerBoard()
{
  if (!PB_ADS.begin())         
  {
    Serial.println("invalid address PB_ADS1x15 or PB_ADSADR not found");
  } 
  else  
  { 
    Serial.println("I found PB_ADS1x15 and PB_ADSADR ");
    // if (!PB_ADS.isConnected())   
    // {Serial.println("address PB_ADSADR not found");} 
    // else 
    // {  //PB_ADS is connected
    Serial.println("address PB_ADSADR found");
    PB_ADS.begin();           // READ ADC on Power Board
    PB_ADS.setGain(0);        //  0: Â±6.144 volt, 2: Â±2.048V
    PB_ADS.setDataRate(4);    //  0=620ms 1=310ms, 2=155ms, 3=155ms, 4=155ms, ....   7 = fast
    PB_ADS.setMode(1);        //  single shot mode
    PB_ADS.requestADC(0);     //  Start ADC0 to activate settings
   
  } 
} //END if (!PB_ADS.begin()) 

unsigned long timecnt_Start = 0;  
void Read_ADS1115()
{
    //Read in the 4 ADC Values from Powerboard
    int16_t tmp;
    if (PB_ADS.isReady()) 
    { 
      if (PB_ADS.getError() != ADS1X15_OK) 
      {     //if error
        Serial.print("ADCchannelPB ");Serial.print(ADCchannelPB); Serial.println(" ERROR CONVERSION");
        ADCPBErrorCounter++;
        tmp =  PB_ADS.getValue(); //Readin erroneous value and request same ADCchannelPB again
      }
      else  //if no error
      {
        tmp = PB_ADS.getValue(); if(tmp <0) {tmp = 0;} //If sampled value is negative
        ADCval[ADCchannelPB] =tmp; // Raw analog input
        switch (ADCchannelPB)
        {
          case 0: { //Temperature
            ADCval0[ADCchannelPB][ADChistoryPB0] = tmp;//writes roundabout the raw values into the History of this channel
            ADChistoryPB0++; if (ADChistoryPB0 >= ADCSAMPLES) { ADChistoryPB0 = 0; }
            ADC0average = 0;
            for (int j = 0; j < ADCSAMPLES; j++) {ADC0average += ADCval0[0][j]; }
            ADC0average /=  ADCSAMPLES; 
            ADC0average = TemperatureFromADCValue(ADC0average); //Real Temperature in Celsius
            
            //Calculate time from last conversion to this conversion of ADC0!!
            unsigned long timecnt_End = micros();
            timeCntAds1115 = timecnt_End - timecnt_Start;
            timecnt_Start = micros();
            break;
          }
          case 1: { //MotorU
            ADCval0[ADCchannelPB][ADChistoryPB1] = tmp; //writes roundabout the raw values into the History of this channel
            ADChistoryPB1++; if (ADChistoryPB1 >= ADCSAMPLES) { ADChistoryPB1 = 0; } 
            ADC1average = 0;
            for (int j = 0; j < ADCSAMPLES; j++)   { ADC1average += ADCval0[1][j]; }
            ADC1average /=  ADCSAMPLES; ADC1average = ADC1average/29; //Real MotorU 0-600V
            break;
          }
          case 2:
          { //MotorI
            ADCval0[ADCchannelPB][ADChistoryPB2] = tmp; 
            ADChistoryPB2++; if (ADChistoryPB2 >= ADCSAMPLES) {ADChistoryPB2 = 0;}
            ADC2average = 0;
            for (int j = 0; j < ADCSAMPLES; j++) { ADC2average += ADCval0[2][j];}
            ADC2average /=  ADCSAMPLES; 
            ADC2average = (ADC2average)/1050; //Real MotorI 0-10A

            break;
          }
          case 3:
          { //Disable-ITRIP
            ADCval0[ADCchannelPB][ADChistoryPB3] = tmp; 
            ADChistoryPB3++; if (ADChistoryPB3 >= ADCSAMPLES) {ADChistoryPB3 = 0;}               
            ADC3average = 0;
            for (int j = 0; j < ADCSAMPLES; j++) { ADC3average += ADCval0[3][j];} 
            ADC3average /=  ADCSAMPLES; 
            ADC3average = ADC3average/Divider4Volt;   //Real Voltage on Disable-ITRIP. 
                                                      //Value > 0.47V disables Powerstage f.e. MOTOROFF signal or I>10A.
            break;
          }
        } //END switch (ADCchannelPB)       
        
        ADCchannelPB++; if (ADCchannelPB >= 4) { ADCchannelPB = 0;}
        PB_ADS.requestADC(ADCchannelPB);//  request next ADCchannelPB

      } //END if (PB_ADS.getError() != ADS1X15_OK)  
    }  //END if (PB_ADS.isReady())    

} //END void Read_ADS1115()

int16_t TemperatureFromADCValue(int16_t ADCvalue)
{
  float Vout;
  float R2;
  int16_t tmp;
  
  Vout = ADCvalue * (6.144 / 32768);
  R2 = (Vout * 4700) / (5 - Vout);
  if (R2 > 29972)
  {
    tmp = -0.00045 * R2 + 63.25;
    return tmp;
  }
  else if (R2 < 29972 && R2 > 3024)
  {
    tmp = -0.0023 * R2 + 106.89;
    return tmp;
  }
  else
  {
    tmp = -0.011 * R2 + 154.03;
    return tmp;
  }
}