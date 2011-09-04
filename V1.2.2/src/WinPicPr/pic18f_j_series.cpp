#include "pic18f_j_series.h"

void JS::Connect(void){
    PIC_HW_SetClockAndData(false,false);
    PIC_HW_SetVdd(true);
    PIC_HW_Delay_50ns();                //P13=100ns
    PIC_HW_Delay_50ns();
    PIC_HW_SetVpp(true);
    PIC_HW_Delay_us(1);                 //P1=1us
    PIC_HW_SetVpp(false);
    PIC_HW_LongDelay_ms(4);             //P19=4ms
    JS::Write(0xc2b2);                  //Sends 0x4d43 but method Write sends lsb first, so the bits must be inverted
    JS::Write(0x0a12);                  //Sends 0x4850 but method Write send  lsb first, so the bits must be inverted
    PIC_HW_SetClockAndData(false,false);//Makes sure clock and data low - maybe an unneeded precaution
    PIC_HW_Delay_50ns();                //P20=50ns
    PIC_HW_SetVpp(true);
    PIC_HW_Delay_us(400);
}

void JS::Disconnect(void){
    PIC_HW_SetClockAndData(false,false);
    PIC_HW_Delay_50ns();
    PIC_HW_SetVpp(false);
    PIC_HW_Delay_us(40);
    PIC_HW_SetVdd(false);
}

