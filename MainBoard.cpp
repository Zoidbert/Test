#include "MainBoard.h"

int MB_REGISTER_0X08_OFFSET = 0x0;
int MB_REGISTER_0X09_OFFSET = 0x0;

char* WIFI_SSID;
char* WIFI_PASSWORD;
long UtcOffsetInSeconds = 7200; // 3600 for MEZ (Winter time) or 7200 for MEZ (Summer time)
unsigned long UpdateIntervalMs = 60000; // update every 1 minute
WiFiUDP NtpUdp;
NTPClient TimeClient(NtpUdp, "pool.ntp.org", UtcOffsetInSeconds, UpdateIntervalMs);
// Definition of Task-Handle (optional)
TaskHandle_t xNTPTaskHandle = NULL;
IPAddress WIFI_IPADDRESS;

void Init_MainBoard()
{
    //Initializes the SPI bus by setting SCK, MOSI, and CS to outputs, pulling SCK and MOSI low, and SS high. 
    SPI.begin(); 
    SPI.setDataMode(SPI_MODE3);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV16);

    //InitRegisters TMC
    //=============
    Serial.println("Init TMC4671");
    currentRow = 0;
    while (currentRow < numRows){         
        if (data[currentRow][2]==1) {
        Serial.print(currentRow); Serial.print(" #"); Serial.print(data[currentRow][1],HEX);
        Serial.print(" ");  Serial.print (data[currentRow][2]);  //Serial.print("W");  
        Serial.print(" "); Serial.println (names[currentRow]);    
            TransferData2TMC();
        }

        //Initialize ADC I1 Offset from file
        if (data[currentRow][0] == 8)
        {
          MB_REGISTER_0X08_OFFSET = strtoul(values[currentRow], NULL, HEX);
        }
        //Initialize ADC I0 Offset from file
        if (data[currentRow][0] == 9)
        {
          MB_REGISTER_0X09_OFFSET = strtoul(values[currentRow], NULL, HEX);
        }

        // Initialize WIFI SSID and Password
        if (data[currentRow][0] == 121)
        {
          String wifi_credentials = String(names[currentRow]);
          int atIndex = wifi_credentials.indexOf('@'); 
          if (atIndex != -1)
          {
            int ssid_bufferSize = wifi_credentials.substring(0, atIndex).length() + 1;
            WIFI_SSID = new char[ssid_bufferSize];
            wifi_credentials.substring(0, atIndex).toCharArray(WIFI_SSID, ssid_bufferSize);

            int password_bufferSize = wifi_credentials.substring(atIndex + 1).length() + 1;
            WIFI_PASSWORD = new char[password_bufferSize];
            wifi_credentials.substring(atIndex + 1).toCharArray(WIFI_PASSWORD, password_bufferSize);
          }
        }
        
        currentRow++; //Serial.println(currentRow);
    }

    Display_Line(5, "TMC4671 Init Registers. Done.", 8);    
    Display_SendBuffer();

    // ledcSetup(MB_LEDC_CHANNEL, MB_LEDC_FREQUENCY, LEDC_TIMER_12_BIT);
    // ledcAttachPin(MB_LEDC_OUTPUT_IO, MB_LEDC_CHANNEL);

    // // Prepare and then apply the LEDC PWM timer configuration
     ledc_timer_config_t timer_initer={
       .speed_mode=MB_LEDC_SPEED_MODE,        //ä½éæ¨¡å¼
       .duty_resolution=LEDC_TIMER_12_BIT,     //10bitçåè¾¨ç(å¯ä»¥ä¸º1~15)
       .timer_num= LEDC_TIMER_0,                //éæ©å®æ¶å¨
       .freq_hz=MB_LEDC_FREQUENCY,                          //1000Hz
       .clk_cfg=LEDC_USE_APB_CLK                 //èªå¨éæ©æ¶éæº 
       };
       ledc_timer_config(&timer_initer);

       ledc_channel_config_t channel_initer={
         .gpio_num=MB_LEDC_OUTPUT_IO,                           //GPIO18
         .speed_mode=MB_LEDC_SPEED_MODE,        //ä½éæ¨¡å¼
         .channel=MB_LEDC_CHANNEL,                             //0~7ä¸å±å«ä¸ªéé,éä¾¿éä¸ä¸ªå°±è¡
         .intr_type=LEDC_INTR_DISABLE,           //ä¸éè¦ä¸­æ­
         .timer_sel= LEDC_TIMER_0,             //éæ©å®æ¶å¨
         .duty=MB_LEDC_DUTY,                             //ä¸é¢éæ©äº10bitçåè¾¨ç,å æ­¤å ç©ºæ¯éæ©èå´ä¸º0~1023
         .hpoint=0                           //éä¾¿å¡«
       };
       ledc_channel_config(&channel_initer);

    Serial.println("TMC4671 Init Registers. Done  "); 
    //END InitRegisters TMC

  // Start init WIFI
  xTaskCreatePinnedToCore(
    ntpSyncTask,   /* Task function. */
    "NTPSync",     /* name of task. */
    4096,          /* Stack size of task */
    NULL,          /* parameter of the task */
    1,             /* priority of the task */
    &xNTPTaskHandle, /* Task handle to keep track of created task */
    0);            /* core where task should run (0 or 1 on ESP32) */

  // End init WIFI
}

// Task function for WLAN connection and NTP synchronization
void ntpSyncTask(void *pvParameters) {
  Serial.println("NTP Sync Task started.");

  // Connect to Wi-Fi (may take longer, but only blocks this task)
  Serial.print("Connecting to WIFI: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int connectionAttempts = 0;
  const int maxConnectionAttempts = 60; // 60 * 500ms = 30 seconds timeout

  while (WiFi.status() != WL_CONNECTED && connectionAttempts < maxConnectionAttempts) {
    vTaskDelay(pdMS_TO_TICKS(500)); // 500ms warten
    Serial.print(".");
    connectionAttempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) 
  {
    WIFI_IPADDRESS = WiFi.localIP();

    Serial.println("\nWIFI connected!");
    Serial.print("IP-Address: ");
    Serial.println(WIFI_IPADDRESS);

    // start NTP-Client
    TimeClient.begin();

    // Infinite loop for the task
    for (;;) {
      // The time is updated here
      TimeClient.update();
      Serial.print("NTP Task: time refreshed: ");
      Serial.println(TimeClient.getFormattedTime());
      vTaskDelay(UpdateIntervalMs); // Wait the set update interval
    }
  }
  else
  {
    Serial.println("\nError: Wi-Fi connection failed after 30 seconds. Task is terminated.");
    vTaskDelete(NULL); // Beende den aktuellen Task. NULL bedeutet "sich selbst beenden".
  }
  
}

void Read_ADSpeed()
{
  SpeedAD = analogRead(MB_Speed165VPin); 
  if(SpeedAD < 41) // Zwischen 1%
  {
    SpeedAD = 41;
  } 
  else if (SpeedAD > 4054) // und 99 Prozent begrenzen;
  {
    SpeedAD = 4054;
  }

  int pwmWave = map(SpeedAD, 0, 4095, 0, 255);

  ledc_set_duty(MB_LEDC_SPEED_MODE, MB_LEDC_CHANNEL, SpeedAD);
  ledc_update_duty(MB_LEDC_SPEED_MODE, MB_LEDC_CHANNEL);
  
  //ledcWrite(MB_LEDC_CHANNEL, pwmWave);
}

void WriteDataToTMC(byte registerCommand, int value)
{
  Serial.println("# START ##################################################");
  Serial.print("Register: "); Serial.print(registerCommand, HEX); Serial.print(" Value: "); Serial.println(value, HEX);
  
  char a2, b2, c2, d2;
  char formattedValue1[9];
  String valueString = String(value, HEX);
  valueString.toUpperCase();
  Serial.print("Value string: "); Serial.println(valueString);

  sprintf(formattedValue1, "%08s", valueString.c_str());

  Serial.print("Formatted Value string: "); Serial.println(formattedValue1);

  a2 =(charToHexDigit (*(formattedValue1+1))) + ((charToHexDigit (*(formattedValue1+0))) <<4); 
  b2 =(charToHexDigit (*(formattedValue1+3))) + ((charToHexDigit (*(formattedValue1+2))) <<4); //MSW 
  c2 =(charToHexDigit (*(formattedValue1+5))) + ((charToHexDigit (*(formattedValue1+4))) <<4); 
  d2 =(charToHexDigit (*(formattedValue1+7))) + ((charToHexDigit (*(formattedValue1+6))) <<4); //LSW , mode

  Serial.print("A2: "); Serial.print(a2, HEX);
  Serial.print(" B2: "); Serial.print(b2, HEX);
  Serial.print(" C2: "); Serial.print(c2, HEX);
  Serial.print(" D2: "); Serial.println(d2, HEX);

  digitalWrite(CSTMCPin, LOW); //CS SPI on TMC 

  SPI.transfer(registerCommand+128);
  delay(1);
  SPI.transfer(a2);
  SPI.transfer(b2);
  SPI.transfer(c2);
  SPI.transfer(d2);

  //digitalWrite(CSTMCPin, HIGH); //CS SPI on TMC 
  //digitalWrite(CSTMCPin, LOW); //CS SPI on TMC 
  digitalWrite(CSTMCPin, HIGH); //CS SPI on TMC 

  Serial.println("# END #############################################");
}

//Transfer Data to TMC ///////////////////////// 
//============================================== 
void TransferData2TMC () 
{ 
    RegisterCommand = data[currentRow][1];     
    //Serial.print("TransferData2TMC:"); Serial.print("RegisterCommand:"); Serial.println(RegisterCommand);
    
    // if (RegisterCommand==RDADCPB) 
    // { //Then display Values from ADC on PB
    // return;
    
    digitalWrite(CSTMCPin, LOW); //CS SPI on TMC 
    formatHexWithZeros(hexBuffer, data[currentRow][1], 2);  // Format hexadecimal value
    sprintf(formattedValue, "%08s", values[currentRow]);  // Format the value with leading zeros
        
    // a =(charToHexDigit (*(formattedValue+1))) + ((charToHexDigit (*(formattedValue+0))) <<4); 
    // b =(charToHexDigit (*(formattedValue+3))) + ((charToHexDigit (*(formattedValue+2))) <<4); //MSW 
    // c =(charToHexDigit (*(formattedValue+5))) + ((charToHexDigit (*(formattedValue+4))) <<4); 
    // d =(charToHexDigit (*(formattedValue+7))) + ((charToHexDigit (*(formattedValue+6))) <<4); //LSW , mode

    long value = strtoul(formattedValue, NULL, 16);
    a = (value >> 24) & 0xFF;
    b = (value >> 16) & 0xFF;
    c = (value >> 8) & 0xFF;
    d = value & 0xFF;

    if (data[currentRow][1] == 0x1b)  { b=3; } // PolePairs MSW =3 for Servomotors

    //Read Data from TMC ///////////////////////////
    if (data[currentRow][2]==0) { //Read Value from TMC    
        receivedValue = SPIreadRegister(RegisterCommand); //Send RegisterCommand and receive the value.  
        //Serial.print("receivedValue");   Serial.println(receivedValue,HEX); 
    } //End Read

    //Write Data to TMC ///////////////////////////
    if (data[currentRow][2]==1) {     //=Write Value to TMC
        RegisterCommand = data[currentRow][1] ;
        byte dummy =0; 
           
        dummy=SPI.transfer(RegisterCommand+128); 
        delay(1);
        dummy=SPI.transfer(a); dummy=SPI.transfer(b);  dummy=SPI.transfer(c); dummy=SPI.transfer(d);
        //if(RegisterCommand==0x1b) {dummy=SPI.transfer(a); dummy=SPI.transfer(b);  dummy=SPI.transfer(c); dummy=SPI.transfer(d);}//PolePairs

        digitalWrite(CSTMCPin, HIGH); //CS SPI on TMC 
        digitalWrite(CSTMCPin, LOW); //CS SPI on TMC 

        if ((RegisterCommand==0x01) || (RegisterCommand==0x03)) //ChipInfoData, AdcRawData
        {
        WRreceivedValue = SPIreadRegister(RegisterCommand-1);} //IF RegisterCommand=1 then read from Register 0
        else
        {
        WRreceivedValue = SPIreadRegister(RegisterCommand );} //Send RegisterCommand and read the value. 
        //Serial.print(" WRreceivedValue");   Serial.println(WRreceivedValue,HEX);  
    } //End Write Data to TMC ///////////////////////

    digitalWrite(CSTMCPin, HIGH); //CS SPI on TMC 
} // End TransferData2TMC /////////////////////////   

//Read SPI from register of TMC4671:
uint32_t SPIreadRegister(byte thisRegister)
{   digitalWrite(CSTMCPin, LOW); //CS SPI on TMC 
    byte  inByte = 0;           // incoming byte from the SPI
    unsigned long result = 0;   // result to return 
    SPI.transfer(thisRegister); // send to the device the register you want to read
    delay(1);
    result = SPI.transfer(0x00);  // send a value of 0 to read the first byte returned
    result = result << 8; inByte = SPI.transfer(0x00); result = result | inByte; 
    result = result << 8; inByte = SPI.transfer(0x00); result = result | inByte; 
    result = result << 8; inByte = SPI.transfer(0x00); result = result | inByte; 
    digitalWrite(CSTMCPin, HIGH); //CS SPI on TMC
    return(result);
}

void formatHexWithZeros(char *buf, unsigned long num, int width) {
  sprintf(buf, "%0*lX", width, num);
}

uint8_t charToHexDigit(char c){
  if (c >= 'A') 
    return c - 'A' + 10;  
  else 
    return c - '0';
  }