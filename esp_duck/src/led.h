/*!
    \file leds.h
    \brief helper class to highlight RGB leds with Neopixel library
    \author Juergen Skrotzky
    \copyright MIT License
 */

#pragma once

#include <Arduino.h>  // String

namespace led {

void begin();
void update();

void setBrightness(uint8_t val);
void setColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void setColors(uint8_t r, uint8_t g, uint8_t b);
void setPixelColor(uint16_t n, uint32_t c);
uint32_t getPixelColor(uint16_t n);
void crossFade(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t speed, uint8_t step = 10);
void show();

uint32_t Wheel(byte WheelPos);

}  // namespace led