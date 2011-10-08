/*
  HT1632C.h - Library for communicating with the popular HT1632/HT1632C
  LED controllers. This library provides higher-level access (including
  text drawing) for these chips. Currently, the library supports writing 
  to a single chip at a time, and has been tested with a single
  Sure Electronics 3208 5mm red board.
  
  Created by Gaurav Manek, April 8, 2011.
  Released into the public domain.
*/
#ifndef HT1632C_h
#define HT1632C_h

/*
 * USER OPTIONS
 * Change these options
 */

// Size of COM and OUT in bits:
#define COM_SIZE 8
#define OUT_SIZE 32
// COM_SIZE MUST be either 8 or 16.

// Use N-MOS (if 1) or P-MOS (if 0):
#define USE_NMOS 1
// There are known issues with this. If the default doesn't work,
// try changing the value.

#define HT1632C_NUM_RED_GREEN_BOARDS 2
#define HT1632C_NUM_CHIPS 8 * HT1632C_NUM_RED_GREEN_BOARDS

/*
 * END USER OPTIONS
 * Don't edit anything below unless you know what you are doing!
 */

// NO-OP Definition
#define NOP(); __asm__("nop\n\t"); 
// The HT1632 requires at least 50 ns between the change in data and the rising
// edge of the WR signal. On a 16MHz processor, this provides 62.5ns per NOP. 

// Standard command list.
// This list is modified from original code by Bill Westfield

#define HT1632C_ID_CMD 0b100	/* ID = 100 - Commands */
#define HT1632C_ID_RD  0b110	/* ID = 110 - Read RAM */
#define HT1632C_ID_WR  0b101	/* ID = 101 - Write RAM */
#define HT1632C_ID_LEN 3         /* IDs are 3 bits */

// Do note that SYSON has been changed to SYSEN
#define HT1632C_CMD_SYSDIS 0x00	/* CMD= 0000-0000-x Turn off oscil */
#define HT1632C_CMD_SYSEN  0x01	/* CMD= 0000-0001-x Enable system oscil */
#define HT1632C_CMD_LEDOFF 0x02	/* CMD= 0000-0010-x LED duty cycle gen off */
#define HT1632C_CMD_LEDON  0x03	/* CMD= 0000-0011-x LEDs ON */
#define HT1632C_CMD_BLOFF  0x08	/* CMD= 0000-1000-x Blink ON */
#define HT1632C_CMD_BLON   0x09	/* CMD= 0000-1001-x Blink Off */
#define HT1632C_CMD_SLVMD  0x10	/* CMD= 0001-00xx-x Slave Mode */
#define HT1632C_CMD_MSTMD  0x14	/* CMD= 0001-01xx-x Master Mode */
#define HT1632C_CMD_RCCLK  0x18	/* CMD= 0001-10xx-x Use on-chip clock */
#define HT1632C_CMD_EXTCLK 0x1C	/* CMD= 0001-11xx-x Use external clock */
#define HT1632C_CMD_COMS00 0x20	/* CMD= 0010-ABxx-x commons options */
#define HT1632C_CMD_COMS01 0x24	/* CMD= 0010-ABxx-x commons options */
#define HT1632C_CMD_COMS10 0x28	/* CMD= 0010-ABxx-x commons options */
#define HT1632C_CMD_COMS11 0x2C	/* CMD= 0010-ABxx-x commons options */
#define HT1632C_CMD_PWM_T  0xA0	/* CMD= 101x-PPPP-x PWM duty cycle - template*/
#define HT1632C_CMD_PWM(lvl) (HT1632C_CMD_PWM_T | (lvl-1))
  /* Produces the correct command from the given value of lvl. lvl = [0..15] */
#define HT1632C_CMD_LEN    8	/* Commands are 8 bits long, excluding the trailing bit */
#define HT1632C_ADDR_LEN   7	/* Addresses are 7 bits long */
#define HT1632C_WORD_LEN   4     /* Words are 4 bits long */
#define HT1632C_MEM_DEPTH  0x3F

class HT1632CClass
{
  private:  
    char _pinCS;
    char _pinCSCLK;
    char _pinWR;
    char _pinDATA;
    
  public:
    void writeCommand(char);
    void writeData(char, char);
    void writeDataRev(char, char);
    void writeSingleBit();
    void init_chips();
    void clock_cs(bool bit);
    void clock_cs(void);
    void deselect_all();
    void select_all();
    void select(char mask);
    void select(); // selects next chip
    void begin(int pinCS, int pinCSCLK, int pinWR, int pinDATA);
    void setBrightness(char brightness, char selectionmask = 0b00010000);
    void clear_display(void);
};

extern HT1632CClass HT1632C;

#else
//#error "HT1632C.h" already defined!
#endif
