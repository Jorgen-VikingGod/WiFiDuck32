/*!
    \file profile.h
    \brief helper class to manage macro profiles (/profile1/config.json)
    \author Juergen Skrotzky
    \copyright MIT License
 */

#pragma once

#include <Arduino.h>  // String

namespace profile {

struct KeyColor {
  bool use;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  KeyColor(bool bUse = true, uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0) {
    use = bUse;
    r = red;
    g = green;
    b = blue;
  }
  KeyColor& operator=(const KeyColor& newVal) {
    use = newVal.use;
    r = newVal.r;
    g = newVal.g;
    b = newVal.b;
    return *this;
  }
};

struct KeyConfig {
  bool active;
  char name[8];
  KeyColor bg_color;
  KeyColor down_color;
  uint8_t up_speed;
  uint8_t down_speed;
  uint8_t size;
  KeyConfig(bool bActive = false, String sName = "", KeyColor colorBG = KeyColor(false),
            KeyColor colorDown = KeyColor(false), uint8_t iUpSpeed = 10, uint8_t iDownSpeed = 0, uint8_t iSize = 0) {
    active = bActive;
    strcpy(name, sName.c_str());
    bg_color = colorBG;
    down_color = colorDown;
    up_speed = iUpSpeed;
    down_speed = iDownSpeed;
    size = iSize;
  }
  KeyConfig& operator=(const KeyConfig& newVal) {
    active = newVal.active;
    strcpy(name, newVal.name);
    bg_color = newVal.bg_color;
    down_color = newVal.down_color;
    up_speed = newVal.up_speed;
    down_speed = newVal.down_speed;
    size = newVal.size;
    return *this;
  }
};

struct Config {
  char name[20];
  KeyColor bg_color;
  KeyColor down_color;
  uint8_t size;
  uint8_t up_speed;
  uint8_t down_speed;
  KeyConfig key1;
  KeyConfig key2;
  KeyConfig key3;
  KeyConfig key4;
  KeyConfig key5;
  KeyConfig key6;
  KeyConfig key7;
  KeyConfig key8;
  KeyConfig key9;
  KeyConfig key10;
  KeyConfig key11;
  KeyConfig enc;
  KeyConfig enc_up;
  KeyConfig enc_down;
  Config(String sName = "", KeyColor colorBG = KeyColor(), KeyColor colorDown = KeyColor(), uint8_t iSize = 2,
         uint8_t iUpSpeed = 10, uint8_t iDownSpeed = 0, KeyConfig configKey1 = KeyConfig(),
         KeyConfig configKey2 = KeyConfig(), KeyConfig configKey3 = KeyConfig(), KeyConfig configKey4 = KeyConfig(),
         KeyConfig configKey5 = KeyConfig(), KeyConfig configKey6 = KeyConfig(), KeyConfig configKey7 = KeyConfig(),
         KeyConfig configKey8 = KeyConfig(), KeyConfig configKey9 = KeyConfig(), KeyConfig configKey10 = KeyConfig(),
         KeyConfig configKey11 = KeyConfig(), KeyConfig configEnc = KeyConfig(), KeyConfig configEncUp = KeyConfig(),
         KeyConfig configEncDown = KeyConfig()) {
    strcpy(name, sName.c_str());
    bg_color = colorBG;
    down_color = colorDown;
    size = iSize;
    up_speed = iUpSpeed;
    down_speed = iDownSpeed;
    key1 = configKey1;
    key2 = configKey2;
    key3 = configKey3;
    key4 = configKey4;
    key5 = configKey5;
    key6 = configKey6;
    key7 = configKey7;
    key8 = configKey8;
    key9 = configKey9;
    key10 = configKey10;
    key11 = configKey11;
    enc = configEnc;
    enc_up = configEncUp;
    enc_down = configEncDown;
  }
  Config& operator=(const Config& newVal) {
    strcpy(name, newVal.name);
    bg_color = newVal.bg_color;
    down_color = newVal.down_color;
    size = newVal.size;
    up_speed = newVal.up_speed;
    down_speed = newVal.down_speed;
    key1 = newVal.key1;
    key2 = newVal.key2;
    key3 = newVal.key3;
    key4 = newVal.key4;
    key5 = newVal.key5;
    key6 = newVal.key6;
    key7 = newVal.key7;
    key8 = newVal.key8;
    key9 = newVal.key9;
    key10 = newVal.key10;
    key11 = newVal.key11;
    enc = newVal.enc;
    enc_up = newVal.enc_up;
    enc_down = newVal.enc_down;
    return *this;
  }
};

void begin();

void update();

void prev();
void next();

void setProfile(uint8_t p);
const String currentProfile();

void resetConfiguration();
void loadConfiguration();
void saveConfiguration();

void showMenu();

const Config getConfig();

const String fileNameByKey(uint8_t k);
const String fileNameByEncoder(uint8_t e);

}  // namespace profile