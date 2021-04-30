/*!
    \file display.cpp
    \brief helper class to access tft display
    \author Juergen Skrotzky
    \copyright MIT License
 */

#include "display.h"

#include "config.h"
#include "debug.h"
#include <SPI.h>
#include <TFT_eSPI.h>

//#include "bmp.h"

namespace display {
// ===== PRIVATE ===== //
TFT_eSPI tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);  // Invoke custom library

void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, const String& text, uint32_t color, uint8_t textSize) {
  tft.setTextSize(textSize);

  int16_t textWidth = tft.textWidth(text);
  int32_t width = w - 2;
  int32_t height = h - 2;

  int32_t textX = x + (width - textWidth) / 2 + 2;
  int32_t textY = y + (height - tft.fontHeight()) / 2 + 2;

  tft.setTextColor(color);
  tft.drawRoundRect(x + 1, y + 1, width, height, 4, color);
  // tft.setCursor(textX, textY);
  tft.drawString(text, textX, textY);
}

void drawCircle(int32_t x, int32_t y, int32_t w, int32_t h, const String& text, uint32_t color, uint8_t textSize) {
  tft.setTextSize(textSize);

  int16_t textWidth = tft.textWidth(text);
  int32_t width = w - 2;
  int32_t height = h - 2;

  int32_t textX = x + (width - textWidth) / 2 + 2;
  int32_t textY = y + (height - tft.fontHeight()) / 2 + 2;

  int32_t r = height / 2;

  tft.setTextColor(color);
  tft.drawCircle(x + 1 + width / 2, y + 1 + r, r, color);
  // tft.setCursor(textX, textY);
  tft.drawString(text, textX, textY);
}

void drawTitle(int32_t x, int32_t y, const String& text, uint32_t color, uint8_t textSize) {
  tft.setTextSize(textSize);
  tft.setTextColor(color);
  tft.drawString(text, x + 1, y + 1);
}

void drawArrow(int32_t x, int32_t y, int32_t w, int32_t h, const String& text, uint32_t color, uint8_t textSize,
               bool reverse) {
  int32_t cX = reverse ? x + 10 : x + w - 10;
  int32_t cY = y + h / 4;
  int32_t lX = reverse ? x + w - 10 : x + 10;
  int32_t lY = y;
  int32_t rX = reverse ? x + w - 10 : x + 10;
  int32_t rY = y + h / 2;
  tft.fillTriangle(cX, cY, lX, lY, rX, rY, color);

  tft.setTextSize(textSize);
  int16_t textWidth = tft.textWidth(text);
  int32_t width = w - 2;
  int32_t height = h * 2 - 2;
  int32_t textX = x + (width - textWidth) / 2 + 2;
  int32_t textY = y + (height - tft.fontHeight()) / 2 + 2;

  tft.setTextColor(color);
  tft.drawString(text, textX, textY);
}

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms) {
  esp_sleep_enable_timer_wakeup(ms * 1000);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
  esp_light_sleep_start();
}

// ===== PUBLIC ====== //
void begin() {
  tft.init();
  tft.setRotation(3);
  /*
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(1);
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 240, 135, sonic);
  espDelay(2000);
  */
}
void update() {}

uint32_t color24to16(uint32_t c) { return tft.color24to16(c); }
uint16_t colorRgbTo16(uint8_t r, uint8_t g, uint8_t b) { return tft.color565(r, g, b); }
int16_t width() { return tft.width(); }
int16_t height() { return tft.height(); }

void fillScreen(uint32_t c) { tft.fillScreen(c); }

void showMenu1() {
  int rectWidth = tft.width() / 4;
  int rectHeight = tft.height() / 3;
  tft.fillScreen(TFT_BLACK);

  drawRect(0, 0, rectWidth, rectHeight, "F1", TFT_RED, 3);
  drawRect(rectWidth, 0, rectWidth, rectHeight, "F2", TFT_GREEN, 3);
  drawRect(rectWidth * 2, 0, rectWidth, rectHeight, "F3", TFT_GOLD, 3);
  drawRect(rectWidth * 3, 0, rectWidth, rectHeight, "F4", TFT_CYAN, 3);

  drawRect(0, rectHeight, rectWidth, rectHeight, "F5", TFT_MAGENTA, 2);
  drawRect(rectWidth, rectHeight, rectWidth, rectHeight, "F6", TFT_GREENYELLOW, 2);
  drawRect(rectWidth * 2, rectHeight, rectWidth, rectHeight, "F7", TFT_SILVER, 2);
  drawRect(rectWidth * 3, rectHeight, rectWidth, rectHeight, "F8", TFT_BLUE, 2);

  drawRect(0, rectHeight * 2, rectWidth, rectHeight, "F9", TFT_VIOLET, 1);
  drawRect(rectWidth, rectHeight * 2, rectWidth, rectHeight, "F10", TFT_OLIVE, 1);
  drawRect(rectWidth * 2, rectHeight * 2, rectWidth, rectHeight, "F11", TFT_MAROON, 1);
}

void showMenu2() {
  int rectWidth = tft.width() / 4;
  int rectHeight = tft.height() / 3;

  tft.fillScreen(TFT_BLACK);

  drawRect(0, 0, rectWidth, rectHeight, "ESC", TFT_BLUE);
  drawRect(rectWidth, 0, rectWidth, rectHeight, "W", tft.color565(200, 150, 100), 4);
  drawRect(rectWidth * 2, 0, rectWidth, rectHeight, "UNDO", TFT_SILVER);
  drawRect(rectWidth * 3, 0, rectWidth, rectHeight, "REDO", TFT_GREENYELLOW);

  drawRect(0, rectHeight, rectWidth, rectHeight, "A", TFT_MAGENTA, 4);
  drawRect(rectWidth, rectHeight, rectWidth, rectHeight, "S", TFT_VIOLET, 4);
  drawRect(rectWidth * 2, rectHeight, rectWidth, rectHeight, "D", TFT_OLIVE, 4);
  drawRect(rectWidth * 3, rectHeight, rectWidth, rectHeight, "SAVE", TFT_GREEN);

  drawRect(0, rectHeight * 2, rectWidth, rectHeight, "PREV", TFT_CYAN);
  drawRect(rectWidth, rectHeight * 2, rectWidth, rectHeight, "NEXT", TFT_GOLD);
  drawRect(rectWidth * 2, rectHeight * 2, rectWidth, rectHeight, "MODE", TFT_RED);
}
}  // namespace display