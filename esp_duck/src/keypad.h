/*!
    \file keypad.h
    \brief helper class to access keypad and handle key clicks
    \author Juergen Skrotzky
    \copyright MIT License
 */

#pragma once

#include <Arduino.h>  // String
#include "profile.h"

namespace keypad {
void begin();

void update();
void setProfileConfig(const profile::Config& config);
void updateProfileColors();
}  // namespace keypad