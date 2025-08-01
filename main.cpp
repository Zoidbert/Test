
#ifndef __CONFIG_H__
  #include "Config.h"
#endif
#ifndef __DISPLAY_H__
  #include "Display.h"
#endif
#ifndef __INPUTBOARD_H__
  #include "InputBoard.h"
#endif
#ifndef __POWERBOARD_H__
  #include "PowerBoard.h"
#endif
#ifndef __MAINBOARD_H__
  #include "MainBoard.h"
#endif
#ifndef __FILESYSTEM_H__
  #include "FileSystem.h"
#endif
#ifndef __SWITCHES_H__
  #include "Switches.h"
#endif

void setup() {
  Serial.begin(115200);  
  Serial.println("Start of SETUP");
  Serial.println("Serial activated 115200 baud");
	//IB_PCF8574_0x21.pinMode(P0, OUTPUT);
	//IB_PCF8574_0x21.pinMode(P1, INPUT);
  


  pinMode(CSSDPin, OUTPUT);  digitalWrite(CSSDPin, HIGH);                       //CS SPI SD
  pinMode(CSTMCPin, OUTPUT);  digitalWrite(CSTMCPin, HIGH);                       //CS SPI on TMC
  pinMode(MotorOnfromIB, INPUT_PULLUP);                                          // Read MotorOn Signal from IB
  pinMode(MotorOnfromENO2PB, INPUT_PULLUP);                                     // Read MotorOn Signal from ENO of TMC to PB
  pinMode(TMCStatus, INPUT_PULLUP);                                             // TMC Status  to read into ESP
  pinMode(MotorOnfromESP2ENI, OUTPUT);  digitalWrite(MotorOnfromESP2ENI, LOW); // MotorON Output from ESP to ENI on TMC
  pinMode(FanON2PB, OUTPUT);  digitalWrite(FanON2PB, LOW);                      // Fan ON to PB
  pinMode(MotorBrakeON2PB, OUTPUT);  digitalWrite(MotorBrakeON2PB, LOW);       // Motor Brake ON from ESP to PB
  pinMode(ShuntOnfromESP, OUTPUT);  digitalWrite(ShuntOnfromESP, LOW);          // Shunt ON from ESP to PB



  //hier PWM ausschalten for Soft Restart

  Init_Switches(); 
  Init_Display();  
  Init_FileSystem();
  Init_MainBoard();
  Init_InputBoard();
  Init_PowerBoard();

  // #define LEDC_SPEED_MODE LEDC_HIGH_SPEED_MODE
  // #define LEDC_CHANNEL LEDC_CHANNEL_0
  // #define LEDC_FREQUENCY          (5000*2) // Frequency in Hertz. Set frequency at 5 kHz
  // #define LEDC_OUTPUT_IO          (32) // Define the output GPIO
  // #define LEDC_DUTY               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095



  // // // Prepare and then apply the LEDC PWM timer configuration
  //  ledc_timer_config_t timer_initer={
  //    .speed_mode=LEDC_SPEED_MODE,        //ä½éæ¨¡å¼
  //    .duty_resolution=LEDC_TIMER_12_BIT,     //10bitçåè¾¨ç(å¯ä»¥ä¸º1~15)
  //    .timer_num= LEDC_TIMER_0,                //éæ©å®æ¶å¨
  //    .freq_hz=LEDC_FREQUENCY,                          //1000Hz
  //    .clk_cfg=LEDC_USE_APB_CLK                 //èªå¨éæ©æ¶éæº 
  //    };
  //    ledc_timer_config(&timer_initer);

  //    ledc_channel_config_t channel_initer={
  //      .gpio_num=LEDC_OUTPUT_IO,                           //GPIO18
  //      .speed_mode=LEDC_SPEED_MODE,        //ä½éæ¨¡å¼
  //      .channel=LEDC_CHANNEL,                             //0~7ä¸å±å«ä¸ªéé,éä¾¿éä¸ä¸ªå°±è¡
  //      .intr_type=LEDC_INTR_DISABLE,           //ä¸éè¦ä¸­æ­
  //      .timer_sel= LEDC_TIMER_0,             //éæ©å®æ¶å¨
  //      .duty=LEDC_DUTY,                             //ä¸é¢éæ©äº10bitçåè¾¨ç,å æ­¤å ç©ºæ¯éæ©èå´ä¸º0~1023
  //      .hpoint=0                           //éä¾¿å¡«
  //    };
  //    ledc_channel_config(&channel_initer);

  //HALL Inputs define
  pinMode(HALL_U_PIN, INPUT);
  pinMode(HALL_V_PIN, INPUT);
  pinMode(HALL_W_PIN, INPUT);





  
  #if WIFION == 1 
  Serial.print("Wifi Scan Networks");
  int num =0;
  num = WiFi.scanNetworks();
  Serial.print("The number of available WIFI around is: ");
  Serial.println(num);
  Serial.print("The WIFI information is:");
  for(int a=0;a<=num;a++) {Serial.print(WiFi.SSID(a)); Serial.println();}
  Serial.println("WifiInit end");
  #endif

  Serial.println("SHORTKEYS for SERIALMONITOR");
  Serial.println("u: Key Up");
  Serial.println("d: Key Down");
  Serial.println("o: Motor On");
  Serial.println("f: Motor Off");
  FirstRun = 1;
    currentRow = -1;
} //END SETUP







void loop() 
{
  limitSwitchUP.loop(); // MUST call the loop() function first
  limitSwitchSEL.loop(); // MUST call the loop() function first
  limitSwitchDWN.loop(); // MUST call the loop() function first
  

  
  
  
 

  //Serial.print("SpeedAD: ");Serial.println(SpeedAD);
  // ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0,SpeedAD);///// Generate PWM
  // ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
  //END Generate PWM
  Read_ADSpeed();
  Read_Write_PCF8574_0x21();    
  Read_ADS1115();

  unsigned long timecnt = 0;   
  timecnt = micros();// loop begin(us)

  StateMotorOnfromIB = digitalRead (MotorOnfromIB);
  StateMotorOnfromESP2ENI = digitalRead (MotorOnfromESP2ENI);
  StateMotorOnfromENO2PB = digitalRead (MotorOnfromENO2PB);

  StateTMCStatus = digitalRead (TMCStatus);
  StateFanON2PB = digitalRead (FanON2PB);
  StateMotorBrakeON2PB = digitalRead (MotorBrakeON2PB);
  StateShuntOnfromESP = digitalRead (ShuntOnfromESP);
  
  //Select Operation Mode MotorOn or MotorOff///////////////////////////////////
    if (Serial.available() > 0)   { IncomingKey = Serial.read();  }  // read the incoming serial byte:  
    if (IncomingKey == 13)   {    IncomingKey = 0;    }
    if ((IncomingKey == 'o') || (StateMotorOnfromIB == 1))
    {
      if (OpMode == 0) //{ return; } //If already in OpMode 1, then return and do nothing
      //else 
      {
        OpMode = 1;
        Serial.println("Event OpMode MotorOn");
        digitalWrite(MotorOnfromESP2ENI, HIGH);
        Display_ClearDisplay(); Display_Header(); 
        Display_Line(2, "Motor Enabled " + String(StateMotorOnfromIB) + " " + String(StateMotorOnfromESP2ENI) + " " + String(StateMotorOnfromENO2PB));
        Display_SendBuffer(); serial0Flush();
      }
    } //End if ((IncomingKey == 'o') || (StateMotorOnfromIB == 1))

    if ((IncomingKey == 'f') || (StateMotorOnfromIB == 0))//=Off
    {
      //if (OpMode == 1)  {ESP.restart();} //Duration too long!


      if (OpMode == 1) //{ return; } //If already in OpMode 0, then return and do nothing
      //else 
      {
        OpMode = 0;  
        digitalWrite(MotorOnfromESP2ENI, LOW);  
        Serial.println("Event OpMode Motor Off");
        serial0Flush();
        Display_Actualize();
      }
    }
  //END Select Operation Mode Mode MotorOn or MotorOff///////////////////////////////////


if (OpMode == 1) 
  {



  } //END if (OpMode == 1) 



  //(OpMode == 0) Mode MotorOff//////////////////////////////////////
  if (OpMode == 0 || OpMode == 1)
  { 
    if ((limitSwitchUP.isPressed() && limitSwitchSEL.getState()) || IncomingKey == 'u')
    {
      if (IncomingKey == 'u') 
      {
        IncomingKey = 0; 
        serial0Flush();
      }
      currentRow = (currentRow + 1) % numberOfRows;
      KeyEvent = true;
      Serial.print("UpButton pressed ");
      Serial.print("currentRow: ");
      Serial.print(currentRow);
      Serial.print(" DataRow: ");
      Serial.println(data[currentRow][0],DEC);
    }
    else if ((limitSwitchDWN.isPressed() && limitSwitchSEL.getState()) || IncomingKey == 'd')
    {
      if (IncomingKey == 'd') 
      {
        IncomingKey = 0; 
        serial0Flush();
      }
      // currentRow ist Initial -1, damit der erste Tastendruck nach unten bei 1 landet. Dafür muss hier ein check passieren, dass currentRow auf 0 gesetzt wird wenn es noch die initiale -1 hat.
      if (currentRow == -1) {currentRow = 0;}
      
      currentRow = (currentRow - 1 + numberOfRows) % numberOfRows;
      KeyEvent = true;
      Serial.print("DownButton pressed ");
      Serial.print("currentRow: ");
      Serial.print(currentRow);
      Serial.print(" DataRow: ");
      Serial.println(data[currentRow][0],DEC);
    }
    
    if (KeyEvent == true)
    {
      KeyEvent = false; //Display the screen once! For displaying continously, put into the screen: KeyEvent = true;

      //Send data to TMC
      TransferData2TMC();
      
      Display_Actualize();
    }
    
  } //End if (OpMode == 0) 

} //Loop
//===================================



void serial0Flush(){
  while(Serial.available() > 0)
  {
    char t = Serial.read();
  }
}

#if WIFION == 1  

void WifiTaskHandler(void *pvParameters) {  // This is a task.
  int tmp=0,flag = 0;

  WiFi.begin(ssd,pasd);
  for (;;) 
  {
    // while(Serial.available()>0)
    // {
    //   String SD=Serial.readStringUntil(','); 
    //   const char *ssd=SD.c_str();

    //   delay(100);

    //   String PSD=Serial.readStringUntil('\0');
    //   const char *pasd=PSD.c_str();
    //   delay(1000);
    //   Serial.println("WIFI:");
    //   Serial.println(SD);
    //   Serial.println("PSD:");
    //   Serial.println(PSD);
    //   WiFi.begin(ssd,pasd);
    // }
    
    while(WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
      tmp++;
      if(tmp > 10)
      {
        Serial.print("connected timeout!\n");
        tmp = 0;
        break;
      }
    }
    if(flag == 0)
    {
      flag = 1;
      //Serial.print("WIFI connected success!\n");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
    //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    xTimerStart(NTPServerTmr, 0);
    }
  delay(2000);
  }
}
#endif

#if WIFION == 1
void NTPServerTmr_Func(TimerHandle_t xTimer)
{
  struct tm timeinfo;

   if (!getLocalTime(&timeinfo))
   {
  //   Serial.println("Failed to obtain time");
     return;
   }
  // //Serial.println(&timeinfo, "%F %T %A");
  
  return;
}

#endif

