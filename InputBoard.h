
#ifndef __CONFIG_H__
    #include "Config.h"
#endif

#ifndef __INPUTBOARD_H__
    #define __INPUTBOARD_H__

    extern PCF8574 IB_PCF8574_0X21;  //PCF8574 on Input Board
    extern int16_t IB_PCF8574_0X21_PortValues[8];

    #define IB_PCF8574_0X21_OUT0_SERVOREADY = 0;
    #define IB_PCF8574_0X21_OUT1 = 1;
    #define IB_PCF8574_0X21_OUT2 = 2;
    #define IB_PCF8574_0X21_IN0_MOTOROVERTEMP = 3;
    #define IB_PCF8574_0X21_IN1_SafeStop = 4;
    #define IB_PCF8574_0X21_IN2 = 5;
    #define IB_PCF8574_0X21_IN3 = 6;
    #define IB_PCF8574_0X21_IN4_MotorOn = 7;
     //1 IB/OUT2, 2 IB/OUT3, 3 IBMotorOverTemp, 4 IBIN1SafeStop, 5 IBIN2, 6 IBIN3, 7 IBMotorON

#endif

void Init_InputBoard();
void Read_Write_PCF8574_0x21();
