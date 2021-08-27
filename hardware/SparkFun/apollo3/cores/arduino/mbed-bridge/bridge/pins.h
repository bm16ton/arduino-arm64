/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _ARDUINO_MBED_BRIDGE_BRIDGE_PINS_H_
#define _ARDUINO_MBED_BRIDGE_BRIDGE_PINS_H_

#include "Arduino.h"

typedef struct _PinState {
    PinName name;
    pin_size_t number;
    arduino::InterruptInParam* irq;
    // mbed::PwmOut* pwm; // todo: implement this
    // mbed::AnalogOut* dac; // todo: implement this
    // mbed::AnalogIn* adc; // todo: implement this
    mbed::DigitalInOut* gpio;
} PinState;

pin_size_t pinIndexByName(PinName name);
pin_size_t pinIndexByNumber(pin_size_t number);

pin_size_t pinNumberByIndex(pin_size_t index);
pin_size_t pinNumberByName(PinName name);

PinName pinNameByIndex(pin_size_t index);
PinName pinNameByNumber(pin_size_t number);

#define pinIRQByIndex(I) variantPinStates[I].irq
#define pinPWMByIndex(I) variantPinStates[I].pwm
#define pinDACByIndex(I) variantPinStates[I].dac
#define pinADCByIndex(I) variantPinStates[I].adc
#define pinGPIOByIndex(I) variantPinStates[I].gpio

extern const pin_size_t variantPinCount;
extern PinState variantPinStates[];

#endif // _ARDUINO_MBED_BRIDGE_BRIDGE_PINS_H_