/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */

#pragma once

#include <Arduino.h>  // String

namespace settings {
void begin();
void load();

void reset();
void save();

String toString();

const char* getSSID();
const char* getPassword();
const char* getChannel();
const char* getAutorun();
const char* getProfile();

uint8_t getChannelNum();
uint8_t getProfileNum();

void set(const char* name, const char* value);

void setSSID(const char* ssid);
void setPassword(const char* password);
void setChannel(const char* channel);
void setAutorun(const char* autorun);
void setProfile(const char* profile);
}  // namespace settings