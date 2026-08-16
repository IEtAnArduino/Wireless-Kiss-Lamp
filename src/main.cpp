#include <Adafruit_NeoPixel.h>
#include <IRremote.h>
#include <ElegantOTA.h>
#include <WiFi.h>

#define PIN 0

#define NUMPIXELS 14 // Popular NeoPixel ring size

uint32_t last_input = 0;
bool is_on = false;
// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels


  uint32_t red = pixels.Color(255,0,0); // E619FF00 red
  uint32_t green = pixels.Color(0,255,0); // E41BFF00, green
  uint32_t blue = pixels.Color(0,0,255); // EE11FF00, blue
  // E01FFF00, off
  // F20DFF00, on

void setup() {
  Serial.begin(115200);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  IrReceiver.begin(2);
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

    uint32_t received_signal = IrReceiver.decodedIRData.decodedRawData;
    if (is_on) {
      switch(received_signal) {

        case 0xE619FF00:
          last_input = red;
          pixels.fill(red, 0, NUMPIXELS);
          pixels.show();
          break;

        case 0xE41BFF00:
          last_input = green;
          pixels.fill(green, 0, NUMPIXELS);
          pixels.show();
          break;

        case 0xEE11FF00:
          last_input = blue;
          pixels.fill(blue, 0, NUMPIXELS);
          pixels.show();
          break;

        case 0xE01FFF00: // off button
            is_on = false;
            pixels.clear();
            pixels.show();
            break;
      }
    } else {
        switch(received_signal) {
          case 0xF20DFF00: // on button
            is_on = true;
            Serial.println(last_input);
            pixels.fill(last_input, 0, NUMPIXELS);
            pixels.show();
            break;
        }
    }
    IrReceiver.resume();
    }







  // pixels.clear(); // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.

  // for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

  //   // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
  //   // Here we're using a moderately bright green color:
  //   pixels.setPixelColor(i, pixels.Color(0, 150, 0));

  //   pixels.show();   // Send the updated pixel colors to the hardware.

  //   delay(DELAYVAL); // Pause before next pass through loop
  // }
}
