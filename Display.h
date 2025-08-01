#include "U8g2lib.h"

#ifndef __DISPLAY_H__
    #define __DISPLAY_H__

    extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;
#endif

#include "SD.h"

#ifndef __CONFIG_H__
    #include "Config.h"
#endif

#ifndef __INPUTBOARD_H__
    #include "InputBoard.h"
#endif

#ifndef __SWITCHES_H__
    #include "Switches.h"
#endif

#ifndef __MAINBOARD_H__
    #include "MainBoard.h"
#endif

#ifndef __FILESYSTEM_H__
    #include "FileSystem.h"
#endif

void Init_Display();
void Display_Line(int line, String text, int indent = 0);
void Display_Header();
void Display_SendAndClearBuffer();
void Display_SendBuffer();
void Display_ClearBuffer();
void Display_Actualize ();
void Display_ClearDisplay();
uint8_t Display_NextPage();