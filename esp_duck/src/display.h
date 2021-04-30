/*!
    \file display.h
    \brief helper class to access tft display
    \author Juergen Skrotzky
    \copyright MIT License
 */

#pragma once

#include <Arduino.h>  // String

namespace display {
void begin();

void update();

uint32_t color24to16(uint32_t c);
uint16_t colorRgbTo16(uint8_t r, uint8_t g, uint8_t b);
int16_t width();
int16_t height();
void fillScreen(uint32_t c);

void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, const String& text, uint32_t color, uint8_t textSize = 2);
void drawCircle(int32_t x, int32_t y, int32_t w, int32_t h, const String& text, uint32_t color, uint8_t textSize = 2);
void drawTitle(int32_t x, int32_t y, const String& text, uint32_t color, uint8_t textSize = 2);
void drawArrow(int32_t x, int32_t y, int32_t w, int32_t h, const String& text, uint32_t color, uint8_t textSize = 1,
               bool reverse = false);
void showMenu1();
void showMenu2();

}  // namespace display