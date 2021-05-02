/*!
    \file leds.cpp
    \brief helper class to highlight RGB leds with Neopixel library
    \author Juergen Skrotzky
    \copyright MIT License
 */

#include "led.h"

#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "debug.h"

namespace led {
// ===== PRIVATE ===== //

uint8_t brightness = 255;
bool updateNeeded = true;

struct LEDAnimation {
  bool active;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t step;
  uint8_t lastStep;
  uint8_t speed;
  unsigned long lastTime;
  LEDAnimation(bool bActive = false, uint8_t iR = 0, uint8_t iG = 0, uint8_t iB = 0, uint8_t iStep = 10,
               uint8_t iLastStep = 0, uint8_t iSpeed = 0, unsigned long lLastTime = 0) {
    active = bActive;
    r = iR;
    g = iG;
    b = iB;
    step = iStep;
    lastStep = iLastStep;
    speed = iSpeed;
    lastTime = lLastTime;
  }
};

LEDAnimation animationFrame[NEOPIXEL_NUM + 1] = {LEDAnimation(), LEDAnimation(), LEDAnimation(), LEDAnimation(),
                                                 LEDAnimation(), LEDAnimation(), LEDAnimation(), LEDAnimation(),
                                                 LEDAnimation(), LEDAnimation(), LEDAnimation(), LEDAnimation()};

bool animationNeeded = false;

Adafruit_NeoPixel leds = Adafruit_NeoPixel(NEOPIXEL_NUM + 1, LED_PIN, NEO_GRB + NEO_KHZ800);

// ===== PUBLIC ====== //
void begin() {
  leds.begin();
  leds.setBrightness(brightness);
  led::show();
#if 0
  for (uint8_t c = 0; c < 11; c++) {
    uint32_t col = Wheel(c * 23);
    uint8_t r = col >> 16;
    uint8_t g = col >> 8;
    uint8_t b = col;
    debug("\"");
    debug(r);
    debug(",");
    debug(g);
    debug(",");
    debug(b);
    debugln("\"");
  }
#endif
}

void update() {
  if (updateNeeded) {
    led::show();
  }
  if (animationNeeded) {
    for (uint8_t k = 0; k < NEOPIXEL_NUM + 1; k++) {
      if (animationFrame[k].active) {
        if ((millis() - animationFrame[k].lastTime) > animationFrame[k].speed) {
          uint32_t startColor = leds.getPixelColor(k);
          uint8_t startRed = (startColor >> 16) & 0xff;
          uint8_t startGreen = (startColor >> 8) & 0xff;
          uint8_t startBlue = startColor & 0xff;
          uint8_t red = map(animationFrame[k].lastStep, 0, 250, startRed, animationFrame[k].r);
          uint8_t green = map(animationFrame[k].lastStep, 0, 250, startGreen, animationFrame[k].g);
          uint8_t blue = map(animationFrame[k].lastStep, 0, 250, startBlue, animationFrame[k].b);
          if (red == startRed && green == startGreen && blue == startBlue && animationFrame[k].lastStep > 10) {
            animationFrame[k].active = false;
            leds.setPixelColor(k, animationFrame[k].r, animationFrame[k].g, animationFrame[k].b);
          } else {
            leds.setPixelColor(k, red, green, blue);
            animationFrame[k].lastStep += animationFrame[k].step;
            animationFrame[k].lastTime = millis();
          }
          leds.show();
        }
      }
    }
    animationNeeded = animationFrame[0].active || animationFrame[1].active || animationFrame[2].active ||
                      animationFrame[3].active || animationFrame[4].active || animationFrame[5].active ||
                      animationFrame[6].active || animationFrame[7].active || animationFrame[8].active ||
                      animationFrame[9].active || animationFrame[10].active || animationFrame[11].active;
  }
}

void setBrightness(uint8_t val) {
  leds.setBrightness(val);
  led::show();
}

void setColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  leds.setPixelColor(n, r, g, b);
  updateNeeded = true;
}

void setColors(uint8_t r, uint8_t g, uint8_t b) {
  for (uint16_t n = 0; n < leds.numPixels(); n++) {
    leds.setPixelColor(n, r, g, b);
  }
  led::show();
}

void setPixelColor(uint16_t n, uint32_t c) {
  leds.setPixelColor(n, c);
  updateNeeded = true;
}

uint32_t getPixelColor(uint16_t n) { return leds.getPixelColor(n); }

void show() {
  leds.show();
  updateNeeded = false;
}

void crossFade(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t speed, uint8_t step) {
  animationFrame[n].active = true;
  animationFrame[n].r = r;
  animationFrame[n].g = g;
  animationFrame[n].b = b;
  animationFrame[n].speed = speed;
  animationFrame[n].lastTime = millis();
  animationFrame[n].lastStep = 0;
  animationFrame[n].step = step;
  animationNeeded = true;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return leds.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return leds.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return leds.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

}  // namespace led