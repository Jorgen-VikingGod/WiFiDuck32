/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */

#include "spiffs.h"

#include "config.h"
#include "debug.h"

#include <SPI.h>
#include <SPIFFS.h>

namespace spiffs {
File streamFile;

// ===== PRIVATE ===== //
void fixPath(String& path) {
  if (!path.startsWith("/")) {
    path = "/" + path;
  }
}

// ===== PUBLIC ====== //
void begin() {
  debug("Initializing SPIFFS...");
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    debugln("SPIFFS Mount Failed");
    return;
  }
  debugln("OK");

  String FILE_NAME = "/startup_spiffs_test";

  remove(FILE_NAME);
  create(FILE_NAME);
  File f = open(FILE_NAME, FILE_READ);
  if (!f) {
    format();
  } else {
    f.close();
    remove(FILE_NAME);
  }
}

void format() {
  debug("Formatting SPIFFS...");
  SPIFFS.format();
  debugln("OK");
}

size_t size() { return SPIFFS.totalBytes(); }

size_t usedBytes() { return SPIFFS.usedBytes(); }

size_t freeBytes() { return (SPIFFS.totalBytes() - SPIFFS.usedBytes()); }

size_t size(String fileName) {
  fixPath(fileName);

  File f = SPIFFS.open(fileName, FILE_READ);

  return f.size();
}

bool exists(String fileName) { return SPIFFS.exists(fileName); }

File open(String fileName, const char* mode) {
  fixPath(fileName);
  debugf("opening file: %s\n", fileName);
  return SPIFFS.open(fileName, mode);  //"a+");
}

void create(String fileName) {
  fixPath(fileName);
  debugf("Writing file: %s\r\n", fileName);

  File file = SPIFFS.open(fileName, FILE_WRITE);
  file.close();
}

void remove(String fileName) {
  fixPath(fileName);
  debugf("Deleting file: %s\r\n", fileName);

  if (SPIFFS.remove(fileName)) {
    debugln("- file deleted");
  } else {
    debugln("- delete failed");
  }
}

void rename(String oldName, String newName) {
  fixPath(oldName);
  fixPath(newName);

  SPIFFS.rename(oldName, newName);
}

void write(String fileName, const char* str) {
  File f = open(fileName, FILE_WRITE);

  if (f) {
    f.println(str);
    f.close();
    debugln("Wrote file");
  } else {
    debugln("File error");
  }
}

void write(String fileName, const uint8_t* buf, size_t len) {
  File f = open(fileName, FILE_WRITE);

  if (f) {
    f.write(buf, len);
    f.close();
    debugln("Wrote file");
  } else {
    debugln("File error");
  }
}

String listDir(String dirName, uint8_t levels) {
  String res;
  fixPath(dirName);

  debugf("Listing directory: %s\r\n", dirName);

  File root = SPIFFS.open(dirName);
  if (!root) {
    debugln("- failed to open directory");
    return String();
  }
  if (!root.isDirectory()) {
    debugln(" - not a directory");
    return String();
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      debug("  DIR : ");
      debugln(file.name());
      if (levels) {
        listDir(file.name(), levels - 1);
      }
    } else {
      debug("  FILE: ");
      debug(file.name());
      debug("\tSIZE: ");
      debugln(file.size());

      res += file.name();
      res += ' ';
      res += size(file.name());
      res += '\n';
    }
    file = root.openNextFile();
  }

  if (res.length() == 0) {
    res += "\n";
  }

  return res;
}

void readFile(String fileName) {
  fixPath(fileName);
  debugf("Reading file: %s\n", fileName);

  File file = SPIFFS.open(fileName, FILE_READ);
  if (!file) {
    debugln("Failed to open file for reading");
    return;
  }

  String debugLogData;
  debug("Read from file: ");
  while (file.available()) {
    debugLogData += char(file.read());
  }
  file.close();
  Serial.println(debugLogData);
}

void streamOpen(String fileName) {
  streamClose();
  streamFile = open(fileName, FILE_READ);
  if (!streamFile) debugln("ERROR: No stream file open");
}

void streamWrite(const char* buf, size_t len) {
  if (streamFile) {
    streamFile = open(streamFile.name(), FILE_WRITE);
    streamFile.write((uint8_t*)buf, len);
  } else {
    debugln("ERROR: No stream file open");
  }
}

String streamRead() {
  if (streamFile) {
    streamFile = open(streamFile.name(), FILE_READ);
    String debugLogData;
    debug("Read from file: ");
    while (streamFile.available()) {
      debugLogData += char(streamFile.read());
    }
    streamFile.close();
    debugln(debugLogData);
    return debugLogData;
  } else {
    debugln("ERROR: No stream file open");
    return String();
  }
}

size_t streamRead(char* buf, size_t len) {
  if (streamFile) {
    streamFile = open(streamFile.name(), FILE_READ);
    size_t i;

    for (i = 0; i < len; ++i) {
      if (!streamFile.available() || (i == len - 1)) {
        buf[i] = '\0';
        break;
      } else {
        buf[i] = streamFile.read();
      }
    }

    return i;
  } else {
    debugln("ERROR: No stream file open");
    return 0;
  }
}

size_t streamReadUntil(char* buf, char delimiter, size_t max_len) {
  if (streamFile) {
    size_t i;
    char c = 'x';

    for (i = 0; i < max_len; ++i) {
      if ((c == delimiter) || !streamFile.available() || (i == max_len - 1)) {
        buf[i] = '\0';
        break;
      } else {
        c = streamFile.read();
        buf[i] = c;
      }
    }

    return i;
  } else {
    debugln("ERROR: No stream file open");
    return 0;
  }
}

void streamClose() { streamFile.close(); }

bool streaming() { return streamFile; }

size_t streamAvailable() {
  if (!streamFile) return 0;
  return streamFile.available();
}
}  // namespace spiffs