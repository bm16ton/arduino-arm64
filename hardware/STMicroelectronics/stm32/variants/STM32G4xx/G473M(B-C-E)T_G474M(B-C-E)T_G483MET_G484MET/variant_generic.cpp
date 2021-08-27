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
#if defined(ARDUINO_GENERIC_G473MBTX) || defined(ARDUINO_GENERIC_G473MCTX) ||\
    defined(ARDUINO_GENERIC_G473METX) || defined(ARDUINO_GENERIC_G474MBTX) ||\
    defined(ARDUINO_GENERIC_G474MCTX) || defined(ARDUINO_GENERIC_G474METX) ||\
    defined(ARDUINO_GENERIC_G483METX) || defined(ARDUINO_GENERIC_G484METX)
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
  PA_8,   // D9/A8
  PA_9,   // D10/A9
  PA_10,  // D11
  PA_11,  // D12
  PA_12,  // D13
  PA_13,  // D14
  PA_14,  // D15
  PA_15,  // D16
  PB_0,   // D17/A10
  PB_1,   // D18/A11
  PB_2,   // D19/A12
  PB_3,   // D20
  PB_4,   // D21
  PB_5,   // D22
  PB_6,   // D23
  PB_7,   // D24
  PB_8,   // D25
  PB_9,   // D26
  PB_10,  // D27
  PB_11,  // D28/A13
  PB_12,  // D29/A14
  PB_13,  // D30/A15
  PB_14,  // D31/A16
  PB_15,  // D32/A17
  PC_0,   // D33/A18
  PC_1,   // D34/A19
  PC_2,   // D35/A20
  PC_3,   // D36/A21
  PC_4,   // D37/A22
  PC_5,   // D38/A23
  PC_6,   // D39
  PC_7,   // D40
  PC_8,   // D41
  PC_9,   // D42
  PC_10,  // D43
  PC_11,  // D44
  PC_12,  // D45
  PC_13,  // D46
  PC_14,  // D47
  PC_15,  // D48
  PD_0,   // D49
  PD_1,   // D50
  PD_2,   // D51
  PD_8,   // D52/A24
  PD_9,   // D53/A25
  PD_10,  // D54/A26
  PE_7,   // D55/A27
  PE_8,   // D56/A28
  PE_9,   // D57/A29
  PE_10,  // D58/A30
  PE_11,  // D59/A31
  PE_12,  // D60/A32
  PE_13,  // D61/A33
  PE_14,  // D62/A34
  PE_15,  // D63/A35
  PF_0,   // D64/A36
  PF_1,   // D65/A37
  PG_10   // D66
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
  8,  // A8,  PA8
  9,  // A9,  PA9
  16, // A10, PB0
  17, // A11, PB1
  18, // A12, PB2
  27, // A13, PB11
  28, // A14, PB12
  29, // A15, PB13
  30, // A16, PB14
  31, // A17, PB15
  32, // A18, PC0
  33, // A19, PC1
  34, // A20, PC2
  35, // A21, PC3
  36, // A22, PC4
  37, // A23, PC5
  51, // A24, PD8
  52, // A25, PD9
  53, // A26, PD10
  54, // A27, PE7
  55, // A28, PE8
  56, // A29, PE9
  57, // A30, PE10
  58, // A31, PE11
  59, // A32, PE12
  60, // A33, PE13
  61, // A34, PE14
  62, // A35, PE15
  63, // A36, PF0
  64  // A37, PF1
};

#endif /* ARDUINO_GENERIC_* */
