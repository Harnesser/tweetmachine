/*
  Milklabs TweetMachine
 */

#include <SPI.h>
#include <Ethernet.h>
#include <HT1632C.h>
#include <SureDisplay.h>

// Display vars
SureDisplay display(5,8,6,7);
int x_pos;

// Ethernet vars
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x00, 0x80, 0x11}; // marty's
//byte mac[] = { 
//  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };
IPAddress ip(192,168,1,100);

// initialize the library instance:
EthernetClient client;

const int num_tweets = 1;
const int requestInterval = 60000;  // delay between requests


char serverName[] = "api.twitter.com";  // twitter URL

boolean requested;                   // whether you've made a request since connecting
long lastAttemptTime = 0;            // last time you connected to the server, in milliseconds

String currentLine = "";            // string to hold the text from server
boolean readingTweet = false;       // if you're currently reading the tweet

char i_tweet; // tweet id
char i_char;  // current string char id
int  w_text;  // width of current string in pixels
char tweets[1][STRING_MAX] = { "..#1......" };
                               //".....#2..." };

void setup()
{
  // Display init
  display.begin();
  display.splash();
  x_pos = DISPLAY_WIDTH;
  
  // Ethernet init
  currentLine.reserve(256);
  if(!Ethernet.begin(mac)){
    Ethernet.begin(mac, ip); // if DHCP fails, start with a hard-coded address:
  }
  connectToServer(); // connect to Twitter:
  
  // Splash screen
  display.clear();
  display.draw_text("Reading...", 0, 6, COLOUR_ORANGE);
  display.update();
  
  //
  i_tweet = 0;
}

void loop()
{
  // -------------------------------------------
  if (client.connected()) {
    i_tweet = 0;
    if (client.available()) {
      // read incoming bytes:
      char inChar = client.read();

      // add incoming byte to end of line:
      currentLine += inChar; 

      // if you get a newline, clear the line:
      if (inChar == '\n') {
        currentLine = "";
      } 
      // if the current line ends with <text>, it will
      // be followed by the tweet:
      if ( currentLine.endsWith("<text>")) {
        // tweet is beginning. Clear the tweet string:
        readingTweet = true; 
        i_char = 0;
      }
      // if you're currently reading the bytes of a tweet,
      // add them to the tweet String:
      if (readingTweet) {
        if (inChar != '<' && (i_char<STRING_MAX)) {
          tweets[i_tweet][i_char++] = inChar;
        } 
        else {
          // if you got a "<" character,
          // you've reached the end of the tweet:
          readingTweet = false;
          if(i_char >= STRING_MAX) {
            i_char = STRING_MAX-1;  
          }
          tweets[i_tweet][i_char] = '\0';
          i_tweet++;
          if(i_tweet == num_tweets) {
            // close the connection to the server:
            client.stop();
            i_tweet = 0;
            w_text = display.get_string_width(tweets[i_tweet]);
          }
        }
      }
    }   
  }
  else if (millis() - lastAttemptTime > requestInterval) {
    // if you're not connected, and two minutes have passed since
    // your last connection, then attempt to connect again:
    connectToServer();
  } else {
      display.clear();
      //display.heart(0, 0);
      display.draw_text(tweets[i_tweet], x_pos--, 6, COLOUR_GREEN);
//    display.draw_text("milklabs", x_pos--, 6, COLOUR_GREEN);
      display.update();
      if(x_pos <= -w_text) { // w_text) {
        x_pos = DISPLAY_WIDTH;
        i_tweet++;
        if(i_tweet == num_tweets) {
          i_tweet = 0;
        }
        w_text = display.get_string_width(tweets[i_tweet]);
      }
      delay(20);
  }

}

void connectToServer() {
  // attempt to connect, and wait a millisecond:
  //Serial.println("connecting to server...");
  if (client.connect(serverName, 80)) {
    //Serial.println("making HTTP request...");
    // make HTTP GET request to twitter:
    display.draw_text("connecting...", 0,0, COLOUR_ORANGE);
    display.update();
    client.println("GET /1/statuses/user_timeline.xml?screen_name=milklabs&count=2 HTTP/1.1");
    client.println("HOST: api.twitter.com");
    client.println();
  } else {
      int i;
      char msg1[] = "oops";
      char msg2[] = "didn't connect";
      for(i=0; i<4; i++)
        tweets[0][i] = msg1[i];
      tweets[0][i] = '\0';
      for(i=0; i<15; i++)
        tweets[1][i] = msg2[i];
      tweets[1][i] = '\0';
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}
