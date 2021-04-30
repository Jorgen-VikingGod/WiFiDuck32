/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */

#pragma once

#include <Arduino.h>  // String
#include <FS.h>

namespace spiffs {
void begin();
void format();

size_t size();
size_t usedBytes();
size_t freeBytes();

size_t size(String fileName);
bool exists(String fileName);

File open(String fileName, const char* mode = "r");
void create(String fileName);

void remove(String fileName);
void rename(String oldName, String newName);
void write(String fileName, const char* str);
void write(String fileName, const uint8_t* buf, size_t len);

void readFile(String fileName);

String listDir(String dirName, uint8_t levels = 1);

void streamOpen(String fileName);
void streamWrite(const char* buf, size_t len);
size_t streamRead(char* buf, size_t len);
String streamRead();
size_t streamReadUntil(char* buf, char delimiter, size_t max_len);
void streamClose();
bool streaming();
size_t streamAvailable();
}  // namespace spiffs