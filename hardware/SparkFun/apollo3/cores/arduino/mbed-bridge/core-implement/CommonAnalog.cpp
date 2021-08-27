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

#define standInFunc() printf("Stand-In for '%s' [file: %s, line: %d]\n", __FUNCTION__, __FILE__, __LINE__)

// int indexAnalogRead(pin_size_t index){
//     // todo: support mbed AnalogIn modules
//     mbed::AnalogIn* adc = pinADCByIndex(index);
//     if (adc == NULL) {
//         adc = new mbed::AnalogIn(name);
//         pinADCByIndex(index) = adc;
// #ifdef ANALOG_CONFIG
//         if (isAdcConfigChanged) {
//             adc->configure(adcCurrentConfig);
//         }
// #endif
//     }
//     return (adc->read_u16() >> (16 - read_resolution));
// }

int analogRead(PinName pinName){
    pin_size_t index = pinIndexByName(pinName);
    if( index == variantPinCount ){ return 0; }
    return indexAnalogRead(index);
}

int analogRead(pin_size_t pinNumber){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return 0; }
    return indexAnalogRead(index);
}

// static int res_analog_w = 8;
// static int res_analog_r = 10;
void indexAnalogWriteDAC(pin_size_t index, int val){
    standInFunc();
    // // todo: support mbed DAC modules
    // mbed::AnalogOut* dac = pinDACByIndex(index);
    // if (dac == NULL) {
    //     dac = new mbed::AnalogOut(pinNameByIndex(index));
    //     pinDACByIndex(index) = dac;
    // }
    // float percent = (float)val/(float)(1 << res_analog_w);
    // dac->write(percent);
}

void analogWriteDAC(PinName pinName, int val){
    pin_size_t index = pinIndexByName(pinName);
    if( index == variantPinCount ){ return; }
    indexAnalogWriteDAC(index, val);
}

void analogWriteDAC(pin_size_t pinNumber, int val){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return; }
    indexAnalogWriteDAC(index, val);
}

// void indexAnalogWrite(pin_size_t index, int val){
//     standInFunc();
//     // mbed::PwmOut* pwm = pinPWMByIndex(index);
//     // if (pwm == NULL) {
//     //     pwm = new mbed::PwmOut(pinNameByIndex(index));
//     //     pinPWMByIndex(index) = pwm;
//     // }
//     // pwm->period_ms(2);
//     // float percent = (float)val/(float)(1 << res_analog_w);
//     // pwm->write(percent);
// }

void analogWrite(PinName pinName, int val){
    pin_size_t index = pinIndexByName(pinName);
    if( index == variantPinCount ){ return; }
    indexAnalogWrite(index, val);
}

void analogWrite(pin_size_t pinNumber, int val){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return; }
    indexAnalogWrite(index, val);
}

// void indexTone(pin_size_t index, unsigned int frequency, usigned long duration){
//     // // todo: implement with mbed
//     // mbed::PwmOut* pwm = pinPWMByIndex(index);
//     // if (pwm == NULL) {
//     //     pwm = new mbed::PwmOut(pinNameByIndex(index));
//     //     pinPWMByIndex(index) = pwm;
//     // }
//     // pwm->period_ms(2);
//     // float percent = (float)val/(float)(1 << res_analog_w);
//     // pwm->write(percent);
// }

void tone(uint8_t pinNumber, unsigned int frequency, unsigned long duration){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return; }
    indexTone(index, frequency, duration);
}

void noTone(uint8_t pinNumber){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return; }
    indexTone(index, 0, 0);
}