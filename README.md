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

> Libaries om te installeren via de library manager

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
![microcontrollers](https://cdn-learn.adafruit.com/assets/assets/000/059/031/medium800/microcontrollers_view-aio-key.png)
![iokey](https://cdn-learn.adafruit.com/assets/assets/000/026/949/medium800/adafruit_io_iokey.png)

```
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

#define FEED_OWNER "{yourusername}"

AdafruitIO_Feed *sharedFeed = io.feed("{yourkey}", FEED_OWNER);
```


### Setup

> de void setup(); om de connectie weer te geven in je serial monitor

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

### Stap 1 - void loop()

> In de void loop(); draait alleen 1 functie: de functie om de adafruit connectie te maken

```shell
void loop() {
  io.run();
}
```

### Stap 2 - void handleMessage(), void getRainData(), void extractDataFromAPI(), void evaluateRain() 

> handleMessage(), deze functie wordt aangeroepen wanneer 'sharedFeed' feed message is binnengekomen van Adafruit IO.

```shell
void handleMessage(AdafruitIO_Data *data){
  if(data->toInt()==1){
    Serial.print("Waarde 1 binnengekregen");
    getRainData(server, lat, lon);    // binnenhalen van de regendata van de website van buienradar
    evaluateRain(); // kijk in die data of het gaat regenen
    delay(20000);
    for(int i=0; i<PIXEL_COUNT; ++i) {
    pixels.clear();
  }
  } 
  else{
    Serial.print("Clear");
    for(int i=0; i<PIXEL_COUNT; ++i) {
    pixels.clear();
  }
  }
  pixels.show();
}
```

> getRainData(), functie voor het binnenhalen van de inhoud van de pagina van buienradar

```shell
void getRainData(String server, String lat, String lon) {
  String url = server + lat + lon;
  Serial.println("Loading: " + url);

  HTTPClient http;
  http.begin(url); //HTTP
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("HTTP OK");
      String payload = http.getString();
      extractDataFromAPI(payload);

      colorWipe(pixels.Color(255, 255, 0), 50); //blue-white flash (update succes)
      delay(1000);
      return;
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    http.end();
    colorWipe(pixels.Color(255, 0, 0), 50); 
    delay(1000);
    return;
  }

}
```

> extractDataFromAPI(), deze functie zet de html pagina (string) om naar bruikbare data

```shell
void extractDataFromAPI(String data) {
  int iterarions = 8;
  for (int i = 0; i < iterarions; i++ ) {
    int subStringStart = i * 11;
    int subStringEnd = i * 11 + 3;
    rainData[i] = data.substring(subStringStart, subStringEnd).toInt();
    Serial.println(rainData[i]);
  }
}
```

> evaluateRain(), deze functie kijkt naar de binnengehaalde data en stel de kleur in aan de hand van deze data

```shell
void evaluateRain() {
  if (rainData[0] + rainData[1] + rainData[2] <= 15) {
    //regent niet
    colorWipe(pixels.Color(0, 255, 0), 50); 
    Serial.println("Lekker Droog");
  }  else if (rainData[0] + rainData[1] + rainData[2] < 150) {
    //regent komende half uur
     colorWipe(pixels.Color(0, 255, 255), 50); 
    Serial.println("Er is regen");
  } else {
    //regent continue
     colorWipe(pixels.Color(0, 0, 255), 50);
    Serial.println("Het blijft regenen!");
  }
}
```

> colorWipe(), kleureneffect voor data

```shell
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
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

