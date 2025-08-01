#include "Switches.h"

ezButton limitSwitchUP(4);  // create ezButton o bject with Transition detect to port 4;
ezButton limitSwitchDWN(15); //create ezButton object with transition detect to port 15;
ezButton limitSwitchSEL(0); //create ezButton object with transition detect to port 0;

void Init_Switches()
{
    limitSwitchUP.setDebounceTime(DebounceTime); 
    limitSwitchDWN.setDebounceTime(DebounceTime);
    limitSwitchSEL.setDebounceTime(DebounceTime); 
    
}