/*!
    \file keypad.cpp
    \brief helper class to access keypad and handle key clicks
    \author Juergen Skrotzky
    \copyright MIT License
 */

#include "keypad.h"

#include "config.h"
#include "debug.h"
#include "profile.h"
#include "duckscript.h"
#include "led.h"
#include <Adafruit_Keypad.h>
#include <Button2.h>
#include <RobustRotaryEncoder.h>

namespace keypad {
// ===== PRIVATE ===== //

// key state (helper for keypad)
struct KeyState {
  bool active;
  profile::KeyColor up;
  profile::KeyColor down;
  uint8_t up_speed;
  uint8_t down_speed;
  KeyState(bool keyActive = false, profile::KeyColor keyUp = profile::KeyColor(false),
           profile::KeyColor keyDown = profile::KeyColor(false), uint8_t iUpSpeed = 0, uint8_t iDownSpeed = 25) {
    active = keyActive;
    up = keyUp;
    down = keyDown;
    up_speed = iUpSpeed;
    down_speed = iDownSpeed;
  }
};
KeyState keyStates[NEOPIXEL_NUM] = {KeyState(), KeyState(), KeyState(), KeyState(), KeyState(), KeyState(),
                                    KeyState(), KeyState(), KeyState(), KeyState(), KeyState()};
// Pins connected to rows
uint8_t rowPins[ROWS] = {13, 12, 14};  //{15, 13, 12};
// Pins connected to columns
uint8_t colPins[COLS] = {27, 26, 25, 33};  //{33, 25, 26, 27};
// define the symbols on the buttons of the keypads
uint8_t keys[COLS][ROWS] = {{0, 1, 2}, {3, 7, 6}, {5, 4, 8}, {9, 10, 11}};

// initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ESP32Encoder encoder;
Button2 encoderBtn(ENCODER_BUTTON_PIN);

RobustRotaryEncoder encoder;

void encoderSetup() {
  // we must initialize rorary encoder
  encoderBtn.setPressedHandler([](Button2& b) {
    String fileName = profile::fileNameByEncoder(0);
    duckscript::run(fileName);
  });
  // configure encoder
  encoder.begin(ENCODER_A_PIN, ENCODER_B_PIN);
}

void encoderLoop() {
  encoderBtn.loop();
  if (encoder.read()) {
    // rotary encoder down
    if (encoder.isCCW()) {
      String fileName = profile::fileNameByEncoder(2);
      duckscript::run(fileName);
    }
    // rotary encoder up
    if (encoder.isCW()) {
      String fileName = profile::fileNameByEncoder(1);
      duckscript::run(fileName);
    }
  }
}

// ===== PUBLIC ====== //
void begin() {
  encoderSetup();
  // initialize keypad
  customKeypad.begin();
}

void update() {
  encoderLoop();
  // put your main code here, to run repeatedly:
  customKeypad.tick();

  while (customKeypad.available()) {
    keypadEvent e = customKeypad.read();
    uint8_t keyIndex = (uint8_t)e.bit.KEY;
    if (!keyStates[keyIndex].active) continue;
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      profile::KeyColor col = keyStates[keyIndex].down;
      led::crossFade(keyIndex, col.r, col.g, col.b, keyStates[keyIndex].down_speed, 1);
      String fileName = profile::fileNameByKey(keyIndex);
      duckscript::run(fileName);
    } else if (e.bit.EVENT == KEY_JUST_RELEASED) {
      profile::KeyColor col = keyStates[keyIndex].up;
      led::crossFade(keyIndex, col.r, col.g, col.b, keyStates[keyIndex].up_speed, 1);
      String fileName = profile::fileNameByKey(keyIndex);
      duckscript::stop(fileName);
    }
  }
}

void assignKeyState(uint8_t k, const profile::KeyConfig& keyConfig, const profile::Config& globalConfig) {
  keyStates[k].active = keyConfig.active;
  keyStates[k].up = keyConfig.bg_color.use ? keyConfig.bg_color : globalConfig.bg_color;
  keyStates[k].down = keyConfig.down_color.use ? keyConfig.down_color : globalConfig.down_color;
  keyStates[k].up_speed = keyConfig.up_speed;
  keyStates[k].down_speed = keyConfig.down_speed;
}

void setProfileConfig(const profile::Config& config) {
  // assign key states
  assignKeyState(0, config.key1, config);
  assignKeyState(1, config.key2, config);
  assignKeyState(2, config.key3, config);
  assignKeyState(3, config.key4, config);
  assignKeyState(4, config.key5, config);
  assignKeyState(5, config.key6, config);
  assignKeyState(6, config.key7, config);
  assignKeyState(7, config.key8, config);
  assignKeyState(8, config.key9, config);
  assignKeyState(9, config.key10, config);
  assignKeyState(10, config.key11, config);
  /*
  // show initial colors
  for (uint8_t k = 0; k < NEOPIXEL_NUM; k++) {
    profile::KeyColor col = keyStates[k].active ? keyStates[k].up : profile::KeyColor();
    led::crossFade(k, col.r, col.g, col.b, keyStates[k].up_speed, 1);
  }
  */
}

void updateProfileColors() {
  // show initial colors
  for (uint8_t k = 0; k < NEOPIXEL_NUM; k++) {
    profile::KeyColor col = keyStates[k].active ? keyStates[k].down : profile::KeyColor();
    led::setColor(k, col.r, col.g, col.b);
  }
  led::update();
  delay(500);
  // show initial colors
  for (uint8_t k = 0; k < NEOPIXEL_NUM; k++) {
    profile::KeyColor col = keyStates[k].active ? keyStates[k].up : profile::KeyColor();
    led::crossFade(k, col.r, col.g, col.b, keyStates[k].up_speed, 1);
  }
}

}  // namespace keypad