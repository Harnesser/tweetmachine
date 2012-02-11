#include "SureDisplay.h"
#include <Arduino.h>
#include "../HT1632C/HT1632C.h"
#include "font_5x4.h"
#include "milklabs.h"

int x_offset [] = { 
  0, 2, 0, 2, 4, 6, 4, 6 };
int y_offset [] = { 
  0, 0, 8, 8, 0, 0, 8, 8 };

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
    clear_array(red_array);
    clear_array(green_array);
}

void SureDisplay::splash()
{ 
  clear();
  draw_text("milklabs", 7, 6, COLOUR_GREEN);
  copy_to_array(green_array, 48, 0, IMG_MILKLABS_GREEN_1, 8, 16);
  copy_to_array(green_array, 56, 0, IMG_MILKLABS_GREEN_2, 8, 16);
  update();
}


/*
 * Display Buffer Operations
 */

/* Clear the display buffer memory */
void SureDisplay::clear_array(char array [][DISPLAY_WIDTH/8] )
{
  for(int i=0; i<16; i++ ) {
    for(int j=0; j<8; j++ ) {
      array[i][j] = 0x00;
    }
  } 
}

/* Copy a small image to the full array
 allow -ve positional values for scrolling (thanks @trevorpower, @cheeves)
 */
void SureDisplay::copy_to_array(char array [][DISPLAY_WIDTH/8], 
    int x_pos, int y_pos, 
    char image[],
    int image_width, int image_height )
{
  int x;
  char bit_pos;
  char tmp;
  int array_x, array_y;

  for(int image_y=0; image_y < image_height; image_y++ ) {
    for(int image_x=0; image_x < image_width; image_x++ ) {
      x = ( x_pos + image_x );
      if( x >= 0 ) {  // ignore stuff in negative space 
        array_x =  x >> 3;   // Which byte do we write to?
        bit_pos = x & 0x07;    // Whit bit in that byte to set?

        // Check limits
        array_y = y_pos + image_y;
        if( ( array_y < DISPLAY_HEIGHT ) && ( array_x < (DISPLAY_WIDTH / 8) ) )
          if( image[image_y] & (0x80 >> image_x) )
            array[array_y][array_x] |= (0x80 >> bit_pos);
      }
    }
  }  
};

/*
 Draw stuff on the board by sending one of the display buffers to 
 the Sure Electronics boards.
 */
void SureDisplay::_update(char array [][DISPLAY_WIDTH/8], int colour)
{
  int data;
  int x_off, y_off;
  char tmp;
  driver.select(0);

  for(int chip=0; chip<8; chip++) {
    x_off = x_offset[chip];
    y_off = y_offset[chip];

    if( colour == 0 )
      driver.start_write(0x00); // Green LED memory space
    else
      driver.start_write(0x20); // Red LED memory space

    for(int col=0; col<16; col++){
      for(int half=0; half<2; half++) {
        data = 0;
        for(int dot=0; dot<4; dot++){
          tmp = array[y_off + (half*4) + (dot)][x_off + (col >= 8)];
          if(col < 8 ) {    
            if( tmp & ( 0x80 >> (col) ) )
              data |= (0x1 << dot);
          } 
          else {
            if( tmp & ( 0x80 >> (col-8) ) )
              data |= (0x1 << dot);          
          }
        }
        driver.writeDataRev(data, 4);
      }
    }  // end col

    driver.select();

  } // end: chip
}     


/* Write all image buffers to the boards */
void SureDisplay::update(void)
{
  driver.deselect_all();
  _update(green_array, 0);
  _update(red_array, 1);  
};


/*
 * Text Writing
 */
 
/* draw_text fills the appropriate array for some coloured text. It
 allows negative positions for the purposes of scrolling
 */
void SureDisplay::draw_text( char string[], int x_pos, int y_pos, char colour=COLOUR_RED)
{
  char currchar;
  int i = 0;
  int cur_x = x_pos;
  
  int gutter_space = 1;
  char *font_width = FONT_5X4_WIDTH;
  int font_height = FONT_5X4_HEIGHT;
  
  bool write_to_red_array   = false;
  bool write_to_green_array = false;

  switch(colour){
  case COLOUR_RED:
    write_to_red_array   = true;
    write_to_green_array = false;
    break;
  case COLOUR_GREEN:
    write_to_red_array   = false;
    write_to_green_array = true;
    break;
  case COLOUR_ORANGE:
    write_to_red_array   = true;
    write_to_green_array = true;
    break;      
  }

  // Write each char of the string to the display buffers.
  while(true) {
    if( string[i] == '\0' ) {
      break;
    }

    currchar = string[i] - 32;

    // lowercase
    if(currchar >= 65 && currchar <= 90) 
      currchar -= 32; // Make this character uppercase.

    // unsupported characters
    if(currchar < 0 || currchar >= 64) {
      ++i;
      continue; // Skip this character.
    }  

    // off-screen
    if(cur_x >= DISPLAY_WIDTH) {
      break;
    }

    // Draw character if some of it ends up at x >= 0
    if(cur_x + font_width[currchar] + gutter_space >= 0){
      if(write_to_red_array)
        copy_to_array(red_array, cur_x, y_pos, 
        FONT_5X4[currchar], font_width[currchar], font_height);

      if(write_to_green_array)
        copy_to_array(green_array, cur_x, y_pos, 
        FONT_5X4[currchar], font_width[currchar], font_height);
    }  

    // Update position and advance to next char
    cur_x += font_width[currchar] + gutter_space;
    i++;

  } // end: while
}


/* Calculate the width of the string in LEDs including spaces */
int SureDisplay::get_string_width( char string[] )
{
  int str_size = 0;
  char currchar;
  int gutter_space = 1;
  char *font_width = FONT_5X4_WIDTH;
  
  for( int ii=0; ii<STRING_MAX; ii++ ){
    if( currchar == '\0' ) {
        return str_size;
    }
    
    // Find ID of char in out arrayss 
    currchar = string[ii] - 32;
    if(currchar >= 65 && currchar <= 90) 
      currchar -= 32; // Make this character uppercase.

    // skip unsupported characters
    if(currchar < 0 || currchar >= 64)
      continue;

    str_size += font_width[currchar] + gutter_space;
  }

  return str_size;
}


/*
 * Testmodes
 */
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

void SureDisplay::test__text(void)
{
  draw_text("milklabs", 3, 4, COLOUR_RED);
  update();
  delay(1000);
  clear();
  draw_text("milklabs", 7, 11, COLOUR_GREEN);
  update();
  delay(1000);
}
