#include <HT1632C.h>
#include <SureDisplay.h>

SureDisplay display(5,8,6,7);
int col = 0;
int x = 0;

void setup()
{
  display.begin();
  display.splash();
  delay(2000);
}

void marty_in_colours() 
{
  display.clear();
  display.draw_text("Marty is so cool", 0, 6, col);
  display.update();
  
  delay(500);
  if( col >= COLOUR_ORANGE ) {
    col = 0;
  } else {
    col++;
  }
}

void scrolling_heart()
{
  display.clear();
  display.heart(x,0);
  display.update();
  x++;
  if( x > DISPLAY_WIDTH ) {
    x = 0;
  }
  delay(500);
  
}

void loop()
{
  for(int i=0; i<4; i++) {
    marty_in_colours();
  }
  for(int i=0; i<10; i++) {
    scrolling_heart();
  }
}

