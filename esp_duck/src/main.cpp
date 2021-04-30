/*
 * WiFiDuck32Keypad: https://github.com/Jorgen-VikingGod/WiFiDuck32Keypad
 * Copyright (C) 2021 Juergen Skrotzky alias Jorgen (JorgenVikingGod@gmail.com)
 *
 * this software is licensed under the MIT License. See the license file for details.
 * Source: https://github.com/Jorgen-VikingGod/WiFiDucky32Keypad
 * main code is based on the awesome WiFiDuck project
 * Source: https://github.com/spacehuhntech/WiFiDuck
 */

#include <FS.h>
#include <SPI.h>
#include <Wire.h>

#include "config.h"
#include "debug.h"

#include "com.h"
#include "duckscript.h"
#include "webserver.h"
#include "spiffs.h"
#include "settings.h"
#include "profile.h"
#include "cli.h"
#include "led.h"
#include "keypad.h"
#include "display.h"
#include "button.h"

void setup() {
  debug_init();

  delay(200);

  spiffs::begin();
  webserver::begin();

  display::begin();

  com::begin();
  settings::begin();
  cli::begin();

  led::begin();
  keypad::begin();

  profile::begin();

  button::begin();

  com::onDone(duckscript::nextLine);
  com::onError(duckscript::stopAll);
  com::onRepeat(duckscript::repeat);

  if (spiffs::freeBytes() > 0) com::send(MSG_STARTED);

  delay(10);
  com::update();

  debug("\n[~~~ WiFi Duck v");
  debug(VERSION);
  debugln(" Started! ~~~]");
  debugln("    __");
  debugln("___( o)>");
  debugln("\\ <_. )");
  debugln(" `---'   hjw\n");

  duckscript::run(settings::getAutorun());
}

void loop() {
  webserver::update();
  led::update();
  button::update();
  keypad::update();
  com::update();
  debug_update();
}
