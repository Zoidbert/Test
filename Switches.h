#include <ezButton.h>

#ifndef __CONFIG_H__
    #include "Config.h"    
#endif

#ifndef __SWITCHES_H__
    extern ezButton limitSwitchUP;  // create ezButton o bject with Transition detect to port 4;
    extern ezButton limitSwitchDWN; //create ezButton object with transition detect to port 15;
    extern ezButton limitSwitchSEL; //create ezButton object with transition detect to port 0;
#endif

void Init_Switches();