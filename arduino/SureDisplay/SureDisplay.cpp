#include "SureDisplay.h"
#include <Arduino.h>
#include "../HT1632C/HT1632C.h"

// Constructor
SureDisplay::SureDisplay(int pinCS, int pinCSCLK, 
    int pinWR, int pinDATA)
{
    _pin_CS    = pinCS;
    _pin_CSCLK = pinCSCLK;
    _pin_WR    = pinWR;
    _pin_DATA  = pinDATA;    
}

// Initialise the display
void SureDisplay::begin()
{
    driver.begin(_pin_CS, _pin_CSCLK, _pin_WR, _pin_DATA);
    driver.clear_display();
}

