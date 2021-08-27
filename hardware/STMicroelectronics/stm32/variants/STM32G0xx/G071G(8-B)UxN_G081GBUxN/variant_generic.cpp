/*
 *******************************************************************************
 * Copyright (c) 2020-2021, STMicroelectronics
 * All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */
#if defined(ARDUINO_GENERIC_G071G8UXN) || defined(ARDUINO_GENERIC_G071GBUXN) ||\
    defined(ARDUINO_GENERIC_G081GBUXN)
#include "pins_arduino.h"

// Digital PinName array
const PinName digitalPin[] = {
  PA_0,   // D1/A0
  PA_1,   // D2/A1
  PA_2,   // D3/A2
  PA_3,   // D4/A3
  PA_4,   // D5/A4
  PA_5,   // D6/A5
  PA_6,   // D7/A6
  PA_7,   // D8/A7
  PA_8,   // D9
  PA_11,  // D10
  PA_12,  // D11
  PA_13,  // D12
  PA_14,  // D13
  PB_0,   // D14/A8
  PB_6,   // D15
  PB_7,   // D16
  PB_8,   // D17
  PB_15,  // D18
  PC_6,   // D19
  PC_14,  // D20
  PC_15,  // D21
  PD_0,   // D22
  PD_1,   // D23
  PD_2,   // D24
  PD_3,   // D25
  PF_2,   // D26
  PA_9_R, // D27
  PA_10_R // D28
};

// Analog (Ax) pin number array
const uint32_t analogInputPin[] = {
  0,  // A0,  PA0
  1,  // A1,  PA1
  2,  // A2,  PA2
  3,  // A3,  PA3
  4,  // A4,  PA4
  5,  // A5,  PA5
  6,  // A6,  PA6
  7,  // A7,  PA7
  13  // A8,  PB0
};

#endif /* ARDUINO_GENERIC_* */
