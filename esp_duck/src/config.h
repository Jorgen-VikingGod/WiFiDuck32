/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */

#pragma once

#define VERSION "0.1.0"

/*! ===== DEBUG Settings ===== */
#define ENABLE_DEBUG
#define DEBUG_PORT Serial
#define DEBUG_BAUD 115200

/*! ===== Communication Settings ===== */
// #define ENABLE_SERIAL
#define SERIAL_PORT Serial
#define SERIAL_BAUD 9600

// #define ENABLE_I2C
#define I2C_ADDR 0x31
// #define I2C_SDA 4
// #define I2C_SCL 5
#define I2C_CLOCK_SPEED 100000L

#define BUFFER_SIZE 384
#define PACKET_SIZE 32

#define MSG_CONNECTED "LED 255 0 255\n"
#define MSG_STARTED "LED 0 255 0\n"

/*! ===== EEPROM Settings ===== */
#define EEPROM_SIZE 4095
#define EEPROM_BOOT_ADDR 3210
#define BOOT_MAGIC_NUM 1234567890

/*! ======= SPIFFS FS ======= */
#define FORMAT_SPIFFS_IF_FAILED true

/*! ===== WiFi Settings ===== */
#define WIFI_SSID "wifiduck"
#define WIFI_PASSWORD "wifiduck"
#define WIFI_CHANNEL "1"

#define STA_SSID "TESTNETWORK-SSID"
#define STA_PASSWORD "TESTNETWORK-PASSWORD"

#define WIFI_TIMEOUT_MS 20000       // 20 second WiFi connection timeout
#define WIFI_RECOVER_TIME_MS 30000  // Wait 30 seconds after a failed connection attempt

#define ARDUINO_RUNNING_CORE 1

#define HOSTNAME "wifiduck"
#define URL "wifi.duck"

#define EDITOR_USER "wifiduck"
#define EDITOR_PASSWORD "wifiduck"

/*! ======= KEYPAD ======== */
#define COLS 4          // Number of switch columns
#define ROWS 3          // Number of switch rows
#define MAX_DEBOUNCE 3  // Software defined debounce

/*! ====== NEOPIXEL =======*/
#define NEOPIXEL_NUM 11
#define LED_PIN 2

/*! ====== ENCODER ======*/
#define ENCODER_A_PIN 39       // 36
#define ENCODER_B_PIN 36       // 37
#define ENCODER_BUTTON_PIN 32  // 32
#define ENCODER_MODE_ROTARY 1  // rotary encoder, comment = no rotary pules, only 2 states (left, right)

/*! ========== Safty checks ========== */
#if !defined(ENABLE_I2C) && !defined(ENABLE_SERIAL)
#define ENABLE_I2C
#define I2C_SDA 21
#define I2C_SCL 22
#endif /* if !defined(ENABLE_I2C) || !defined(ENABLE_SERIAL) */

#if !defined(ESP32)
#error You are compiling for the wrong board, mate! Select something with an ESP32.
#endif /* ifdef DUCKMCU && DUCKMCU!="ATMEGA32U4" */

#if defined(ENABLE_DEBUG) && defined(ENABLE_SERIAL) && DEBUG_PORT == SERIAL_PORT
#error Using same serial port for debugging and Communication!\
    Use I2C instead or disable debug.
#endif /* if DEBUG_PORT == SERIAL_PORT */

#if defined(ENABLE_I2C) && I2C_SDA == I2C_SCL
#error SDA pin equals to SCL pin
#endif /* if !defined(ENABLE_I2C) && !defined(ENABLE_I2C) */

#if defined(ENABLE_I2C) && defined(ENABLE_SERIAL) && (I2C_SDA == 1 || I2C_SDA == 3 || I2C_SCL == 1 || I2C_SCL == 3)
#error I2C pins overlap with RX and TX pins. Disable serial debugging or change the I2C pins.
#endif /* if !defined(ENABLE_I2C) && !defined(ENABLE_I2C) */
