/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "bridge/pins.h"

pin_size_t pinIndexByName(PinName name){
    pin_size_t index = 0;
    while(index < variantPinCount){
        if(variantPinStates[index].name == name){ return index; }
        index++;
    }
    return variantPinCount;
}

pin_size_t pinIndexByNumber(pin_size_t number){
    pin_size_t index = 0;
    while(index < variantPinCount){
        if(variantPinStates[index].number == number){ return index; }
        index++;
    }
    return variantPinCount;
}

pin_size_t pinNumberByIndex(pin_size_t index){
    if(index >= variantPinCount){ return (pin_size_t)NC; }
    return variantPinStates[index].number;
}

pin_size_t pinNumberByName(PinName name){
    pin_size_t index = pinIndexByName(name);
    return pinNumberByIndex(index);
}

PinName pinNameByIndex(pin_size_t index){
    if(index >= variantPinCount){ return NC; }
    return variantPinStates[index].name;
}

PinName pinNameByNumber(pin_size_t number){
    pin_size_t index = pinIndexByNumber(number);
    return pinNameByIndex(index);
}
