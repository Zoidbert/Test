#include "InputBoard.h"

PCF8574 IB_PCF8574_0X21(0x21);  //PCF8574 on Input Board
int16_t IB_PCF8574_0X21_PortValues[8];


void Init_InputBoard()
{
  Serial.println("Init IB_PCF8574_0x21_0x21...");
  if (IB_PCF8574_0X21.begin()) 
  {
      Serial.println("IB_PCF8574_0x21_0x21 OK");
  } 
  else
  {
      Serial.println("IB_PCF8574_0x21_0x21 NOT PRESENT");	
      IB_PCF8574_0X21ErrorCount++;	
  }
  //IB_PCF8574_0X21_PortValues[0] = 1; //P0  Output OUT1ServoRdy
  //IB_PCF8574_0X21_PortValues[1] = 0; //P1  Out2
  //IB_PCF8574_0X21_PortValues[2] = 0; //P2  Out3
  //IB_PCF8574_0X21_PortValues[3] = 0; //P3 MotorOvrTemp from Motor
  //IB_PCF8574_0X21_PortValues[4] = 1; //P4 IN1SafeStp
  //IB_PCF8574_0X21_PortValues[5] = 1; //P5 IN2
  //IB_PCF8574_0X21_PortValues[6] = 0; //P6 IN3
  //IB_PCF8574_0X21_PortValues[7] = 0; //P7 MotorON frm CNC

  for(int i=0;i<3;i++) 
  {
    IB_PCF8574_0X21.pinMode(i, OUTPUT);
  }

  for(int i=3;i<8;i++) 
  {
    IB_PCF8574_0X21.pinMode(i, INPUT);
  }
}

void Read_Write_PCF8574_0x21()
{
    #if Serial_IB_PCF8574_0X21 == 1               
    #endif 
    // Read in IB_PCF8574_0x21 on Input Board
    if (!IB_PCF8574_0X21.begin())
    {
      IB_PCF8574_0X21ErrorCount++;	
    }
    else
    {  // 1= Output  24V ON
       // 0= Output  24V OFF
      IB_PCF8574_0X21_PortValues[0] = 1; //P0  Output OUT1ServoRdy
      IB_PCF8574_0X21_PortValues[1] = 0; //P1  Out2
      IB_PCF8574_0X21_PortValues[2] = 1; //P2 Out3 
      //IB_PCF8574_0X21_PortValues[6] = 0; //P6 IN3
      //IB_PCF8574_0X21_PortValues[7] = 0; //[2] = 0; //P2  Out3
      //IB_PCF8574_0X21_PortValues[3] = 1; //P3 MotorOvrTemp from Motor
      //IB_PCF8574_0X21_PortValues[4] = 0; //P4 IN1SafeStp
      //IB_PCF8574_0X21_PortValues[5] = 0P7 MotorON frm CNC
      for(int i = 0; i < 3; i++)  
      {
        IB_PCF8574_0X21.digitalWrite(i, !IB_PCF8574_0X21_PortValues[i]); 
      }

      for(int i = 3; i < 8; i++)  
      { 
        IB_PCF8574_0X21_PortValues[i]=IB_PCF8574_0X21.digitalRead(i, true);
        #if Serial_IB_PCF8574_0X21 == 1   
        Serial.print(IB_PCF8574_0X21_PortValues[i]);  Serial.println(" ");         
        #endif 
      }
    }
}