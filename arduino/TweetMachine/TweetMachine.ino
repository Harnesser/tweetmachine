/*
  Milklabs TweetMachine
 */

#include <HT1632C.h>
#include <SureDisplay.h>

SureDisplay display(5,8,6,7);

void setup()
{
  delay(500);
  display.begin();
  delay(500);
  display.test__checkerboard();
  delay(1000);
  display.clear();
  
}

void loop()
{
  display.test__text();
}

