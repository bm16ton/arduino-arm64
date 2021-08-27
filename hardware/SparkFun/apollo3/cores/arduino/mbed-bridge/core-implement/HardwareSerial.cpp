/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "core-extend/HardwareSerial.h"

UART Serial;

// redirect stdout / stdin to Serial
mbed::FileHandle *mbed::mbed_override_console(int)
{
    return (mbed::FileHandle*)&Serial;
}

UART::UART(PinName tx, PinName rx, PinName rts, PinName cts) : 
    UnbufferedSerial(tx, rx)
{
#ifdef DEVICE_SERIAL_FC
    mbed::SerialBase::Flow control = mbed::SerialBase::Disabled;
    PinName flow1 = rts;
    pinName flow2 = cts;
    bool has_rts = (rts != NC);
    bool has_cts = (cts != NC);
    if(has_rts && has_cts){
        control = mbed::SerialBase::RTSCTS;
    }else{
        if(has_rts){
            control = mbed::SerialBase::RTS;
            flow1 = rts;
        }
        if(has_cts){
            control = mbed::SerialBase::CTS;
            flow1 = cts;
        }
    }
    BufferedSerial::set_flow_control(control, flow1, flow2);
#endif // DEVICE_SERIAL_FC
}

UART::UART(pin_size_t tx, pin_size_t rx, pin_size_t rts, pin_size_t cts) : 
    UART(pinNameByNumber(tx), pinNameByNumber(rx), pinNameByNumber(rts), pinNameByNumber(cts))
{

}

UART::UART( void ) : 
    UART(STDIO_UART_TX, STDIO_UART_RX)
{

}

UART::~UART( void ){

}

void UART::rxISR( void ){
    char c;
    while(UnbufferedSerial::readable()) {
        UnbufferedSerial::read(&c, 1);
        _rxbuf.store_char(c);
    }
}

void UART::begin(unsigned long baudrate, uint16_t config){
    mbed::SerialBase::Parity parity;
    int stop_bits;
    int bits;

    switch(config & SERIAL_PARITY_MASK){
        case SERIAL_PARITY_EVEN : parity = mbed::SerialBase::Even; break;
        case SERIAL_PARITY_ODD : parity = mbed::SerialBase::Odd; break;
        case SERIAL_PARITY_MARK : parity = mbed::SerialBase::Forced1; break;
        case SERIAL_PARITY_SPACE : parity = mbed::SerialBase::Forced0; break;
        case SERIAL_PARITY_NONE :
        default :
            parity = mbed::SerialBase::None;
            break;
    }

    switch(config & SERIAL_STOP_BIT_MASK){
        case SERIAL_STOP_BIT_1_5 :
        case SERIAL_STOP_BIT_2 :
            stop_bits = 2;
            break;
        case SERIAL_STOP_BIT_1 :
        default :
            stop_bits = 1;
            break;
    }

    switch(config & SERIAL_DATA_MASK){
        case SERIAL_DATA_5 : bits = 5; break;
        case SERIAL_DATA_6 : bits = 6; break;
        case SERIAL_DATA_7 : bits = 7; break;
        case SERIAL_DATA_8 :
        default :
            bits = 8;
            break;
    }

    // disable that pesky FIFO
    AM_CRITICAL_BEGIN
    UARTn(0)->LCRH_b.FEN = 0;
    UARTn(1)->LCRH_b.FEN = 0;
    AM_CRITICAL_END

    mbed::UnbufferedSerial::set_blocking (false);
    mbed::UnbufferedSerial::baud((int)baudrate);
    mbed::UnbufferedSerial::format(bits, parity, stop_bits);
    mbed::UnbufferedSerial::attach(mbed::callback(this, &UART::rxISR), mbed::UnbufferedSerial::RxIrq);
}

void UART::begin(unsigned long baudrate){
    begin(baudrate, SERIAL_8N1);
}

void UART::end( void ){
    
}

int UART::available(void){
    return _rxbuf.available();
}

int UART::peek(void){
    return _rxbuf.peek();
}

int UART::read(void){
    return _rxbuf.read_char();
}

void UART::flush(void){

}

size_t UART::write(uint8_t c){
    return write(&c, 1);
}

size_t UART::write(const uint8_t* buffer, size_t size){
    while (!UnbufferedSerial::writeable()){};
    int result = UnbufferedSerial::write((void*)buffer, size);
    return (result < 0) ? 0 : result;
}

int UART::printf(const char *format, ...){

    va_list args;
    va_start(args, format);
    const int space = vsnprintf(NULL, 0, format, args) + 1;
    char buf[space];
    memset(buf, 0x00, space);
    vsnprintf(buf, space, format, args);
    va_end(args);

    int size = strlen(buf);
    write(buf, size);
    return size;
}
