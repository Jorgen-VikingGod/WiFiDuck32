/*!
    \file profile.cpp
    \brief helper class to manage macro profiles (/profile1/config.json)
    \author Juergen Skrotzky
    \copyright MIT License
 */

#include "profile.h"

#include "config.h"
#include "debug.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "spiffs.h"
#include "display.h"
#include "settings.h"
#include "keypad.h"

namespace profile {
// ===== PRIVATE ===== //
Config config;
// Current profile index
const uint8_t maxProfiles = 10;
const String profiles[maxProfiles] = {"/profile1/", "/profile2/", "/profile3/", "/profile4/", "/profile5/",
                                      "/profile6/", "/profile7/", "/profile8/", "/profile9/", "/profile10/"};

const String keys[11] = {"key1.txt", "key2.txt", "key3.txt", "key4.txt",  "key5.txt", "key6.txt",
                         "key7.txt", "key8.txt", "key9.txt", "key10.txt", "key11.txt"};

const String encoder[3] = {"enc.txt", "enc_up.txt", "enc_down.txt"};

static uint8_t currentIndex = 0;

static KeyColor stringToColor(String cStr) {
  KeyColor col;
  int posR = 0;
  int posG = cStr.indexOf(',');
  int posB = cStr.indexOf(',', posG + 1);
  col.r = cStr.substring(posR, posG).toInt();
  col.g = cStr.substring(posG + 1, posB).toInt();
  col.b = cStr.substring(posB + 1).toInt();

#if 0
  debug("stringToColor(");
  debug(cStr);
  debug(") > (");
  debug(col.r);
  debug(", ");
  debug(col.g);
  debug(", ");
  debug(col.b);
  debugln(") ");
#endif
  return col;
}

static String colorToString(KeyColor col) { return String(col.r) + "," + String(col.g) + "," + String(col.b); }

// ===== PUBLIC ====== //
void begin() {
  uint8_t storedProfile = settings::getProfileNum();
  debugln(settings::toString());
  setProfile(storedProfile - 1);
}
void update() {}

void prev() {
  currentIndex = (currentIndex < 1) ? maxProfiles : currentIndex - 1;
  profile::setProfile(currentIndex);
}
void next() {
  currentIndex = (currentIndex > maxProfiles - 1) ? 0 : currentIndex + 1;
  profile::setProfile(currentIndex);
}

void setProfile(uint8_t p) {
  if (p >= 0 && p < maxProfiles) {
    currentIndex = p;
  } else {
    currentIndex = 0;
  }
  settings::setProfile(String(currentIndex + 1).c_str());

  String fileName = currentProfile() + "config.json";
  if (spiffs::exists(fileName)) {
    loadConfiguration();
  } else {
    resetConfiguration();
    saveConfiguration();
  }

  for (uint8_t k = 0; k < 11; k++) {
    if (!spiffs::exists(fileNameByKey(k))) {
      spiffs::create(fileNameByKey(k));
    }
  }
  for (uint8_t e = 0; e < 3; e++) {
    if (!spiffs::exists(fileNameByEncoder(e))) {
      spiffs::create(fileNameByEncoder(e));
    }
  }
}

const String currentProfile() { return profiles[currentIndex]; }

// reset local configuration
void resetConfiguration() { config = Config(); }

// Loads the configuration from a file
void loadConfiguration() {
  // Open file for reading
  String fileName = currentProfile() + "config.json";
  File file = SPIFFS.open(fileName);
  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<4096> doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    debugf("Failed to read file: %s\r\n", fileName);
    return;
  }
  // Copy values from the JsonDocument to the Config
  strlcpy(config.name, doc["name"] | "profile1", sizeof(config.name));
  config.bg_color = profile::stringToColor(doc["bg_color"].as<String>());
  config.down_color = profile::stringToColor(doc["down_color"].as<String>());
  config.size = doc["size"] | 2;
  config.up_speed = doc["up_speed"] | 10;
  config.down_speed = doc["down_speed"] | 0;

  JsonObject key1 = doc["key1"];
  if (!key1.isNull()) {
    config.key1.active = true;
    strcpy(config.key1.name, key1["name"] | "key1");
    config.key1.bg_color = key1["bg_color"].isNull() ? KeyColor(false) : stringToColor(key1["bg_color"].as<String>());
    config.key1.down_color =
        key1["down_color"].isNull() ? KeyColor(false) : stringToColor(key1["down_color"].as<String>());
    config.key1.size = key1["size"] | 0;
    config.key1.up_speed = key1["up_speed"] | config.up_speed;
    config.key1.down_speed = key1["down_speed"] | config.down_speed;
  } else {
    config.key1.active = false;
  }

  JsonObject key2 = doc["key2"];
  if (!key2.isNull()) {
    config.key2.active = true;
    strcpy(config.key2.name, key2["name"] | "key2");
    config.key2.bg_color =
        key2["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(key2["bg_color"].as<String>());
    config.key2.down_color =
        key2["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(key2["down_color"].as<String>());
    config.key2.size = key2["size"] | 0;
    config.key2.up_speed = key2["up_speed"] | config.up_speed;
    config.key2.down_speed = key2["down_speed"] | config.down_speed;
    debugln(config.key2.name);
  } else {
    config.key2.active = false;
  }

  JsonObject key3 = doc["key3"];
  if (!key3.isNull()) {
    config.key3.active = true;
    strcpy(config.key3.name, key3["name"] | "key3");
    config.key3.bg_color =
        key3["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(key3["bg_color"].as<String>());
    config.key3.down_color =
        key3["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(key3["down_color"].as<String>());
    config.key3.size = key3["size"] | 0;
    config.key3.up_speed = key3["up_speed"] | config.up_speed;
    config.key3.down_speed = key3["down_speed"] | config.down_speed;
  } else {
    config.key3.active = false;
  }

  JsonObject key4 = doc["key4"];
  if (!key4.isNull()) {
    config.key4.active = true;
    strcpy(config.key4.name, key4["name"] | "key4");
    config.key4.bg_color =
        key4["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(key4["bg_color"].as<String>());
    config.key4.down_color =
        key4["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(key4["down_color"].as<String>());
    config.key4.size = key4["size"] | 0;
    config.key4.up_speed = key4["up_speed"] | config.up_speed;
    config.key4.down_speed = key4["down_speed"] | config.down_speed;
  } else {
    config.key4.active = false;
  }

  JsonObject key5 = doc["key5"];
  if (!key5.isNull()) {
    config.key5.active = true;
    strcpy(config.key5.name, key5["name"] | "key5");
    config.key5.bg_color =
        key5["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(key5["bg_color"].as<String>());
    config.key5.down_color =
        key5["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(key5["down_color"].as<String>());
    config.key5.size = key5["size"] | 0;
    config.key5.up_speed = key5["up_speed"] | config.up_speed;
    config.key5.down_speed = key5["down_speed"] | config.down_speed;
  } else {
    config.key5.active = false;
  }

  JsonObject key6 = doc["key6"];
  if (!key6.isNull()) {
    config.key6.active = true;
    strcpy(config.key6.name, key6["name"] | "key6");
    config.key6.bg_color =
        key6["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(key6["bg_color"].as<String>());
    config.key6.down_color =
        key6["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(key6["down_color"].as<String>());
    config.key6.size = key6["size"] | 0;
    config.key6.up_speed = key6["up_speed"] | config.up_speed;
    config.key6.down_speed = key6["down_speed"] | config.down_speed;
  } else {
    config.key6.active = false;
  }

  JsonObject key7 = doc["key7"];
  if (!key7.isNull()) {
    config.key7.active = true;
    strcpy(config.key7.name, key7["name"] | "key7");
    config.key7.bg_color =
        key7["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(key7["bg_color"].as<String>());
    config.key7.down_color =
        key7["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(key7["down_color"].as<String>());
    config.key7.size = key7["size"] | 0;
    config.key7.up_speed = key7["up_speed"] | config.up_speed;
    config.key7.down_speed = key7["down_speed"] | config.down_speed;
  } else {
    config.key7.active = false;
  }

  JsonObject key8 = doc["key8"];
  if (!key8.isNull()) {
    config.key8.active = true;
    strcpy(config.key8.name, key8["name"] | "key8");
    config.key8.bg_color =
        key8["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(key8["bg_color"].as<String>());
    config.key8.down_color =
        key8["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(key8["down_color"].as<String>());
    config.key8.size = key8["size"] | 0;
    config.key8.up_speed = key8["up_speed"] | config.up_speed;
    config.key8.down_speed = key8["down_speed"] | config.down_speed;
  } else {
    config.key8.active = false;
  }

  JsonObject key9 = doc["key9"];
  if (!key9.isNull()) {
    config.key9.active = true;
    strcpy(config.key9.name, key9["name"] | "key9");
    config.key9.bg_color =
        key9["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(key9["bg_color"].as<String>());
    config.key9.down_color =
        key9["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(key9["down_color"].as<String>());
    config.key9.size = key9["size"] | 0;
    config.key9.up_speed = key9["up_speed"] | config.up_speed;
    config.key9.down_speed = key9["down_speed"] | config.down_speed;
  } else {
    config.key9.active = false;
  }

  JsonObject key10 = doc["key10"];
  if (!key10.isNull()) {
    config.key10.active = true;
    strcpy(config.key10.name, key10["name"] | "key10");
    config.key10.bg_color =
        key10["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(key10["bg_color"].as<String>());
    config.key10.down_color =
        key10["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(key10["down_color"].as<String>());
    config.key10.size = key10["size"] | 0;
    config.key10.up_speed = key10["up_speed"] | config.up_speed;
    config.key10.down_speed = key10["down_speed"] | config.down_speed;
  } else {
    config.key10.active = false;
  }

  JsonObject key11 = doc["key11"];
  if (!key11.isNull()) {
    config.key11.active = true;
    strcpy(config.key11.name, key11["name"] | "key11");
    config.key11.bg_color =
        key11["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(key11["bg_color"].as<String>());
    config.key11.down_color =
        key11["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(key11["down_color"].as<String>());
    config.key11.size = key11["size"] | 0;
    config.key11.up_speed = key11["up_speed"] | config.up_speed;
    config.key11.down_speed = key11["down_speed"] | config.down_speed;
  } else {
    config.key11.active = false;
  }

  JsonObject enc = doc["enc"];
  if (!enc.isNull()) {
    config.enc.active = true;
    strcpy(config.enc.name, enc["name"] | "enc");
    config.enc.bg_color =
        enc["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(enc["bg_color"].as<String>());
    config.enc.down_color =
        enc["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(enc["down_color"].as<String>());
    config.enc.size = enc["size"] | 0;
  } else {
    config.enc.active = false;
  }

  JsonObject enc_up = doc["enc_up"];
  if (!enc_up.isNull()) {
    config.enc_up.active = true;
    strcpy(config.enc_up.name, enc_up["name"] | "enc_up");
    config.enc_up.bg_color =
        enc_up["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(enc_up["bg_color"].as<String>());
    config.enc_up.down_color =
        enc_up["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(enc_up["down_color"].as<String>());
    config.enc_up.size = enc_up["size"] | 0;
  } else {
    config.enc_up.active = false;
  }

  JsonObject enc_down = doc["enc_down"];
  if (!enc_down.isNull()) {
    config.enc_down.active = true;
    strcpy(config.enc_down.name, enc_down["name"] | "enc_down");
    config.enc_down.bg_color =
        enc_down["bg_color"].isNull() ? KeyColor(false) : profile::stringToColor(enc_down["bg_color"].as<String>());
    config.enc_down.down_color =
        enc_down["down_color"].isNull() ? KeyColor(false) : profile::stringToColor(enc_down["down_color"].as<String>());
    config.enc_down.size = enc_down["size"] | 0;
  } else {
    config.enc_down.active = false;
  }
  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();

  keypad::setProfileConfig(config);
  showMenu();
  keypad::updateProfileColors();
}

// Saves the configuration to a file
void saveConfiguration() {
  // Delete existing file, otherwise the configuration is appended to the file
  String fileName = currentProfile() + "config.json";
  SPIFFS.remove(fileName);

  // Open file for writing
  File file = SPIFFS.open(fileName, FILE_WRITE);
  if (!file) {
    debugf("Failed to create file: %s\r\n", fileName);
    return;
  }
  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<4096> doc;
  doc["name"] = config.name;
  doc["bg_color"] = profile::colorToString(config.bg_color);
  doc["down_color"] = profile::colorToString(config.down_color);
  doc["size"] = config.size;
  doc["up_speed"] = config.up_speed;
  doc["down_speed"] = config.down_speed;

  JsonObject key1 = doc.createNestedObject("key1");
  key1["name"] = config.key1.name;
  if (config.key1.bg_color.use) key1["bg_color"] = profile::colorToString(config.key1.bg_color);
  if (config.key1.down_color.use) key1["down_color"] = profile::colorToString(config.key1.down_color);
  if (config.key1.size > 0) key1["size"] = config.key1.size;
  if (config.key1.up_speed != config.up_speed) key1["up_speed"] = config.key1.up_speed;
  if (config.key1.down_speed != config.down_speed) key1["down_speed"] = config.key1.down_speed;

  JsonObject key2 = doc.createNestedObject("key2");
  key2["name"] = config.key2.name;
  if (config.key2.bg_color.use) key2["bg_color"] = profile::colorToString(config.key2.bg_color);
  if (config.key2.down_color.use) key2["down_color"] = profile::colorToString(config.key2.down_color);
  if (config.key2.size > 0) key2["size"] = config.key2.size;
  if (config.key2.up_speed != config.up_speed) key2["up_speed"] = config.key2.up_speed;
  if (config.key2.down_speed != config.down_speed) key2["down_speed"] = config.key2.down_speed;

  JsonObject key3 = doc.createNestedObject("key3");
  key3["name"] = config.key3.name;
  if (config.key3.bg_color.use) key3["bg_color"] = profile::colorToString(config.key3.bg_color);
  if (config.key3.down_color.use) key3["down_color"] = profile::colorToString(config.key3.down_color);
  if (config.key3.size > 0) key3["size"] = config.key3.size;
  if (config.key3.up_speed != config.up_speed) key3["up_speed"] = config.key3.up_speed;
  if (config.key3.down_speed != config.down_speed) key3["down_speed"] = config.key3.down_speed;

  JsonObject key4 = doc.createNestedObject("key4");
  key4["name"] = config.key4.name;
  if (config.key4.bg_color.use) key4["bg_color"] = profile::colorToString(config.key4.bg_color);
  if (config.key4.down_color.use) key4["down_color"] = profile::colorToString(config.key4.down_color);
  if (config.key4.size > 0) key4["size"] = config.key4.size;
  if (config.key4.up_speed != config.up_speed) key4["up_speed"] = config.key4.up_speed;
  if (config.key4.down_speed != config.down_speed) key4["down_speed"] = config.key4.down_speed;

  JsonObject key5 = doc.createNestedObject("key5");
  key5["name"] = config.key5.name;
  if (config.key5.bg_color.use) key5["bg_color"] = profile::colorToString(config.key5.bg_color);
  if (config.key5.down_color.use) key5["down_color"] = profile::colorToString(config.key5.down_color);
  if (config.key5.size > 0) key5["size"] = config.key5.size;
  if (config.key5.up_speed != config.up_speed) key5["up_speed"] = config.key5.up_speed;
  if (config.key5.down_speed != config.down_speed) key5["down_speed"] = config.key5.down_speed;

  JsonObject key6 = doc.createNestedObject("key6");
  key6["name"] = config.key6.name;
  if (config.key6.bg_color.use) key6["bg_color"] = profile::colorToString(config.key6.bg_color);
  if (config.key6.down_color.use) key6["down_color"] = profile::colorToString(config.key6.down_color);
  if (config.key6.size > 0) key6["size"] = config.key6.size;
  if (config.key6.up_speed != config.up_speed) key6["up_speed"] = config.key6.up_speed;
  if (config.key6.down_speed != config.down_speed) key6["down_speed"] = config.key6.down_speed;

  JsonObject key7 = doc.createNestedObject("key7");
  key7["name"] = config.key7.name;
  if (config.key7.bg_color.use) key7["bg_color"] = profile::colorToString(config.key7.bg_color);
  if (config.key7.down_color.use) key7["down_color"] = profile::colorToString(config.key7.down_color);
  if (config.key7.size > 0) key7["size"] = config.key7.size;
  if (config.key7.up_speed != config.up_speed) key7["up_speed"] = config.key7.up_speed;
  if (config.key7.down_speed != config.down_speed) key7["down_speed"] = config.key7.down_speed;

  JsonObject key8 = doc.createNestedObject("key8");
  key8["name"] = config.key8.name;
  if (config.key8.bg_color.use) key8["bg_color"] = profile::colorToString(config.key8.bg_color);
  if (config.key8.down_color.use) key8["down_color"] = profile::colorToString(config.key8.down_color);
  if (config.key8.size > 0) key8["size"] = config.key8.size;
  if (config.key8.up_speed != config.up_speed) key8["up_speed"] = config.key8.up_speed;
  if (config.key8.down_speed != config.down_speed) key8["down_speed"] = config.key8.down_speed;

  JsonObject key9 = doc.createNestedObject("key9");
  key9["name"] = config.key9.name;
  if (config.key9.bg_color.use) key9["bg_color"] = profile::colorToString(config.key9.bg_color);
  if (config.key9.down_color.use) key9["down_color"] = profile::colorToString(config.key9.down_color);
  if (config.key9.size > 0) key9["size"] = config.key9.size;
  if (config.key9.up_speed != config.up_speed) key9["up_speed"] = config.key9.up_speed;
  if (config.key9.down_speed != config.down_speed) key9["down_speed"] = config.key9.down_speed;

  JsonObject key10 = doc.createNestedObject("key10");
  key10["name"] = config.key10.name;
  if (config.key10.bg_color.use) key10["bg_color"] = profile::colorToString(config.key10.bg_color);
  if (config.key10.down_color.use) key10["down_color"] = profile::colorToString(config.key10.down_color);
  if (config.key10.size > 0) key10["size"] = config.key10.size;
  if (config.key10.up_speed != config.up_speed) key10["up_speed"] = config.key10.up_speed;
  if (config.key10.down_speed != config.down_speed) key10["down_speed"] = config.key10.down_speed;

  JsonObject key11 = doc.createNestedObject("key11");
  key11["name"] = config.key11.name;
  if (config.key11.bg_color.use) key11["bg_color"] = profile::colorToString(config.key11.bg_color);
  if (config.key11.down_color.use) key11["down_color"] = profile::colorToString(config.key11.down_color);
  if (config.key11.size > 0) key11["size"] = config.key11.size;
  if (config.key11.up_speed != config.up_speed) key11["up_speed"] = config.key11.up_speed;
  if (config.key11.down_speed != config.down_speed) key11["down_speed"] = config.key11.down_speed;

  JsonObject enc = doc.createNestedObject("enc");
  enc["name"] = config.enc.name;
  if (config.enc.bg_color.use) enc["bg_color"] = profile::colorToString(config.enc.bg_color);
  if (config.enc.down_color.use) enc["down_color"] = profile::colorToString(config.enc.down_color);
  if (config.enc.size > 0) enc["size"] = config.enc.size;

  JsonObject enc_up = doc.createNestedObject("enc_up");
  enc_up["name"] = config.enc_up.name;
  if (config.enc_up.bg_color.use) enc_up["bg_color"] = profile::colorToString(config.enc_up.bg_color);
  if (config.enc_up.down_color.use) enc_up["down_color"] = profile::colorToString(config.enc_up.down_color);
  if (config.enc_up.size > 0) enc_up["size"] = config.enc_up.size;

  JsonObject enc_down = doc.createNestedObject("enc_down");
  enc_down["name"] = config.enc_down.name;
  if (config.enc_down.bg_color.use) enc_down["bg_color"] = profile::colorToString(config.enc_down.bg_color);
  if (config.enc_down.down_color.use) enc_down["down_color"] = profile::colorToString(config.enc_down.down_color);
  if (config.enc_down.size > 0) enc_down["size"] = config.enc_down.size;
  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    debugf("Failed to write to file: %s\r\n", fileName);
  }
  // Close the file
  file.close();
}

uint16_t colorByConfig(KeyColor color) {
  KeyColor col = color.use ? color : config.down_color;
  return display::colorRgbTo16(col.r, col.g, col.b);
}
uint8_t sizeByConfig(uint8_t size) { return (size > 0 ? size : config.size); }

void showMenu() {
  int rectWidth = display::width() / COLS - 2;
  int rectHeight = display::height() / COLS - 2;
  display::fillScreen(display::color24to16(0x0));

  if (config.key1.active)
    display::drawRect(0, 0, rectWidth, rectHeight, config.key1.name, colorByConfig(config.key1.down_color),
                      sizeByConfig(config.key1.size));
  if (config.key2.active)
    display::drawRect(rectWidth, 0, rectWidth, rectHeight, config.key2.name, colorByConfig(config.key2.down_color),
                      sizeByConfig(config.key2.size));
  if (config.key3.active)
    display::drawRect(rectWidth * 2, 0, rectWidth, rectHeight, config.key3.name, colorByConfig(config.key3.down_color),
                      sizeByConfig(config.key3.size));
  if (config.key4.active)
    display::drawRect(rectWidth * 3, 0, rectWidth, rectHeight, config.key4.name, colorByConfig(config.key4.down_color),
                      sizeByConfig(config.key4.size));

  if (config.key8.active)
    display::drawRect(0, rectHeight, rectWidth, rectHeight, config.key8.name, colorByConfig(config.key8.down_color),
                      sizeByConfig(config.key8.size));
  if (config.key7.active)
    display::drawRect(rectWidth, rectHeight, rectWidth, rectHeight, config.key7.name,
                      colorByConfig(config.key7.down_color), sizeByConfig(config.key7.size));
  if (config.key6.active)
    display::drawRect(rectWidth * 2, rectHeight, rectWidth, rectHeight, config.key6.name,
                      colorByConfig(config.key6.down_color), sizeByConfig(config.key6.size));
  if (config.key5.active)
    display::drawRect(rectWidth * 3, rectHeight, rectWidth, rectHeight, config.key5.name,
                      colorByConfig(config.key5.down_color), sizeByConfig(config.key5.size));

  if (config.key9.active)
    display::drawRect(0, rectHeight * 2, rectWidth, rectHeight, config.key9.name, colorByConfig(config.key9.down_color),
                      sizeByConfig(config.key9.size));
  if (config.key10.active)
    display::drawRect(rectWidth, rectHeight * 2, rectWidth, rectHeight, config.key10.name,
                      colorByConfig(config.key10.down_color), sizeByConfig(config.key10.size));
  if (config.key11.active)
    display::drawRect(rectWidth * 2, rectHeight * 2, rectWidth, rectHeight, config.key11.name,
                      colorByConfig(config.key11.down_color), sizeByConfig(config.key11.size));

  if (config.enc.active) {
    display::drawCircle(rectWidth * 3 + 1, rectHeight * 2 + 1, rectWidth + 6, rectHeight + 6, config.enc.name,
                        colorByConfig(config.enc.down_color), sizeByConfig(config.enc.size));
  }
  if (config.enc_down.active) {
    display::drawArrow(rectWidth * 3 + 1, rectHeight * 3 + 18, (rectWidth + 6) / 2, rectHeight / 2,
                       config.enc_down.name, colorByConfig(config.enc_down.down_color), 1, true);
  }
  if (config.enc_up.active) {
    display::drawArrow(rectWidth * 3.5 + 4, rectHeight * 3 + 18, (rectWidth + 6) / 2, rectHeight / 2,
                       config.enc_up.name, colorByConfig(config.enc_up.down_color), 1, false);
  }

  display::drawTitle(10, rectHeight * 3 + 24, config.name,
                     display::colorRgbTo16(config.down_color.r, config.down_color.g, config.down_color.b), 3);
}

const Config getConfig() { return config; }

const String fileNameByKey(uint8_t k) {
  if (k > 0 && k < 12) {
    return currentProfile() + keys[k];
  } else {
    return currentProfile() + keys[0];
  }
}

const String fileNameByEncoder(uint8_t e) {
  if (e > 0 && e < 3) {
    return currentProfile() + encoder[e];
  } else {
    return currentProfile() + encoder[0];
  }
}

}  // namespace profile