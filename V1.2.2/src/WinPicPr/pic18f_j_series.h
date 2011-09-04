#ifndef PIC18F_J_SERIES_H
#define PIC18F_J_SERIES_H
#include "WinPicPr/PIC_HW.h

class JS    //short for PIC18F J Series
public:
protected:
    static void SendCommand((uint8_t 4bitcommand);
    static void Write(uint16_t data);
    static uint8_t Read(void);
    static void Connect(void);
    static void Disconnect(void);
private:
};

#endif // PIC18F_J_SERIES_H
