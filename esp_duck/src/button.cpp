/*!
    \file button.cpp
    \brief helper class to handle button clicks
    \author Juergen Skrotzky
    \copyright MIT License
 */

#include "button.h"

#include "debug.h"
#include "config.h"
#include "display.h"
#include "profile.h"
#include <Button2.h>

namespace button {
// ===== PRIVATE ===== //
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

// ===== PUBLIC ====== //
void begin() {
  /*btn1.setLongClickHandler([](Button2& b) {

  });*/
  btn1.setPressedHandler([](Button2& b) { profile::prev(); });
  btn2.setPressedHandler([](Button2& b) { profile::next(); });
}
void update() {
  btn1.loop();
  btn2.loop();
}
}  // namespace button