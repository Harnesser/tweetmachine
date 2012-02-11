#ifndef SURE_DISPLAY_H
#define SURE_DISPLAY_H

#define STRING_MAX 60

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 16

#define COLOUR_RED    0
#define COLOUR_GREEN  1
#define COLOUR_ORANGE 2

#include "HT1632C.h"

class SureDisplay
{
private:
    char _pin_CS;
    char _pin_CSCLK;
    char _pin_WR;
    char _pin_DATA;
    
protected:
    HT1632CClass driver;
    
public:
    SureDisplay(int pinCS, int pinCSCLK, int pinWR, int pinDATA);
    void begin();
};



#endif

