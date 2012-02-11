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

void SureDisplay::clear()
{
    driver.clear_display();
}

void SureDisplay::test__checkerboard(void)
{
    driver.select_all();
    driver.start_write(0x00);
    for(int i=0; i<32; i++)
        driver.writeDataRev(0xAA, 4);
    driver.deselect_all();
}

void SureDisplay::test__sweep(void)
{
    driver.deselect_all();
    for(int ichip=0; ichip<8; ichip++) {
        for(int i=0; i<8; i++ ) {
        
            // Green
            driver.select(ichip);
            driver.start_write(0x00+i);
            driver.writeDataRev(0xAA,4);
            delay(500);
            driver.select(ichip);
            driver.start_write(0x00+i);
            driver.writeDataRev(0x00, 4);
            
            // Red
            driver.select(ichip);
            driver.start_write(0x20+i);
            driver.writeDataRev(0x55,4);
            delay(500);
            driver.select(ichip);
            driver.start_write(0x20+i);
            driver.writeDataRev(0x00, 4);

        }
        driver.select(); // next chip
    }
}
