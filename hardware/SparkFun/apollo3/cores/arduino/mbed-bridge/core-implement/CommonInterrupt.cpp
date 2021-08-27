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

void indexAttachInterruptParam(pin_size_t index, voidFuncPtrParam callback, PinStatus mode, void* param){
    indexDetachInterrupt(index);
    arduino::InterruptInParam* irq = pinIRQByIndex(index);
    if(!irq){
        irq = new arduino::InterruptInParam(pinNameByIndex(index));
    }
    pinIRQByIndex(index) = irq;
    switch (mode) {
        case CHANGE :
            irq->rise(mbed::callback(callback), param);
            irq->fall(mbed::callback(callback), param);
            break;
        case FALLING :
            irq->fall(mbed::callback(callback), param);
            break;
        case RISING :
        default :
            irq->rise(mbed::callback(callback), param);
            break;
    }
    if(!pinGPIOByIndex(index)){ // Give a default pullup for the pin, since calling InterruptIn with PinMode is impossible
        switch (mode) {
            case FALLING :
                indexPinMode(index, INPUT_PULLUP);
                break;
            case RISING :
                indexPinMode(index, INPUT_PULLDOWN);
                break;
            case CHANGE :
            default:
                indexPinMode(index, INPUT);
                break;
        }
    }
}

void attachInterruptParam(pin_size_t pinNumber, voidFuncPtrParam callback, PinStatus mode, void* param){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return; }
    indexAttachInterruptParam(index, callback, mode, param);
}

void attachInterruptParam(PinName pinName, voidFuncPtrParam callback, PinStatus mode, void* param){
    pin_size_t index = pinIndexByName(pinName);
    if( index == variantPinCount ){ return; }
    indexAttachInterruptParam(index, callback, mode, param);
}

void indexAttachInterrupt(pin_size_t index, voidFuncPtr callback, PinStatus mode){
    indexAttachInterruptParam(index, (voidFuncPtrParam)callback, mode, NULL);
}

void attachInterrupt(pin_size_t pinNumber, voidFuncPtr callback, PinStatus mode){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return; }
    indexAttachInterrupt(index, callback, mode);
}

void attachInterrupt(PinName pinName, voidFuncPtr callback, PinStatus mode){
    pin_size_t index = pinIndexByName(pinName);
    if( index == variantPinCount ){ return; }
    indexAttachInterrupt(index, callback, mode);
}

void indexDetachInterrupt(pin_size_t index){
    arduino::InterruptInParam* irq = pinIRQByIndex(index);
    if(!irq){ return; }
    irq->rise(NULL, NULL);
    irq->fall(NULL, NULL);
}

void detachInterrupt(pin_size_t pinNumber){
    pin_size_t index = pinIndexByNumber(pinNumber);
    if( index == variantPinCount ){ return; }
    indexDetachInterrupt(index);
}

void detachInterrupt(PinName pinName){
    pin_size_t index = pinIndexByName(pinName);
    if( index == variantPinCount ){ return; }
    indexDetachInterrupt(index);
}

//
// InterruptInParam implementation

// Note: This single-parameter constructor exists to maintain binary
//       compatibility.
//       If not for that, we could simplify by having only the 2-param
//       constructor, with a default value for the PinMode.
InterruptInParam::InterruptInParam(PinName pin) : 
    InterruptIn(pin)
{
    irq_init(pin);
}

InterruptInParam::InterruptInParam(PinName pin, PinMode mode) :
    InterruptIn(pin, mode)
{
    irq_init(pin);
}

InterruptInParam::~InterruptInParam()
{

}

void InterruptInParam::irq_init(PinName pin)
{
    gpio_irq_init(&gpio_irq, pin, (&InterruptInParam::_irq_handler), (uint32_t)this);
}

void InterruptInParam::rise(mbed::Callback<void(void*)> func, void* param)
{
    core_util_critical_section_enter();
    if (func) {
        _rise = func;
        gpio_irq_set(&gpio_irq, IRQ_RISE, 1);
    } else {
        _rise = nullptr;
        gpio_irq_set(&gpio_irq, IRQ_RISE, 0);
    }
    if (param) {
        _rise_param = param;
    } else {
        _rise_param = nullptr;
    }
    core_util_critical_section_exit();
}

void InterruptInParam::rise(mbed::Callback<void()> func){
    error("InterruptInParam.rise called with 'void(void)' callback (should be 'void(void*)')\r\n");
}

void InterruptInParam::fall(mbed::Callback<void(void*)> func, void* param)
{
    core_util_critical_section_enter();
    if (func) {
        _fall = func;
        gpio_irq_set(&gpio_irq, IRQ_FALL, 1);
    } else {
        _fall = nullptr;
        gpio_irq_set(&gpio_irq, IRQ_FALL, 0);
    }
    if (param) {
        _fall_param = param;
    } else {
        _fall_param = nullptr;
    }
    core_util_critical_section_exit();
}

void InterruptInParam::fall(mbed::Callback<void()> func){
    error("InterruptInParam.fall called with 'void(void)' callback (should be 'void(void*)')\r\n");
}

void InterruptInParam::_irq_handler(uint32_t id, gpio_irq_event event)
{
    InterruptInParam *handler = (InterruptInParam *)id;
    switch (event) {
        case IRQ_RISE:
            if (handler->_rise) {
                handler->_rise(handler->_rise_param);
            }
            break;
        case IRQ_FALL:
            if (handler->_fall) {
                handler->_fall(handler->_fall_param);
            }
            break;
        case IRQ_NONE:
            break;
    }
}
