/*
  This software is licensed under the MIT License. See the license file for details.
  Source: https://github.com/spacehuhntech/WiFiDuck
*/

#include "config.h"
#include "debug.h"

// #include "keyboard.h"
#include "com.h"
#include "duckparser.h"
#include "serial_bridge.h"

//Include this module whether using Arduino stack or TinyUSB stack
#include <TinyUSB_Mouse_and_Keyboard.h>

// ===== SETUP ====== //
void setup() {
  debug_init();

  serial_bridge::begin();
  Keyboard.begin();
  Mouse.begin();
  //keyboard::begin();
  com::begin();

  debugs("Started! ");
  debugln(VERSION);
}

// ===== LOOOP ===== //
void loop() {
  com::update();
  if (com::hasData()) {
    const buffer_t& buffer = com::getBuffer();

    debugs("Interpreting: ");

    for (size_t i = 0; i < buffer.len; i++) debug(buffer.data[i]);

    duckparser::parse(buffer.data, buffer.len);

    com::sendDone();
  }
}
