/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifndef _ARDUINO_MBED_BRIDGE_CORE_EXTEND_HARDWARESERIAL_H_
#define _ARDUINO_MBED_BRIDGE_CORE_EXTEND_HARDWARESERIAL_H_

#include "bridge/pins.h"
#include "core-api/api/RingBuffer.h"

#define RX_BUF_LEN (256)

class UART : public HardwareSerial, public mbed::UnbufferedSerial {
private:
    RingBufferN<RX_BUF_LEN> _rxbuf;

protected:
public:
    UART(PinName tx, PinName rx, PinName rts = NC, PinName cts = NC);
	UART(pin_size_t tx, pin_size_t rx, pin_size_t rts = variantPinCount, pin_size_t cts = variantPinCount);
    UART( void );
	~UART( void );

    void rxISR( void );

    void begin(unsigned long baudrate, uint16_t config);
    void begin(unsigned long baudrate);
    void end( void );
    int available(void);
    int peek(void);
    int read(void);
    void flush(void);
    size_t write(uint8_t c);
    size_t write(const uint8_t* buffer, size_t size);
    using Print::write;
    int printf(const char *format, ...);
    operator bool(){
        return true;
    }
};

extern UART Serial;

#endif // _ARDUINO_MBED_BRIDGE_CORE_EXTEND_HARDWARESERIAL_H_
