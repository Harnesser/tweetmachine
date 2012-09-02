#ifndef SURE_DISPLAY_H
#define SURE_DISPLAY_H

#define STRING_MAX 140

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 16

#define COLOUR_RED    0
#define COLOUR_GREEN  1
#define COLOUR_ORANGE 2

#include "HT1632C.h"
#include <avr/pgmspace.h>

class SureDisplay
{
private:
    char _pin_CS;
    char _pin_CSCLK;
    char _pin_WR;
    char _pin_DATA;

    // LED matrix buffers
    char red_array [DISPLAY_HEIGHT][DISPLAY_WIDTH/8];
    char green_array [DISPLAY_HEIGHT][DISPLAY_WIDTH/8];

    // Display Buffer Operations
    void clear_array(char array [][DISPLAY_WIDTH/8] );
    void copy_to_array(char array [][DISPLAY_WIDTH/8], 
        int x_pos, int y_pos, 
        const char image[],
        int image_width, int image_height );
    void _update(char array [][DISPLAY_WIDTH/8], int colour);
    
protected:
    HT1632CClass driver;
    
public:
    SureDisplay(int pinCS, int pinCSCLK, int pinWR, int pinDATA);
    
    // Basic Ops
    void begin();
    void clear();
    void splash();
    void heart(int x, int y);
    
    // Update display (copies buffer to display)
    void update(void);
    
    // Writing Text
    void draw_text( char string[], int x_pos, int y_pos,
        char colour);
    int get_string_width( char string[] );
    
    // Testmode methods
    void test__checkerboard(void);
    void test__sweep(void);
    void test__text(void);
};



#endif

