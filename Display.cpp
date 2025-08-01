#include "Display.h"

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

NonBlockingDelay SwitchSelDelay(3000);

void Init_Display()
{
  u8g2.begin();
  u8g2.setFont(u8g2_font_NokiaSmallPlain_tf); //7x8
  u8g2.clearDisplay();

  Display_Header();
  Display_Line(2, "Initializing...", 8);
  Display_SendAndClearBuffer();
}

void Display_Line(int line, String text, int indent)
{
  int py = line * 10;
  u8g2.setCursor(indent, py);
  u8g2.println(text);
  //u8g2.sendBuffer();
}

void Display_Header() 
{
  Display_Line(1, String(SOFTWAREVERSION)+String(FS_FILENAME) +" " + String(OpMode));
}

void Display_SendAndClearBuffer()
{
  Display_SendBuffer();
  Display_ClearBuffer();
}

void Display_SendBuffer()
{
  u8g2.sendBuffer();
}

void Display_ClearBuffer()
{
  u8g2.clearBuffer();
}

void Display_ClearDisplay()
{
  u8g2.clearDisplay(); 
}

void Display_Actualize() 
{ 
  bool saveActive = false;
  Display_ClearBuffer();
  Display_Header();

  switch (RegisterCommand)
  {
    case 0x7f: //ESP-IO:
    { 
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":" + String(hexBuffer) + "h"+ " " + names[currentRow]  );    
      Display_Line(3, "MotorOnChain " +String(StateMotorOnfromIB) + " " + String(StateMotorOnfromESP2ENI) + " " + String(StateMotorOnfromENO2PB));
      Display_Line(4, "TMCStatus" + String(StateTMCStatus) + "V");



      KeyEvent = true; //Diplay screen continously
      break;
    }



    case 0x7b: //RDADCPB: //Then display Values from ADC on PBb, register7b
    { 
      //Display_Line(1,  String(((timeCntAds1115))/1000) + "ms ConversionTime ADC0");

      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );    
      Display_Line(2, "Err:" + String(ADCPBErrorCounter) , 90);

      if (((float)ADCval[0])/Divider4Volt < 1.5) // TEMP ADC under 0.5V then show Error on Display
      {
        Display_Line(3, "TEMP: ERROR");
        Display_Line(3, String(((float)ADCval[0])/Divider4Volt,2) + "V", 98);
      }
      else
      {
        Display_Line(3, "TEMP: " + String(ADC0average, 2) + "C");
        Display_Line(3, String(((float)ADCval[0])/Divider4Volt,2) + "V", 98);
      }

      Display_Line(4, "MotorU: " + String(ADC1average, 2) + "V");
      Display_Line(4, String(((float)ADCval[1])/Divider4Volt,2) + "V", 98);

      Display_Line(5, "MotorI: " + String(((float)ADC2average),2) + "A");
      Display_Line(5, String(((float)ADCval[2])/Divider4Volt,2) + "V", 98);

      Display_Line(6, "Disable-ITRIP: " + String(((float)ADC3average),2) + "V");
      Display_Line(6, String(((float)ADCval[3])/Divider4Volt,2) + "V", 98);

      KeyEvent = true; //Diplay screen continously
      break;
    }

    case 0x7a: //Loop Time:
    { 
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );    
      
      Display_Line(3, "Display Task = " + String(LoopCycle) + "us");
      Display_Line(3, String(LoopFrequency) + "Hz", 90);
      Display_Line(4, "WIFI Task    = " + String(LoopCycle) + "us");
      Display_Line(4, String(LoopFrequency) + "Hz", 90);
      Display_Line(5, "Loop Task    = " + String(LoopCycle) + "us");
      Display_Line(5, String(LoopFrequency) + "Hz", 90);
      Display_Line(6,  "ConversionTimeADC0 "  + String(((timeCntAds1115))/1000)+ "ms");

      KeyEvent = true; //Diplay screen continously
      break;
    }
      
  
    case 0x76: //#122 PORTIBx21:
    { 
      // Values are actualized in Main.cpp
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":" + String(hexBuffer) + "h"+ " " + names[currentRow]  );
      Display_Line(2, "Err" + String(IB_PCF8574_0X21ErrorCount), 90);
      
      if (((millis()/1000) % 2) == 0 ) 
      { 
        Display_Line(3, "OUT1ServoRdy: " + String( IB_PCF8574_0X21_PortValues[0])); //P0  Output
      }
      else 
      { 
        Display_Line(3, "OUT1ServoRdy: " + String( !IB_PCF8574_0X21_PortValues[0])); //P0  Output
      } 
      
      Display_Line(4, "IN1SafeStp:       " + String( IB_PCF8574_0X21_PortValues[4]));   //P4  Input
      Display_Line(5, "MotorOvrTemp: " + String( IB_PCF8574_0X21_PortValues[3])); //P3  Input
      Display_Line(6, "MotorON:          " + String( IB_PCF8574_0X21_PortValues[7]));      //P7  Input

      Display_Line(3, "OUT2: " + String( IB_PCF8574_0X21_PortValues[1]), 80);     //P1  Output
      Display_Line(4, "OUT3: " + String( IB_PCF8574_0X21_PortValues[2]), 80);     //P2  Output
      Display_Line(5, "IN2: " + String( IB_PCF8574_0X21_PortValues[5]), 88);      //P5  Input
      Display_Line(6, "IN3: " + String( IB_PCF8574_0X21_PortValues[6]), 88);      //P6  Input
    
      KeyEvent = true; //Diplay screen continously
      break;
    }

    case 0x74: //#121 WLAN Time
    { 
      time_t now = TimeClient.getEpochTime();
      struct tm *tm = localtime(&now);
      
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );
      Display_Line(4, "WLAN: " + String(WIFI_SSID)); // SSID
      Display_Line(5, "IP: " + WIFI_IPADDRESS.toString());
      Display_Line(6, "Time: " + String(tm->tm_year+1900) + "-" + String(tm->tm_mon + 1) + "-" + String(tm->tm_mday) + " " + String(tm->tm_hour) + ":" + String(tm->tm_min) + ":" + String(tm->tm_sec));

      #if SERIAL_REG_0x74 == 1
      //if (RegisterCommand == 0x74) {  Serial.printf("%02d:%02d:%02d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);}
      #endif

      KeyEvent = true; //Diplay screen continously
      break;
    }

    case 0x73: //#PWM Test:
    { 
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );
      Display_Line(4, "Frq: 10.0KHz");
      Display_Line(5, "Duty: " + String((float)SpeedAD/40.95, 2) + "%");

      KeyEvent = true; //Diplay screen continously
      break;
    }

    case 0x72: //EmptyRegister6:
    { 
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );    

      KeyEvent = true; //Display this screen continously!
      break;
    }
  
    case 0x71: //#118 HALLSensor Test:
    { 
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );    

      if (limitSwitchSEL.isReleased())
      {
        if (!SD.begin(CSSDPin)) 
        {
          Serial.println("SD-Karten-Initialisierung fehlgeschlagen!");
          return;
        }
        Serial.println("SD-Karte erfolgreich initialisiert.");

        
        
          Serial.println("Die Datei HALL.HEX wird erstellt");
          File file = SD.open("/HALL.HEX", FILE_WRITE);

          if (file) 
          {
            for (int i = 0; i < 85; i++)  {  file.print(01100001,BIN); }  
            for (int i = 0; i < 85; i++)  {  file.print(00100101,BIN); }  
            for (int i = 0; i < 85; i++)  {  file.print(00110100,BIN); }  
            for (int i = 0; i < 85; i++)  {  file.print(00010110,BIN); }  
            for (int i = 0; i < 85; i++)  {  file.print(01010010,BIN); }  
            for (int i = 0; i < 85; i++)  {  file.print(01010101,BIN); }  
            for (int i = 0; i < 85; i++)  {  file.print(01010101,BIN); }  
            for (int i = 0; i < 85; i++)  {  file.print(01000011,BIN); }  
            for (int i = 0; i < 85; i++)  {  file.print(01010101,BIN); }  
            for (int i = 0; i < 85; i++)  {  file.print(01010101,BIN); }  
            for (int i = 0; i < 85; i++)  {  file.print(01010101,BIN); }  
            
            file.close();
            Serial.println("HALL.HEX wurde erfolgreich erstellt und mit Werten gefüllt.");
          } else 
          { Serial.println("Fehler beim Erstellen von HALL.HEX!"); }
        
      } //END if (limitSwitchSEL.isReleased())

      //  HALL Sensor Test
      hallU = digitalRead(HALL_U_PIN);
      hallV = digitalRead(HALL_V_PIN);
      hallW = digitalRead(HALL_W_PIN);
      HALL = hallU + hallV*2 + hallW*4;

      Display_Line(3, "U" + String(hallU) + " V" + String(hallV) + " W" + String(hallW) + "   " + String(HALL));
      if (HALL >6 | HALL==0)  { Display_Line(4, "HALL Error!"); }

      receivedValue = SPIreadRegister(0x27); //Read ABNCount   
      Display_Line(5, "ABNCount: " + String(receivedValue, HEX) + "h " + String(receivedValue, DEC) + "d");   
      receivedValue = SPIreadRegister(0x28); //Read ABNCount from Refpulse of Encoder

      Display_Line(6, "ABNCountN: " + String(receivedValue, HEX) + "h " + String(receivedValue, DEC) + "d");
      
      KeyEvent = true; //Display this screen continously!
      break;
    }
  
    case 0x70: //EmptyRegister8:
    { 
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );    

      KeyEvent = false; //Display this screen once!
      break;
    }

    case 0x03: 
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );    
      Display_Line(3, "WR Reg: " + String(hexBuffer) + "h" +" with: " + String(formattedValue));
      
      if(d == 0)  //Read I_RAW and Offset
      {
        Display_Line(4, "I1_RAW: " + String((WRreceivedValue>>16)&0xffff,HEX) + "h");
        int dummy = (WRreceivedValue>>16)&0xffff - (MB_REGISTER_0X08_OFFSET&0xffff);
        Display_Line(4, String((WRreceivedValue>>16)&0xffff,DEC) + "d" , 70);
        



        Display_Line(5, "I0_RAW: " + String(WRreceivedValue&0xffff,HEX) + "h");


        Display_Line(6, "Offset: " + String((MB_REGISTER_0X08_OFFSET&0xffff), HEX) + "h");
        Display_Line(6, "Offset: " + String((MB_REGISTER_0X09_OFFSET&0xffff), HEX) + "h", 70);
        
        if (limitSwitchSEL.isReleased())
        {
          MB_REGISTER_0X08_OFFSET = 0xFEFF0000^((WRreceivedValue>>16)&0xffff);
          MB_REGISTER_0X09_OFFSET = 0xFEFF0000^(WRreceivedValue&0xffff);

          WriteDataToTMC(0x08, MB_REGISTER_0X08_OFFSET);
          WriteDataToTMC(0x09, MB_REGISTER_0X09_OFFSET);
        }

        #if Serial_REG_0x03  ==1
          Serial.print("ADC_I1_RAW:"); Serial.print(((WRreceivedValue>>16)&0xffff),HEX); Serial.print("h");
          Serial.print("ADC_I0_RAW:"); Serial.print(WRreceivedValue&0xffff,HEX); Serial.print("h");
        #endif 
      }
      else if(d == 1) //Read VM_Raw and AnalogInput_Raw
      {
        Display_Line(5, "ADC_AGPI_A_RAW: " + String((WRreceivedValue>>16)&0xffff,HEX) + "d");
        int dummy = (WRreceivedValue&0xffff - 0x8000) / 70;
        Display_Line(6, "ADC_VM_RAW: " + String(WRreceivedValue&0xffff,DEC) + "d " + String(dummy,DEC) + "V" );

        #if Serial_REG_0x03 ==1
          Serial.print("ADC_AGPI_A_RAW:");Serial.print(((WRreceivedValue>>16)&0xffff),HEX);Serial.print("h");
          Serial.print("ADC_VM_RAW:");Serial.print(WRreceivedValue&0xffff,HEX);Serial.println("h");
        #endif
      }
      KeyEvent = true; //Diplay screen continously
    break;

    case 0x08: //#8 ADC_I1 Scale, Offset
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );    
      Display_Line(3, "WR Reg: " + String(hexBuffer) + "h");
      Display_Line(4, "with: " + String(formattedValue));

      Display_Line(5, "Scale: " + String((WRreceivedValue>>16)&0xffff,HEX) + "h " + String((WRreceivedValue>>16)&0xffff,DEC) + "d");
      Display_Line(6, "Offset: " + String(WRreceivedValue&0xffff,HEX) + "h");
      
      #if Serial_REG_0x08 ==1
        Serial.print("Scale:");Serial.print(((WRreceivedValue>>16)&0xffff),HEX);Serial.print("h");
        Serial.print("Offset:");Serial.print(WRreceivedValue&0xffff,HEX);Serial.println("h");
      #endif
      KeyEvent = true; //Diplay screen continously  
    break;

    case 0x09: //#9 ADC_I0 Scale, Offset
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );    
      Display_Line(3, "WR Reg: " + String(hexBuffer) + "h");
      Display_Line(4, "with: " + String(formattedValue));
      
      Display_Line(5, "Scale: " + String((WRreceivedValue>>16)&0xffff,HEX) + "h " + String((WRreceivedValue>>16)&0xffff,DEC) + "d");
      Display_Line(6, "Offset: " + String(WRreceivedValue&0xffff,HEX) + "h");
      
      #if Serial_REG_0x09 ==1
        Serial.print("Scale:");Serial.print(((WRreceivedValue>>16)&0xffff),HEX);Serial.print("h");
        Serial.print("Offset:");Serial.print(WRreceivedValue&0xffff,HEX);Serial.println("h");
      #endif
      KeyEvent = true; //Diplay screen continously  
    break;

    case 0x1B: //#2 Mode, Polepairs
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );    
      Display_Line(3, "WR Reg: " + String(hexBuffer) + "h");
      Display_Line(4, "with: " + String(formattedValue));
      Display_Line(5, "Mode: " + String((WRreceivedValue>>16)&0xffff,HEX) + "h " + String((WRreceivedValue>>16)&0xffff,DEC) + "d"
                   +" PolePairs: " + String(WRreceivedValue&0xffff,HEX) + "h " + String(WRreceivedValue&0xffff,DEC) + "d");
      Display_Line(6, "SEL 2, 4, 6, 8, 10" ); // Add here 2, 4, 6, 8, 10
      
      if(!limitSwitchSEL.getState() ) { Display_Line(6, "SEL-Step: 2, 4, 6, 8, 10" );}

      #if Serial_REG_0x1b ==1
        Serial.print("Mode: ");Serial.print(((WRreceivedValue>>16)&0xffff),HEX);Serial.print("h");
        Serial.print("PolePairs: ");Serial.print(WRreceivedValue&0xffff,HEX);Serial.println("h");
      #endif

      saveActive = true; // Save routine with Select switch active
      KeyEvent = false; //Display this screen once!
    break;

    case 0x24: //#15 UD-EXT-VALUE
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );    
      Display_Line(3, "WR Reg: " + String(hexBuffer) + "h" +"with: " + String(formattedValue));
      Display_Line(4, "VM Effective: " + String( (ADC1average*WRreceivedValue) / 0x7fff) + "V" );
      Display_Line(5, "RxValue: " + String(WRreceivedValue,HEX) + "h ");     
      Display_Line(5, String(WRreceivedValue,DEC) + "d", 75);
      Display_Line(6, "SEL-Step: 1, 10, 100, 1000" ); // Add here with limits 0, 5000, inc 1,10,100,1000
      
      if(limitSwitchUP.isPressed() && !limitSwitchSEL.getState())
      {
        long value = strtoul(values[currentRow], NULL, HEX);
        if (value >= 1 && value < 10) { value += 1; } 
        else if (value >= 10 && value < 100) { value += 10; } 
        else if (value >= 100 && value < 1000) { value += 100; }
        else if (value >= 1000 && value < 10000) { value += 1000; }
        if (value > 10000) value = 10000;
        strcpy(values[currentRow], String(value, HEX).c_str());
      }
      else if (limitSwitchDWN.isPressed() && !limitSwitchSEL.getState())
      {
        long value = strtoul(values[currentRow], NULL, HEX);
        if (value > 100 && value <= 1000) { value -= 100; } 
        else if (value > 10 && value <= 100) { value -= 10; } 
        else if (value > 1 && value <= 10) { value -= 1; }
        else if (value > 1000 && value <= 10000) { value -= 1000; }
        if (value < 1) value = 1;
        strcpy(values[currentRow], String(value, HEX).c_str());
      }

      saveActive = true; // Save routine with Select switch active
      KeyEvent = true; //Refresh this screen
    break;

    case 0x21: //#17 Speed Target
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );    
      Display_Line(3, "WR Reg: " + String(hexBuffer) + "h");
      Display_Line(4, "with: " + String(formattedValue));
      Display_Line(5, "RxValue: " + String(WRreceivedValue,HEX) + "h ");     
      Display_Line(5, String(WRreceivedValue,DEC) + "d", 75);
      Display_Line(6, "SEL-Step: 1, 10, 100" ); // Add here with limits 0, 1000, inc 1,10,100

      //Fabian, bitte so kompakt programmieren wenn möglich!
      if(limitSwitchUP.isPressed() && !limitSwitchSEL.getState())
      {
        long value = strtoul(values[currentRow], NULL, HEX);
        if (value >= 1 && value < 10) { value += 1; } 
        else if (value >= 10 && value < 100) { value += 10; } 
        else if (value >= 100 && value < 1000) { value += 100; }
        if (value > 1000) value = 1000;
        strcpy(values[currentRow], String(value, HEX).c_str());    
      }
      else if (limitSwitchDWN.isPressed() && !limitSwitchSEL.getState())
      {
        long value = strtoul(values[currentRow], NULL, HEX);
        if (value > 100 && value <= 1000) { value -= 100; } 
        else if (value > 10 && value <= 100) { value -= 10; } 
        else if (value > 1 && value <= 10) { value -= 1; }
        if (value < 1) value = 1;
        strcpy(values[currentRow], String(value, HEX).c_str());  
      }

      saveActive = true; // Save routine with Select switch active
      KeyEvent = true; //Display this screen once!
    break;

    default: //if not defined by case...
      Display_Line(2, "#" + String(data[currentRow][0],DEC) + ":"+ String(hexBuffer) + "h"+ " " + names[currentRow]  );    
  
      if (data[currentRow][2] == 0)
      {
        Display_Line(3, "RD Reg: " + String(hexBuffer) + "h");
      }
      else if (data[currentRow][2] == 1)
      {
        Display_Line(3, "WR Reg: " + String(hexBuffer) + "h");
      }
      
      if (data[currentRow][2]==0) //Only Read
      {
        Display_Line(5, "RxValue: " + String(receivedValue,HEX) + "h ");     
        Display_Line(5, String(receivedValue,DEC) + "d", 75);
        
        KeyEvent = true; //Display this screen once!
      }
      else if (data[currentRow][2]==1) //Write
      {
        Display_Line(4, "with: " + String(formattedValue));
        Display_Line(5, "RxValue: " + String(WRreceivedValue,HEX) + "h ");     
        Display_Line(5, String(WRreceivedValue,DEC) + "d", 75);
        //Serial.print("RxValue:");Serial.print(WRreceivedValue,HEX);Serial.println("h");
        KeyEvent = true; //Display this screen once!
      }      
    break;
  } // END switch (RegisterCommand)

  if (saveActive)
  {
    if(limitSwitchSEL.getState())
      {
        //reset delay of select switch when switch is not pressed
        SwitchSelDelay.reset();
      }
      else if (!limitSwitchSEL.getState())
      {
        //reset delay of select switch when up or down switch is pressed
        if (limitSwitchDWN.isPressed() || limitSwitchUP.isPressed())
        {
          SwitchSelDelay.reset();
        }

        //execute saving of new value
        if (SwitchSelDelay.check())
        {
          Serial.println("SELECT 3 seconds pressed");
          FileSystem_WriteRegisterValue(FS_FILENAME, currentRow);          
          SwitchSelDelay.reset();
        }
      }
  }
  

  WRreceivedValue = receivedValue = 0;
  Display_SendBuffer();
  serial0Flush(); 

} //End Actualize Display and Serial /////////////////

uint8_t Display_NextPage()
{
  return u8g2.nextPage();
}