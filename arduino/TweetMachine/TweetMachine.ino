/*
  Milklabs TweetMachine
 */

#include <HT1632C.h>
#include <SureDisplay.h>

SureDisplay display(5,8,6,7);
int x_pos;

void setup()
{
  delay(500);
  display.begin();
  delay(500);
  display.test__checkerboard();
  delay(1000);
  display.clear();
  
  x_pos = DISPLAY_WIDTH;
}

void loop()
{
  int w_text;
  
  w_text = display.get_string_width("Milklabs");
  display.draw_text("Milklabs", x_pos--, 6, COLOUR_GREEN);
  display.update();
  delay(100);
  
  if(x_pos == -(w_text+1)) {
    x_pos = DISPLAY_WIDTH;
  }
  display.clear();
}

