#include "HT1632C.h"
#include "Arduino.h"


HT1632CClass::HT1632CClass()
{
};

/*
 * MID LEVEL FUNCTIONS
 * Functions that handle internal memory, initialize the hardware
 * and perform the rendering go here:
 */
void HT1632CClass::begin(int pinCS, int pinCSCLK, int pinWR, int pinDATA)
{
  _pinCS    = pinCS;
  _pinCSCLK = pinCSCLK;
  _pinWR    = pinWR;
  _pinDATA  = pinDATA;
  
  pinMode(_pinWR, OUTPUT);
  pinMode(_pinDATA, OUTPUT);
  pinMode(_pinCS, OUTPUT);
  pinMode(_pinCSCLK, OUTPUT);
  
  init_chips();
}

 
void HT1632CClass::init_chips(){
  // Send configuration to chip:
  // This configuration is from the HT1632 datasheet, with one modification:
  //   The RC_MASTER_MODE command is not sent to the master. Since acting as
  //   the RC Master is the default behaviour, this is not needed. Sending
  //   this command causes problems in HT1632C (note the C at the end) chips. 
  
    select_all();
 
    writeData(HT1632C_ID_CMD, HT1632C_ID_LEN);    // Command mode
    writeCommand(HT1632C_CMD_SYSDIS); // Disable system
    writeCommand(HT1632C_CMD_COMS00); // 16*32, PMOS drivers
      
//  writeCommand(HT1632C_CMD_MSTMD);  // MASTER MODE
    writeCommand(HT1632C_CMD_RCCLK);  // Internal clock
    
    writeCommand(HT1632C_CMD_SYSEN);  // System on
    writeCommand(HT1632C_CMD_LEDON);

    deselect_all();    
}


// Set the brightness to an integer level between 1 and 16 (inclusive).
// Uses the PWM feature to set the brightness.
void HT1632CClass::setBrightness(char brightness, char selectionmask) {
#if MARTY
  if(selectionmask == 0b00010000) {
    if(_tgtBuffer < _numActivePins)
      selectionmask = 0b0001 << _tgtBuffer;
    else
      return;
  }
#endif

  //select(selectionmask); 
  writeData(HT1632C_ID_CMD, HT1632C_ID_LEN);    // Command mode
  writeCommand(HT1632C_CMD_PWM(brightness));   // Set brightness
  deselect_all();
}


/*
 * LOWER LEVEL FUNCTIONS
 * Functions that directly talk to hardware go here:
 */
 
void HT1632CClass::writeCommand(char data) {
  writeData(data, HT1632C_CMD_LEN);
  writeSingleBit();
} 
// Integer write to display. Used to write commands/addresses.
// PRECONDITION: WR is LOW
void HT1632CClass::writeData(char data, char len) {
  for(int j=len-1, t = 1 << (len - 1); j>=0; --j, t >>= 1){
    // Set the DATA pin to the correct state
    digitalWrite(_pinDATA, ((data & t) == 0)?LOW:HIGH);
    NOP(); // Delay 
    // Raise the WR momentarily to allow the device to capture the data
    digitalWrite(_pinWR, HIGH);
    NOP(); // Delay
    // Lower it again, in preparation for the next cycle.
    digitalWrite(_pinWR, LOW);
  }
}
// REVERSED Integer write to display. Used to write cell values.
// PRECONDITION: WR is LOW
void HT1632CClass::writeDataRev(char data, char len) {
  for(int j=0; j<len; ++j){
    // Set the DATA pin to the correct state
    digitalWrite(_pinDATA, data & 1);
    NOP(); // Delay
    // Raise the WR momentarily to allow the device to capture the data
    digitalWrite(_pinWR, HIGH);
    NOP(); // Delay
    // Lower it again, in preparation for the next cycle.
    digitalWrite(_pinWR, LOW);
    data >>= 1;
  }
}
// Write single bit to display, used as padding between commands.
// PRECONDITION: WR is LOW
void HT1632CClass::writeSingleBit() {
  // Set the DATA pin to the correct state
  digitalWrite(_pinDATA, LOW);
  NOP(); // Delay
  // Raise the WR momentarily to allow the device to capture the data
  digitalWrite(_pinWR, HIGH);
  NOP(); // Delay
  // Lower it again, in preparation for the next cycle.
  digitalWrite(_pinWR, LOW);
}


// Prereq : CSCLK is low
void HT1632CClass::clock_cs(bool bit) {
    digitalWrite(_pinCS, bit);
    clock_cs();
}
    
void HT1632CClass::clock_cs(void) {
    digitalWrite(_pinCSCLK, LOW);
    digitalWrite(_pinCSCLK, HIGH);
    digitalWrite(_pinCSCLK, LOW);
}


// Choose a chip. 
// This function sets the correct CS line to LOW, and the rest to HIGH
// Called without an argument, it selects the next chip in the series
// by just clocking the CS line again.
void HT1632CClass::select(char chip_id) {

    // To be sure that we're chip-selecting the only chip we want,
    // we have to shift thru a deselect all();
    deselect_all();
    
    // Now clock in the select...
    clock_cs(LOW);
    for(int i=0; i<chip_id; i++)
        clock_cs(HIGH);
}

void HT1632CClass::select(void) {
    clock_cs(HIGH);
}

void HT1632CClass::select_all() {
    digitalWrite(_pinCS, LOW);
    for(int i=0; i<HT1632C_NUM_CHIPS; ++i)
        clock_cs();
    digitalWrite(_pinCS,HIGH);
}

void HT1632CClass::deselect_all() {
    digitalWrite(_pinCS, HIGH);
    for(int i=0; i<HT1632C_NUM_CHIPS; ++i)
        clock_cs();
}

//
//
//
void HT1632CClass::clear_display(void)
{
  select_all();
  writeData(HT1632C_ID_WR, HT1632C_ID_LEN);  // Write command
  writeData(0, HT1632C_ADDR_LEN);            // Start address
  for(int i=0; i<HT1632C_MEM_DEPTH; i++) {
    writeData(0, HT1632C_WORD_LEN);
  }
  writeData(0, HT1632C_WORD_LEN); // extra one just to make sure?
  deselect_all();  
}

 
HT1632CClass HT1632C;

