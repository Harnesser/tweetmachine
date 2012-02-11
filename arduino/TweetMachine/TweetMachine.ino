/*
  Milklabs TweetMachine
 */

#include <HT1632C.h>
#include <SureDisplay.h>

SureDisplay display(1,2,3,4);

void setup()
{
  display.begin();
}

void loop()
{
  delay(1000);
}

