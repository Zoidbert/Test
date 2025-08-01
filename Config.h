#include <Arduino.h>

#include "PCF8574.h"
#include "WiFi.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include <NonBlockingDelay.hpp>


#ifndef __CONFIG_H__
    #define __CONFIG_H__
    // U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

    // Serial Monitor Enable Flags (0 = Disabled, 1 = Enabled)
    #define Serial_PB_ADS1115   0   //ADS1115PB sends messages to SerialMonitor
    #define Serial_IB_PCF8574_0X21   0   //IB_PCF8574_0x21 sends messages to SerialMonitor
    #define Serial_MB_TMC4761   0   //TMC4761 sends messages to SerialMonitor
    #define Serial_MB_Wifi      0   //Wifi sends messages to SerialMonitor
    #define Serial_REG_0x03     0   //Screen 0x03 sends messages to SerialMonitor
    #define Serial_REG_0x08     0   //Screen 0x08 sends messages to SerialMonitor
    #define Serial_REG_0x1b     0   //Screen 0x1b sends messages to SerialMonitor
    #define Serial_REG_0x73     0   //Screen 0x73 sends messages to SerialMonitor
    #define Serial_REG_0x7b     0   //Screen 0x7b sends messages to SerialMonitor

    #define DebounceTime  0     //debounce time for keyboard

    #define LOOPON1 0 
    #define LOOPON2 0
    #define LOOPON3 0
    #define DISPLAYON 1 
    #define WIFION 0

    #define MB_LEDC_SPEED_MODE LEDC_HIGH_SPEED_MODE
    #define MB_LEDC_CHANNEL    LEDC_CHANNEL_0
    #define MB_LEDC_FREQUENCY  5000*2   // Frequency in Hertz. Set frequency at 5 kHz
    #define MB_LEDC_OUTPUT_IO  32
    #define MB_LEDC_DUTY       4095     // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095

    #define FORMAT_LITTLEFS_IF_FAILED                   true

    #define Loop_TASK_STK_SIZE                          32000
    #define TASK_Loop_PRIORITY                          100   //fast

    #define Display_TASK_STK_SIZE                       32000
    #define TASK_Display_PRIORITY                       60 //30 //slow

    #define Wifi_TASK_STK_SIZE                       32000
    #define TASK_Wifi_PRIORITY                       20  //slow

    #define CSTMCPin            2  //CS TMC
    #define CSSDPin             5   //CS SDcard
    #define MB_Speed165VPin     36  //Speed +-1.65V on Input 36
    #define MotorOnfromIB       39 //MotorOn Freigabeeingang on Input 39
    #define MotorOnfromESP2ENI  33 // MotorOn from ESP to ENI of TMC Output 33
    #define MotorOnfromENO2PB   34 // MotorOn from ENO of TMC to PB Input 34
    #define TMCStatus           35 // TMC Status Pin to read into ESP
    #define FanON2PB            25 // Fan ON to PB
    #define MotorBrakeON2PB     26 // Motor Brake ON to PB
    #define ShuntOnfromESP      13 // Shunt ON from ESP to PB
    #define maxLineLength     30
    #define numRows           127
    #define emptyRegister     0x0  // Empty register value



    

    #define       ADCSAMPLES              2
    #define       Divider4Volt            5325

    
    //-----------Network time acquisition-----------//
    #define ntpServer1 "pool.ntp.org" //NTP Server
    #define ntpServer2 "ntp1.aliyun.com" //NTP Server

    

    // Definition der Pins für die Hall-Sensoren
    #define HALL_U_PIN 25
    #define HALL_V_PIN 26
    #define HALL_W_PIN 27

    #define FS_WITH_SD_CARD     1
    #define FS_FILENAME         "/TMC.TXT"          //Read this file from SDcard
    #define SOFTWAREVERSION     "TMC300-05-V1.09"   //This is Software Version according to this INO file.0

    extern const int ContinousDispl; // WR and RD Data2TMC continously
    extern const int WithSDcard; //Use SD Card for reading

    extern uint32_t timeCntAds1115;
    

    extern bool displayErrorV;
    extern bool displayData;

    extern byte   OperatingMode; //0= MotorDisabled, 1= MotorActivated
    extern byte   RegisterCommand;
    extern long   receivedValue;
    extern char   IncomingKey;
    extern int8_t FirstRun;
    extern int8_t currentRow; //  static int currentRow = 0;
    extern int8_t KeyEvent;

    extern uint8_t SpiMode;
    extern uint8_t StateMotorOnfromIB;
    extern uint8_t StateMotorOnfromENO2PB;
    extern uint8_t StateTMCStatus;
    extern uint8_t StateMotorOnfromESP2ENI;
    extern uint8_t StateFanON2PB;
    extern uint8_t StateMotorBrakeON2PB;
    extern uint8_t StateShuntOnfromESP;

    extern uint8_t  OpMode;    //Mode MotorOff

    extern uint8_t       ADCchannelPB; //0:TEMP, 1:USense, 2:ISense, 3:DISABLE
    extern uint8_t       ADChistoryPB0; //10 samples history
    extern uint8_t       ADChistoryPB1;
    extern uint8_t       ADChistoryPB2;
    extern uint8_t       ADChistoryPB3;
    extern float       ADCval[4];
    extern float       ADCval0[4][10];
    extern float      ADC0average;
    extern float      ADC1average;
    extern float      ADC2average;
    extern float      ADC3average;
    extern uint8_t       ADCPBErrorCounter;
    extern uint8_t       ADCPBConversionCounter;
    extern uint8_t      IB_PCF8574_0X21ErrorCount;

    extern uint32_t SpeedAD;    //Read Speed from AD
    extern uint32_t ActualSpeed;    //Speed from TMC
    extern uint32_t WRreceivedValue; //Receivd Value after Write into TMC
    extern uint32_t LoopCycle;     //calculate loop time
    extern uint32_t LoopFrequency; //calculate loop frequency



    
    extern char a, b, c, d;
    extern char hexBuffer[3]; 
    extern char formattedValue[9];                               // 8 characters + null terminator
    extern char values[numRows][9];    // Array to store register names
    extern char names[numRows][maxLineLength];    // Array to store register names
    extern int data[numRows][4];       // Array to store data: 0=Numerator, 1=Register2Read, 2=Read/Write, 3=Value
    extern int numberOfRows;

    void serial0Flush();

    extern SemaphoreHandle_t xBinarySemaphore;

    //ï¼UTC/GMT+8:00ï¼ 8*3600
    extern const long gmtOffset_sec;    
    extern const int daylightOffset_sec;   //daylight saving time 3600ï¼or 0

    extern const char* ssd;
    extern const char* pasd;

    extern TimerHandle_t NTPServerTmr;   


    // Variablen zur Speicherung der Hall-Signale
    extern int hallU, hallV, hallW, HALL, HALL_1;

#endif