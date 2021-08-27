/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "mbed.h"

#define PinMode Arduino_PinMode
#include "core-api/api/Common.h"
#undef PinMode

// WMath prototypes
long random(long upper){
    if(upper == 0){ return 0; }
    return random(0, upper);
}

long random(long lower, long upper){
    long diff = upper - lower;
    return rand() % diff + lower;
}

void randomSeed(unsigned long seed){
    if ( seed != 0 ){
        srand( seed ) ;
    }
}
