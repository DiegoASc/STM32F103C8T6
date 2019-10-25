#include "Delay.h"
int systime;

void SysTime (void){
    static int systime_=0;
    systime_++;
    if(systime_>=1000){
        systime_=0;
        systime++;
    }
}