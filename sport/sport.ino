// Adafruit IO Feed Reading
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-feeds/sharing-a-feed
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Brent Rubell for Adafruit Industries
// Copyright (c) 2018 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/************************ Example Starts Here *******************************/

#include "Adafruit_NeoPixel.h"

#define PIXEL_PIN     D5
#define PIXEL_COUNT   24
#define PIXEL_TYPE    NEO_GRB + NEO_KHZ800

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// the Adafruit IO username of whomever owns the feed
#define FEED_OWNER "babung"

// set up the `sharedFeed`
AdafruitIO_Feed *sharedFeed = io.feed("iot.ivy", FEED_OWNER);

void setup() {
  
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'sharedFeed' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  sharedFeed->onMessage(handleMessage);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  sharedFeed->get();

  pixels.begin();
  pixels.show();

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

}

// this function is called whenever an 'sharedFeed' feed message
// is received from Adafruit IO. it was attached to
// the 'digital' feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {

  Serial.print("received <-  ");
  Serial.println(data->toString());
  Serial.println(data->toInt());

   if(data->toInt()==1){
    Serial.print("Er is een event gemaakt!");
    colorWipe(pixels.Color(0, 255, 255), 50);
    delay(10000);
    for(int i=0; i<PIXEL_COUNT; ++i) {
    pixels.clear();
  }
  Serial.println("LED UIT");
  } 
  else{
    Serial.print("Refresh");
    for(int i=0; i<PIXEL_COUNT; ++i) {
    pixels.clear();
  } 
  }

  pixels.show();
  
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
  }  }
