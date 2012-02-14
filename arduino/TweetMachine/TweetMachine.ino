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
IPAddress ip(192,168,1,101);

// initialize the library instance:
EthernetClient client;

const int num_tweets = 1;
const int requestInterval = 10000; //60000;  // delay between requests


char serverName[] = "api.twitter.com";  // twitter URL

boolean requested;                   // whether you've made a request since connecting
long lastAttemptTime = 0;            // last time you connected to the server, in milliseconds

String currentLine = "";            // string to hold the text from server
boolean readingTweet = false;       // if you're currently reading the tweet

char i_tweet;
char i_char;
char tweets[2][140] = { "..#1......",
                        ".....#2..." };

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
}

void loop()
{
  int w_text;

  // -------------------------------------------
  if (client.connected()) {
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
        i_tweet = 0;
        i_char = 0;
      }
      // if you're currently reading the bytes of a tweet,
      // add them to the tweet String:
      if (readingTweet) {
        if (inChar != '<') {
          tweets[i_tweet][i_char++] = inChar;
        } 
        else {
          // if you got a "<" character,
          // you've reached the end of the tweet:
          readingTweet = false;
          tweets[i_tweet++][i_char] = '\0';
          
          if(i_tweet == num_tweets) {
            // close the connection to the server:
            client.stop();
            w_text = display.get_string_width(tweets[i_tweet-1]); 
            i_tweet = 0;
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
    client.println("GET /1/statuses/user_timeline.xml?screen_name=milklabs&count=2 HTTP/1.1");
    client.println("HOST: api.twitter.com");
    client.println();
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}
