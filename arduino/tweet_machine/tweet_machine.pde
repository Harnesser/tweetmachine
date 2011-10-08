/*
 *  Example driving commands into the Sure Electronics
 * 3216 red/green boards. This should support writing to 
 * two boards.
 *
 *   Make sure the HT1632C library is in the libraries
 * directory so that the Arduino IDE can find it.
 */
 
#include <HT1632C.h>
#include "images.h"
#include "font_5x4.h"
#include "milklabs.h"

#define STRING_MAX 60

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 16
#define COLOUR_RED    0
#define COLOUR_GREEN  1
#define COLOUR_ORANGE 2

static const byte ht1632c_csclk = 4;  // Chip Select Shiftreg CLK
static const byte ht1632c_cs    = 5;  // Chip Select Shiftin
static const byte ht1632c_wrclk = 6;  // Write clock
static const byte ht1632c_data  = 7;  // Data pin

byte red_array [DISPLAY_HEIGHT][DISPLAY_WIDTH/8];
byte green_array [DISPLAY_HEIGHT][DISPLAY_WIDTH/8];
 
/* Cycle thru the PWM settings to produce a soothing pulsation */
void pulsate(int board)
{
  for(int i=16; i > 0; i-- ){
    HT1632C.select(board);
    HT1632C.setBrightness(i-1,0);
    delay(50);
  } 
  for(int i=0; i < 16; i++ ){
    HT1632C.select(board);
    HT1632C.setBrightness(i,0);
    delay(50);
  }
}


/* Initialise a write to the LED array memory */
void start_write(int address) 
{
  HT1632C.writeData(HT1632C_ID_WR, HT1632C_ID_LEN);
  HT1632C.writeData(address, HT1632C_ADDR_LEN);
}


/* Clear the display buffer memory */
void clear_array(byte array [][DISPLAY_WIDTH/8] )
{
   for(int i=0; i<16; i++ ) {
      for(int j=0; j<8; j++ ) {
         array[i][j] = 0x00;
      }
   } 
}


// Copy a small image to the full array
void copy_to_array(byte array [][DISPLAY_WIDTH/8], 
  int x_pos, int y_pos, 
  char image[], int image_width, int image_height )
{
  int x;
  char bit_pos;
  byte tmp;
  int array_x, array_y;
  
  for(int image_y=0; image_y < image_height; image_y++ ) {
    for(int image_x=0; image_x < image_width; image_x++ ) {
      x = ( x_pos + image_x );
      array_x =  x >> 3;   // Which byte do we write to?
      bit_pos = x & 0x07;    // Whit bit in that byte to set?
      
      // Check limits
      array_y = y_pos + image_y;
      if( ( array_y < DISPLAY_HEIGHT ) && ( array_x < (DISPLAY_WIDTH / 8) ) )
        if( image[image_y] & (0x80 >> image_x) )
          array[array_y][array_x] |= (0x80 >> bit_pos);
    }
  }  
};

int x_offset [] = { 0, 2, 0, 2, 4, 6, 4, 6 };
int y_offset [] = { 0, 0, 8, 8, 0, 0, 8, 8 };

// Draw shit on the board
void _render(byte array [][DISPLAY_WIDTH/8], int colour)
{
  int data;
  int x_off, y_off;
  char tmp;
  HT1632C.select(0);
  
  for(int chip=0; chip<8; chip++) {
    x_off = x_offset[chip];
    y_off = y_offset[chip];
    
    if( colour == 0 )
      start_write(0x00); // Green LED memory space
    else
      start_write(0x20); // Red LED memory space
      
    for(int col=0; col<16; col++){
      for(int half=0; half<2; half++) {
        data = 0;
        for(int dot=0; dot<4; dot++){
          tmp = array[y_off + (half*4) + (dot)][x_off + (col >= 8)];
          if(col < 8 ) {    
            if( tmp & ( 0x80 >> (col) ) )
              data |= (0x1 << dot);
          } else {
            if( tmp & ( 0x80 >> (col-8) ) )
              data |= (0x1 << dot);          
          }
        }
        HT1632C.writeDataRev(data, 4);
      }
    }  // end col
    
    HT1632C.select();
    
  } // end: chip
}     

void render(void)
{
  HT1632C.deselect_all();
  _render(green_array, 0);
  _render(red_array, 1);  
};

char *font_width = FONT_5X4_WIDTH;
int font_height = FONT_5X4_HEIGHT;
int gutter_space = 1;

void draw_text( char string[], char x_pos, char y_pos, char colour=COLOUR_RED)
{
  char currchar;
  char i = 0;
  char cur_x = x_pos;

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
    if( string[i] == '\0' )
      break;
      
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
    if(cur_x >= DISPLAY_WIDTH)
      break;
      
    // Draw character
    if(cur_x + font_width[currchar] + gutter_space >= 0){
      
      if(write_to_red_array)
        copy_to_array(red_array, (int)cur_x, (int)y_pos, 
          FONT_5X4[currchar], font_width[currchar], font_height);
      
      if(write_to_green_array)
        copy_to_array(green_array, (int)cur_x, (int)y_pos, 
          FONT_5X4[currchar], font_width[currchar], font_height);
            
      // Draw the gutter space
      //for(char j = 0; j < gutter_space; ++j)
      //  drawImage(font, 1, font_height, curr_x + font_width[currchar] + j, y, 0);
      
     cur_x += font_width[currchar] + gutter_space;
     ++i;
    }
    
  } // end: while
}

void draw_milklabs_logo(char x_pos, char y_pos)
{ 
 copy_to_array(green_array, x_pos, y_pos, IMG_MILKLABS_GREEN_1, 8, 16);
 copy_to_array(green_array, x_pos+8, y_pos, IMG_MILKLABS_GREEN_2, 8, 16);
}

void setup()
{
  Serial.begin(9600);
  HT1632C.begin( ht1632c_cs, ht1632c_csclk, ht1632c_wrclk, ht1632c_data );
  //HT1632C.clear_display();
}


char disp_str[STRING_MAX] ;

void loop()
{ 
  int ii;
  
  if( Serial.available() ) {
    delay(1000);
     
    // Copy serial chars into string buffer
    while( (Serial.available() > 0 ) && (ii <STRING_MAX ) ) {
      disp_str[ii++] = Serial.read();
    }
   
    // Blank the LED array
    clear_array(red_array);
    clear_array(green_array);
    HT1632C.clear_display();
    
    draw_text(disp_str, 0,0);
    render();
  }

  render();
}
 
