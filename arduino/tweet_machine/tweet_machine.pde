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
// allow -ve positional values for scrolling (thanks @trevorpower, @cheeves)
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

int get_string_width( char string[] ){
  int str_size = 0;
  char currchar;
  
  for( int ii=0; ii<STRING_MAX; ii++ ){
   
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

// draw_text fills the appropriate array for some coloured text. It
// allows negative positions for the purposes of scrolling
void draw_text( char string[], int x_pos, int y_pos, char colour=COLOUR_RED)
{
  char currchar;
  int i = 0;
  int cur_x = x_pos;

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

void draw_milklabs_logo(char x_pos, char y_pos)
{ 
 copy_to_array(green_array, x_pos, y_pos, IMG_MILKLABS_GREEN_1, 8, 16);
 copy_to_array(green_array, x_pos+8, y_pos, IMG_MILKLABS_GREEN_2, 8, 16);
}

void setup()
{
  Serial.begin(9600);
  HT1632C.begin( ht1632c_cs, ht1632c_csclk, ht1632c_wrclk, ht1632c_data );
  HT1632C.clear_display();   
}


#define SERBUF_SIZE 140*5 // Roughly 5 tweets
char serial_buffer[SERBUF_SIZE]; 
char disp_str[STRING_MAX] ;
int x_pos=0;
int y_pos=0;
int string_width;

int i_serbuf;
char startstr[] = "MILKLABTWEET";

void loop()
{ 
  
  // Fill serial buffer with tweet goodnesss
  if( Serial.available() ) {
    delay(100); // This delay seems to be very important
    i_serbuf = 0;
    while( Serial.available() > 0 ) {  
       serial_buffer[i_serbuf++] = Serial.read();  
    }
    serial_buffer[i_serbuf] = '\0';
    
    Serial.println("Buffer--------");
    Serial.println(serial_buffer);
    Serial.println("--------------");
    
    // Copy tweet data into display buffer
    for(int ii=0; ii<STRING_MAX; ii++ ) {
      disp_str[ii] = serial_buffer[ii];
    }
    disp_str[STRING_MAX-1] = '\0';
    
    // Blank the LED array
    x_pos = DISPLAY_WIDTH;
    y_pos = 0;
    string_width = get_string_width(disp_str);
      
  } else {
    // Scroll existing text
    if( x_pos == -(string_width+1) ) {
      x_pos = DISPLAY_WIDTH;
      // Some Y messing
      y_pos++;
      if(y_pos>11)
        y_pos = 0;
    }
    else
      x_pos = x_pos - 1;
  }

  // Update the display
  clear_array(red_array);
  clear_array(green_array);
  draw_text(disp_str, x_pos, y_pos);
  render();
  
}
 
