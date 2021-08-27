# Arduino CAN MCP2515-nb [![Build Status](https://travis-ci.org/WitchCraftWorks/arduino-MCP2515-nb.svg?branch=master)](https://travis-ci.org/WitchCraftWorks/arduino-MCP2515-nb)

This is an Arduino library for sending and receiving data using CAN bus with the MCP2515 CAN controller.

This library supports "non-blocking" write packet operations. Which means this library will not wait until the CAN controller has written the packet
onto the CAN bus. The library supports defer checking the status to a later time. However that means that no new packet can be written to the TX buffer
of the CAN controller until the library has confirmed the status of the CAN packet.

Thus there are two waits to make this efficient: Using interrupts (for both RX and TX parts) or periodic check (i.e. at the start of your `loop` function).
Recommendation is clearly using the interrupts approach as it's more efficient. If you prefer the periodic check, call `MCP.processTxQueue()` at the top of your
`loop` function. The function will also send any queued CAN packet (automatically done when using interrupts).

When using non-blocking write operations, any outgoing packet that can't be written immediately to the CAN controller, will be queued. The max queue size is defined
by the `MCP2515_CANPACKET_TX_QUEUE_SIZE` macro, which can be defined before including `MCP2515_nb.h` (defaults to `16`).

This library depends on `avr_stl` for platforms without STL distributed with the core (i.e. SAMD ships with STL). Currently `avr_stl` is conditionally included only for AVR.
Please open an issue with your platform, if your platform does not include STL, so the platform can be added.

# Getting Started

# Compatible Hardware

* [Microchip MCP2515](http://www.microchip.com/wwwproducts/en/en010406) based boards/shields
  * [Arduino MKR CAN shield](https://store.arduino.cc/arduino-mkr-can-shield)

## Microchip MCP2515 wiring

| Microchip MCP2515 | Arduino |
| :---------------: | :-----: |
| VCC | 5V |
| GND | GND |
| SCK | SCK |
| SO | MISO |
| SI | MOSI |
| CS | 10 |
| INT | 2 |


`CS` and `INT` pins can be changed by using `CAN.setPins(cs, irq)`. `INT` pin is optional, it is only needed for receive callback mode. If `INT` pin is used, it **must** be interrupt capable via [`attachInterrupt(...)`](https://www.arduino.cc/en/Reference/AttachInterrupt).

**NOTE**: Logic level converters must be used for boards which operate at 3.3V.

# Installation

Download zip from GitHub and import library as zip in Arduino IDE.

## API

See [API.md](API.md).

## Examples

See [examples](examples) folder.

## Thanks

This library is based upon the initial work of [sandeepmistry](https://github.com/sandeepmistry), so thanks!
I also want to thank every other MCP2515 library writer, as I've taken some of their work as comparison.
