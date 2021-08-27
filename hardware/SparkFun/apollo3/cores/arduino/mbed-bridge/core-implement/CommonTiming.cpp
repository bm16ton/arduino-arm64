/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "mbed.h"

#define PinMode Arduino_PinMode
#include "core-api/api/Common.h"
#undef PinMode

#include "core-extend/Common.h"

#define standInFunc() printf("Stand-In for '%s' [file: %s, line: %d]\n", __FUNCTION__, __FILE__, __LINE__)

// #if DEVICE_LPTICKER
// static mbed::LowPowerTimer t;
// #else
static mbed::Timer t;
// #endif

using namespace std::chrono_literals;
using namespace std::chrono;

void initTimer(void){
    t.start();
}

unsigned long millis(void){
    return duration_cast<milliseconds>(t.elapsed_time()).count();
}

unsigned long micros(void){
    return t.elapsed_time().count();
}

void delay(unsigned long ms){
#ifndef NO_RTOS
  rtos::ThisThread::sleep_for(ms * 1ms);
#else
  wait_us(ms * 1000);
#endif
}

void delayMicroseconds(unsigned int us){
    wait_us(us);
}
