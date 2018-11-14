# How to connect Adafruit with Arduino to receive data

> Arduino Ivy Calendar with Zapier, written by Agung Tarumampen ET1 2018

After you made a Zap that connects to your Adafruit feed with Zapier, you can start sending data to the feed and trigger specific functions.

In this example, we will light up a LED strip whenever an Agenda Event is created through Zapier.

Optional: The lighting(effect) can be changed to your liking.


---


## Table of Contents

- [Things you need](#thingsyouneed)
- [Installation](#installation)
- [FAQ](#faq)


---


## Things you need

- Zapier Account connected with your Google Calendar 
- Adafruit account
- ESP6288 Board (with an included micro-usb)
- LED Strip


---


## Installation

> These are the mandatory libraries to be installed

- Adafruit_NeoPixel
- Adafruit IO Arduino

### Setup

Setting up 

> Set up the network credentials

```shell
#include "config.h"
```


> With this code you are including the needed libraries and setting up the LED strip. In this example, I used pin D5 to connect the DIN part of my LED strip.

```shell
#include "Adafruit_NeoPixel.h"

#define PIXEL_PIN     D5
#define PIXEL_COUNT   24
#define PIXEL_TYPE    NEO_GRB + NEO_KHZ800

```


> Here we define "pixels" as the strip we are selecting. Fill in your Adafruit account name and AIO key, which you can get here:
![iokeyandusername](https://cdn-learn.adafruit.com/assets/assets/000/059/032/medium800/microcontrollers_aio-popup-window.png?1533923653)

```
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

#define FEED_OWNER "{yourusername}"

AdafruitIO_Feed *sharedFeed = io.feed("{yourkey}", FEED_OWNER);
```


### Make to connection to Adafruit and give status updates

> This is done within the setup();

```shell
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
```

### Step 1 - Initiate the sketch

> io.run(); is required for all sketches. it should always be present at the top of your loop function. it keeps the client connected to io.adafruit.com, and processes any incoming data.

```shell
void loop() {
  io.run();
}
```

### Step 2 - getting the data and what to with it

> This function, which is placed outside of the loop function,  is called whenever an 'sharedFeed' feed message is received from Adafruit IO. It is attached to the name of  your feed in the setup() function above.

```shell
void handleMessage(AdafruitIO_Data *data) {

  Serial.print("received <-  ");
  Serial.println(data->toString());
  Serial.println(data->toInt()); // Give the exact data adafruit is getting, convert it into a string and serial print it

   if(data->toInt()==1){
    Serial.print("Er is een event gemaakt!");
    colorWipe(pixels.Color(0, 255, 255), 50); // run a color effect to show that an agendaevent is created by Zapier
    delay(10000); // wait 10 seconds for it to
    for(int i=0; i<PIXEL_COUNT; ++i) { // clear all pixels
    pixels.clear();
  }
  Serial.println("LED UIT");
  } 
  else{ // when necessary add a random value to reset the strip
    Serial.print("Refresh");
    for(int i=0; i<PIXEL_COUNT; ++i) {
    pixels.clear();
  } 
  }

  pixels.show(); // turns the strip on
  
}
```


> The function Colorwipe() loops and turns on each individual pixels one by one fast.

```shell
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pixels.numPixels(); i++) { // loop through all of the pixels on the strip
    pixels.setPixelColor(i, c); // set the color of the strip to be whatever rgb color we want
    pixels.show(); // turns the strip on
    delay(wait); // wait until the end of the loop
  }  
}
```


---


## FAQ

- **LEDstrip brandt Rood**
    - Dat betekent dat het geen connectie kan maken. Check je netwerk connectie in de <configure.h> tab.

- **LEDstrip blijft op "het blijft regenen", maar is geen enkel druppel te zien**
    - Check of de Buienradar API link klopt met wat er is vermeld.


---

