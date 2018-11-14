# How to connect Adafruit with Arduino to receive data

> Arduino Ivy Calendar with Zapier, written by Agung Tarumampen ET1 2018

Met deze opdracht gaan we door middel van Zapier data versturen naar Adafruit om vervolgens met Arduino data op te halen van de Buienradar API. Als output krijgen we kleureneffect te zien via een LEDstrip gebaseerd op of het lekker droog blijft, het gaat binnen een half uur gaat regenen of het continu zal blijven regenen.
Dit krijgen we te zien elk uur. 

Optioneel: De trigger is naar eigen preference te veranderen naar bijvoorbeeld: twitter, instagram en allerlei andere apps.


---


## Inhoudsopgave

- [Benodigdheden](#benodigdheden)
- [Zapier en Adafruit](#zapierenadafruitsetup)
- [Installatie](#installatie)
- [FAQ](#faq)


---


## Benodigdheden

- Arduino
- ESP6288 Board
- Buienradar API
- Zapier Account
- Optioneel: twitter acccount


---


## Zapier en Adafruit setup

### Stap 1 - Zapier setup

- Maak een Zapier account aan en maak een Zap aan! https://zapier.com/learn/getting-started-guide/build-zap-workflow

### Stap 2 - Adafruit setup

- In Adafruit maak je een account aan
- Daarin maak je een feed en onthoudt je de feed key
- In Adafruit heb je je AIO Key, die je nodig hebt om de koppeling te maken tussen Adafruit en Arduino

### Stap 3 - Zapier en Adafruit link

- In Zapier link je Adafruit aan arduino d.m.v. https://docs.google.com/document/d/1KvKUJE_4mgKk9AfHR9K_8wLWQh2QyTWSqcO_cVrfCGk/edit?usp=drive_web&ouid=108835577367122712922


---


## Installatie

> Libaries om te installeren via de library manager

- Adafruit_NeoPixel
- Adafruit IO Arduino

### Setup

De libraries die included moeten worden

> Including

```shell
#include "config.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

```

> Neopixel library, Adafruit connectie met feed en benodigde variabelen aanmaken

```shell
#include "Adafruit_NeoPixel.h"

#define PIXEL_PIN     D5
#define PIXEL_COUNT   24
#define PIXEL_TYPE    NEO_GRB + NEO_KHZ800

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

#define FEED_OWNER "{yourusername}"

AdafruitIO_Feed *sharedFeed = io.feed("{yourkey}", FEED_OWNER);
```

> Setup van de Buienradar API

```shell
// set up the `sharedFeed`
const char* server = "http://gpsgadget.buienradar.nl/data/raintext/";
const char* lat = "?lat=52.36";
const char* lon = "&lon=4.91";

int rainData[] = {
  100, 200, 000, 000, 000, 000, 000, 000
};
```

### Buienradar

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
