/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// The "Arduino.h" header file is intended to only be included by C++ sources.

#if ARDUINO_LIBRARY_DISCOVERY_PHASE == 1
#define ARDUINO_LIB_DISCOVERY_PHASE
#endif
#if !defined(_ARDUINO_MBED_BRIDGE_ARDUINO_H_) && !defined(ARDUINO_LIB_DISCOVERY_PHASE)
#define _ARDUINO_MBED_BRIDGE_ARDUINO_H_

#include "mbed.h"

#define PinMode Arduino_PinMode         // note: this changes the Arduino API for mbed compatibility - use Arduino_PinMode where PinMode was specified in the Arduino API
#include "core-api/api/ArduinoAPI.h"
#undef PinMode

#include "core-extend/ArduinoAPI.h"

#include "bridge/pins.h"

#include "variant.h"                    // user-supplied

#endif // _ARDUINO_MBED_BRIDGE_ARDUINO_H_
