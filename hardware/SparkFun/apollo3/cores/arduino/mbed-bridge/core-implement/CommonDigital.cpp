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

void indexPinMode(pin_size_t index, Arduino_PinMode pinMode){
    mbed::DigitalInOut* gpio = pinGPIOByIndex(index);
    if( gpio == NULL ){
        gpio = new mbed::DigitalInOut(pinNameByIndex(index));
    }
    pinGPIOByIndex(index) = gpio;

    switch (pinMode) {
        case INPUT:
            gpio->input();
            gpio->mode(PullNone);
            break;
        case OUTPUT:
            gpio->output();
            break;
        case INPUT_PULLUP:
            gpio->input();
            gpio->mode(PullUp);
            break;
        case INPUT_PULLDOWN:
        default:
            gpio->input();
            gpio->mode(PullDown);
            break;
    }
}

void pinMode(pin_size_t pinNumber, Arduino_PinMode pinMode){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return; }
    indexPinMode(index, pinMode);
}

void pinMode(PinName pinName, Arduino_PinMode pinMode){
    pin_size_t index = pinIndexByName(pinName);
    if( index == variantPinCount ){ return; }
    indexPinMode(index, pinMode);
}

void indexDigitalWrite(pin_size_t index, PinStatus val){
    mbed::DigitalInOut* gpio = pinGPIOByIndex(index);
    if (gpio == NULL) {
        gpio = new mbed::DigitalInOut(pinNameByIndex(index), PIN_OUTPUT, PullNone, val);
        pinGPIOByIndex(index) = gpio;
    }
    gpio->write(val);
}

void digitalWrite(pin_size_t pinNumber, PinStatus val){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return; }
    indexDigitalWrite(index, val);
}

void digitalWrite(PinName pinName, PinStatus val){
    pin_size_t index = pinIndexByName(pinName);
    if( index == variantPinCount ){ return; }
    indexDigitalWrite(index, val);
}

PinStatus indexDigitalRead(pin_size_t index){
    mbed::DigitalInOut* gpio = pinGPIOByIndex(index);
    if (gpio == NULL) {
        gpio = new mbed::DigitalInOut(pinNameByIndex(index), PIN_INPUT, PullNone, 0);
        pinGPIOByIndex(index) = gpio;
    }
    return (PinStatus) gpio->read();
}

PinStatus digitalRead(pin_size_t pinNumber){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return LOW; }
    return indexDigitalRead(index);
}

PinStatus digitalRead(PinName pinName){
    pin_size_t index = pinIndexByName(pinName);
    if( index == variantPinCount ){ return LOW; }
    return indexDigitalRead(index);
}

void indexShiftOut(pin_size_t data_index, pin_size_t clock_index, BitOrder bitOrder, uint8_t val){
	for (uint8_t idx = 0; idx < 8; idx++)  {
		if (bitOrder == LSBFIRST) {
			indexDigitalWrite(data_index, !!(val & (1 << idx)) ? HIGH : LOW);
        }else{
            indexDigitalWrite(data_index, !!(val & (1 << (7 - idx))) ? HIGH : LOW);
        }	
		indexDigitalWrite(clock_index, HIGH);
		indexDigitalWrite(clock_index, LOW);		
	}
}

void shiftOut(pin_size_t dataPinNumber, pin_size_t clockPinNumber, BitOrder bitOrder, uint8_t val){
    pin_size_t data_index = pinIndexByNumber(dataPinNumber);
    pin_size_t clock_index = pinIndexByNumber(dataPinNumber);
    if( data_index == variantPinCount ){ return; }
    if( clock_index == variantPinCount ){ return; }
    indexShiftOut(data_index, clock_index, bitOrder, val);
}

void shiftOut(PinName dataPinName, PinName clockPinName, BitOrder bitOrder, uint8_t val){
    pin_size_t data_index = pinIndexByName(dataPinName);
    pin_size_t clock_index = pinIndexByName(clockPinName);
    if( data_index == variantPinCount ){ return; }
    if( clock_index == variantPinCount ){ return; }
    indexShiftOut(data_index, clock_index, bitOrder, val);
}

pin_size_t indexShiftIn(pin_size_t data_index, pin_size_t clock_index, BitOrder bitOrder){
	uint8_t value = 0;
	for (uint8_t idx = 0; idx < 8; ++idx) {
		indexDigitalWrite(clock_index, HIGH);
		if (bitOrder == LSBFIRST){
			value |= indexDigitalRead(data_index) << idx;
        }else{
			value |= indexDigitalRead(data_index) << (7 - idx);
        }
		indexDigitalWrite(clock_index, LOW);
	}
	return value;
}

pin_size_t shiftIn(pin_size_t dataPinNumber, pin_size_t clockPinNumber, BitOrder bitOrder){
    pin_size_t data_index = pinIndexByNumber(dataPinNumber);
    pin_size_t clock_index = pinIndexByNumber(clockPinNumber);
    if( data_index == variantPinCount ){ return 0x00; }
    if( clock_index == variantPinCount ){ return 0x00; }
    return indexShiftIn(data_index, clock_index, bitOrder);
}

pin_size_t shiftIn(PinName dataPinName, PinName clockPinName, BitOrder bitOrder){
    pin_size_t data_index = pinIndexByName(dataPinName);
    pin_size_t clock_index = pinIndexByName(clockPinName);
    if( data_index == variantPinCount ){ return 0x00; }
    if( clock_index == variantPinCount ){ return 0x00; }
    return indexShiftIn(data_index, clock_index, bitOrder);
}