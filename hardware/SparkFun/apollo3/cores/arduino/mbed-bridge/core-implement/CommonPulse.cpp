/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "mbed.h"

#define PinMode Arduino_PinMode
#include "core-api/api/Common.h"
#undef PinMode

#include "core-extend/Common.h"
#include "bridge/pins.h"

// unsigned long indexPulseIn(pin_size_t index, uint8_t state, unsigned long timeout){
//     standInFunc();
//     return 0;
// }

unsigned long pulseIn(PinName pinName, uint8_t state, unsigned long timeout){
    pin_size_t index = pinIndexByName(pinName);
    if( index == variantPinCount ){ return 0; }
    return indexPulseIn(index, state, timeout);
}

unsigned long pulseIn(pin_size_t pinNumber, uint8_t state, unsigned long timeout){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return 0; }
    return indexPulseIn(index, state, timeout);
}

unsigned long indexPulseInLong(pin_size_t index, uint8_t state, unsigned long timeout){
    return indexPulseIn(index, state, timeout); // pulseIn and pulseInLong are identical
}

unsigned long pulseInLong(PinName pinName, uint8_t state, unsigned long timeout){
    pin_size_t index = pinIndexByName(pinName);
    if( index == variantPinCount ){ return 0; }
    return pulseInLong(index, state, timeout);
}

unsigned long pulseInLong(pin_size_t pinNumber, uint8_t state, unsigned long timeout){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return 0; }
    return pulseInLong(index, state, timeout);
}
