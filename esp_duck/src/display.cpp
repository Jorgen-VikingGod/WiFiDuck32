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

  tft.setTextColor(color);
  tft.drawRoundRect(x + 1, y + 1, width, height, 4, color);
  if (textWidth > 0) {
    int32_t textX = x + (width - textWidth) / 2 + 2;
    int32_t textY = y + (height - tft.fontHeight()) / 2 + 2;
    tft.drawString(text, textX, textY);
  }
}

void drawCenterText(int32_t x, int32_t y, int32_t w, int32_t h, const String& text, uint32_t color, uint8_t textSize) {
  tft.setTextSize(textSize);

  int16_t textWidth = tft.textWidth(text);
  int32_t width = w - 2;
  int32_t height = h - 2;

  tft.setTextColor(color);
  if (textWidth > 0) {
    int32_t textX = x + (width - textWidth) / 2 + 2;
    int32_t textY = y + (height - tft.fontHeight()) / 2 + 2;
    tft.drawString(text, textX, textY);
  }
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
}
void update() {}

uint32_t color24to16(uint32_t c) { return tft.color24to16(c); }
uint16_t colorRgbTo16(uint8_t r, uint8_t g, uint8_t b) { return tft.color565(r, g, b); }
int16_t width() { return tft.width(); }
int16_t height() { return tft.height(); }

void fillScreen(uint32_t c) { tft.fillScreen(c); }

}  // namespace display