#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#ifndef __MAINBOARD_H__
    #define __MAINBOARD_H__

    extern int MB_REGISTER_0X08_OFFSET;
    extern int MB_REGISTER_0X09_OFFSET;

    extern char* WIFI_SSID;
    extern char* WIFI_PASSWORD;
    extern long UtcOffsetInSeconds;
    extern unsigned long UpdateIntervalMs;
    extern WiFiUDP NtpUdp;
    extern NTPClient TimeClient;
    extern TaskHandle_t xNTPTaskHandle;
    extern IPAddress WIFI_IPADDRESS;
#endif

#ifndef __CONFIG_H__
    #include "Config.h"
#endif

#ifndef __DISPLAY_H__
    #include "Display.h"
#endif

void Init_MainBoard();
void TransferData2TMC();
uint32_t SPIreadRegister(byte thisRegister);
void formatHexWithZeros(char *buf, unsigned long num, int width);
uint8_t charToHexDigit(char c);
void Read_ADSpeed();
void WriteDataToTMC(byte registerCommand, int value);
void ntpSyncTask(void *pvParameters);