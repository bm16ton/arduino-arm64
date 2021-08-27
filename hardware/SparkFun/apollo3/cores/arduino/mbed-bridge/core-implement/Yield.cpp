/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "core-extend/Yield.h"

#include "mbed.h"

void yield( void ){
  rtos::ThisThread::yield();
}
