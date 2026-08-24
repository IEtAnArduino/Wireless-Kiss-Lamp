#include <Adafruit_NeoPixel.h>
#include <IRremote.h>
#include <ElegantOTA.h>
#include <WiFi.h>

#define NEO_PIN 16

#define NUMPIXELS 14 // Popular NeoPixel ring size
#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

int state = 0; // 0 = solid color, 1 = fade, 2 = cycle, 3 = rainbow

bool is_on = false;

int brightness = 255; // aka value for adjustValue function, 0-255

bool dimming = true; // for fade function

Adafruit_NeoPixel pixels(NUMPIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);

uint32_t red = pixels.gamma32(pixels.ColorHSV(0));              // 0xFB04EF00 red
uint32_t green = pixels.gamma32(pixels.ColorHSV(65536 / 3));    // 0xFA05EF00, green
uint32_t blue = pixels.gamma32(pixels.ColorHSV(2 * 65536 / 3)); // 0xF906EF00, blue
uint32_t white = pixels.gamma32(pixels.ColorHSV(0, 0, 255));   // 0xF807EF00, white
// 0xFB04EF00, off
// 0xF906EF00, on

uint32_t currentColor = red; // default color



// thank you Google AI overview for this helper function
uint32_t adjustValue(uint32_t color, uint8_t brightness) {
    // 1. Extract individual color channels using bitwise shifting
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8)  & 0xFF;
    uint8_t b =  color        & 0xFF;

    // 2. Scale each channel down by the brightness factor
    // Using 16-bit math to prevent variable overflow during multiplication
    r = (r * brightness) / 255;
    g = (g * brightness) / 255;
    b = (b * brightness) / 255;

    // 3. Pack the adjusted channels back into a single uint32_t
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}



void updateColors()
{
  if (is_on)
  {
    if (state == 0)
    {
      pixels.fill(currentColor, 0, NUMPIXELS);
      pixels.show();
    }
    else if (state == 1)
    {
      // fade
      pixels.fill(adjustValue(currentColor, brightness), 0, NUMPIXELS);
      pixels.show();
      if (dimming)
      {
        brightness -= 5;
      }
      else
      {
        brightness += 5;
      }
      if (brightness <= 0 || brightness >= 255)
      {
        dimming = !dimming;
      }
    }
  }
  else 
  {
    pixels.clear();
    pixels.show();
  }
}




void setup()
{
  Serial.begin(115200);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.show();  // Update strip to match

  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);
  digitalWrite(21, LOW);  // acting as ground for the KY-022 IR receiver module
  digitalWrite(22, HIGH); // acting as VCC for the KY-022 IR receiver module
  IrReceiver.begin(2);
}

void loop()
{

  static unsigned long lastUpdate = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate >= 20)
  {
    lastUpdate = currentMillis;
    updateColors();
  }



  if (IrReceiver.decode())
  {
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

    uint32_t received_signal = IrReceiver.decodedIRData.decodedRawData;
    if (is_on)
    {
      switch (received_signal)
      {

      case 0xFD02EF00: // off button
        is_on = false;
        break;

      case 0xF40BEF00: // solid mode button
        state = 0;
        break;

      case 0xF00FEF00: // fade mode button
        state = 1;
        break;

      case 0xFB04EF00:
        currentColor = red;
        break;

      case 0xFA05EF00:
        currentColor = green;
        break;

      case 0xF906EF00:
        currentColor = blue;
        break;

      case 0xF807EF00:
        currentColor = white;
        break;

      }
    }
    else
    {
      switch (received_signal)
      {
      case 0xFC03EF00: // on button
        is_on = true;
        Serial.println(currentColor);

        break;
      }
    }
    IrReceiver.resume();
  }
}

