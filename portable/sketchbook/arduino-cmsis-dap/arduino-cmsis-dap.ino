/* CMSIS-DAP ported to run on the Pro Micro and Teensy 3.2
 * Copyright (C) 2016 Phillip Pearson <pp@myelin.co.nz>
 *
 * CMSIS-DAP Interface Firmware
 * Copyright (c) 2009-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *******************************************************************
 *
 * INSTALLATION INSTRUCTIONS
 *
 * DEPENDENCIES
 *
 * To build this code, you'll need the Arduino toolchain, plus the
 * appropriate package for your board.  You'll also need to install
 * NicoHood's HID library: https://github.com/NicoHood/HID
 *
 * If you get an error about HID-Project.h, double check that you've
 * installed the HID library properly.
 *
 * BUILD SETUP
 *
 * For a device to be correctly detected as a CMSIS-DAP adapter, it must
 * contain the string "CMSIS-DAP" in its USB product name.  Unfortunately
 * we can't override this from within the sketch -- you'll have to edit
 * files in your Arduino installation to change it.
 *
 * Arduino Micro:
 *   Inside your Arduino installation, edit hardware/arduino/avr/boards.txt
 *   and add CMSIS-DAP to micro.build.usb_product:
 *
 *   micro.build.usb_product="Arduino Micro CMSIS-DAP"
 *
 * Sparkfun Pro Micro:
 *   Inside your Arduino installation, edit ~/.arduino15/packages/SparkFun/hardware/avr/1.1.12/boards.txt
 *   and add CMSIS-DAP to micro.build.usb_product:
 *
 *   promicro.build.usb_product="SparkFun Pro Micro CMSIS-DAP"
 *
 * Teensy 3.2:
 *   Inside your Arduino installation, edit hardware/teensy/avr/cores/teensy3/usb_desc.h
 *   and change some of the text in the line starting #define PRODUCT_NAME
 *   to include CMSIS-DAP:
 *
 *   #define PRODUCT_NAME   {'T','e','e','n','s','y',' ','-',' ','C','M','S','I','S','-','D','A','P'}
 *
 *   You also need to select "Raw HID" under Tools | USB Type, after
 *   selecting the Teensy 3.2 board.
 *
 * WIRING
 *
 * Connect up your SWD or JTAG target to the appropriate pins for your board:
 *
 * Pro Micro pins:
 *   2 SWDIO
 *   3 SWCLK
 *   4 TDO
 *   5 TDI
 *   6 /RESET
 *
 * Teensy 3.2 pins:
 *  17 SWDIO   changed from 19 i2c sniffer needs 19
 *  20 SWCLK
 *  21 TDO
 *  22 TDI
 *  23 /RESET
 *
 * Then give it a try!  With OpenOCD, the command line will look something like this:
 *
 *   openocd -f interface/cmsis-dap.cfg -f target/kl25.cfg
 *
 * If it doesn't work, try adding '-d' to get more debug information.
 *
 */

#include "DAP_config.h"
#include "DAP.h"

uint8_t rawhidRequest[DAP_PACKET_SIZE];
uint8_t rawhidResponse[DAP_PACKET_SIZE];


void setup() {
  Serial.begin(115200);

  DAP_Setup();

#ifdef HIDPROJECT_RAWHID
  // Set the RawHID OUT report array.
  // Feature reports are also (parallel) possible, see the other example for this.
  RawHID.begin(rawhidRequest, DAP_PACKET_SIZE);
#endif
}

void loop() {
  // Check if there is new data from the RawHID device
  auto bytesAvailable =
#ifdef HIDPROJECT_RAWHID
    RawHID.available();
#else
    RawHID.recv(rawhidRequest, 0);
#endif
  if (bytesAvailable > 0) {
#if DAP_SERIAL_LOG
    Serial.print("cmd ");
    Serial.print(rawhidRequest[0], HEX);
    Serial.print(" ");
    Serial.print(rawhidRequest[1], HEX);
    Serial.print(" ");
#endif /* DAP_SERIAL_LOG */
    auto sz = DAP_ProcessCommand(rawhidRequest, rawhidResponse);
#if DAP_SERIAL_LOG
    Serial.print("rsp ");
    Serial.print(sz);
    Serial.println(" B");
#endif /* DAP_SERIAL_LOG */
#ifdef HIDPROJECT_RAWHID
    RawHID.enable(); // signal that we're ready to receive another buffer
#endif
    if (sz > 0) {
#ifdef HIDPROJECT_RAWHID
      RawHID.write(rawhidResponse, DAP_PACKET_SIZE);
#else
      RawHID.send(rawhidResponse, DAP_PACKET_SIZE);
#endif
    }
  }
}
