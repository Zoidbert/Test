#include "config.h"

const int ContinousDispl  = 1; // WR and RD Data2TMC continously
const int WithSDcard      = 1; //Use SD Card for reading



bool displayErrorV        = false;
bool displayData          = false;

byte   OperatingMode      = 0; //0= MotorDisabled, 1= MotorActivated
byte   RegisterCommand    = 0x00;
long   receivedValue      = 0;
char   IncomingKey        = 0;
int8_t FirstRun           = 0;
int8_t currentRow         = -1; //  static int currentRow = 0;
int8_t KeyEvent  = false;

uint8_t SpiMode =0;
uint8_t StateMotorOnfromIB   = 0;
uint8_t StateMotorOnfromENO2PB = 0;
uint8_t StateTMCStatus = 0;
uint8_t StateMotorOnfromESP2ENI = 0;
uint8_t StateFanON2PB = 0;
uint8_t StateMotorBrakeON2PB = 0;
uint8_t StateShuntOnfromESP = 0;

uint8_t OpMode               = 0;    //Mode MotorOff

uint8_t       ADCchannelPB =  0; //0:TEMP, 1:USense, 2:ISense, 3:DISABLE
uint8_t       ADChistoryPB0 = 0; //10 samples history
uint8_t       ADChistoryPB1 = 0;
uint8_t       ADChistoryPB2 = 0;
uint8_t       ADChistoryPB3 = 0;
float       ADCval[4]=      { 0 };
float       ADCval0[4][10]= { 0 };
float      ADC0average = 0;
float      ADC1average = 0;
float      ADC2average = 0;
float      ADC3average = 0;
uint8_t       ADCPBErrorCounter = 0;
uint8_t       ADCPBConversionCounter = 0;
uint8_t     IB_PCF8574_0X21ErrorCount =0;

uint32_t    timeCntAds1115 = 0;

uint32_t SpeedAD               = 0;    //Read Speed from AD
uint32_t ActualSpeed           = 0;    //Speed from TMC
uint32_t WRreceivedValue  = 0; //Receivd Value after Write into TMC
uint32_t LoopCycle;     //calculate loop time
uint32_t LoopFrequency; //calculate loop frequency

char a, b, c, d;
char hexBuffer[3]; 
char formattedValue[9];                               // 8 characters + null terminator
char values[numRows][9]   = { 0 };    // Array to store register names
char names[numRows][maxLineLength]   = { 0 };    // Array to store register names
int data[numRows][4]      = { 0 };       // Array to store data: 0=Numerator, 1=Register2Read, 2=Read/Write, 3=Value
int numberOfRows = 0;

void serial0Flush();

SemaphoreHandle_t xBinarySemaphore;

//ï¼UTC/GMT+8:00ï¼ 8*3600
const long gmtOffset_sec = 8 * 3600;    
const int daylightOffset_sec = 0;   //daylight saving time 3600ï¼or 0



TimerHandle_t NTPServerTmr;   


// Variablen zur Speicherung der Hall-Signale
int hallU, hallV, hallW, HALL, HALL_1;