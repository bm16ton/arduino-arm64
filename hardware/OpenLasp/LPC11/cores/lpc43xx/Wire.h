#ifndef TwoWire_h
#define TwoWire_h

#include "Arduino.h"
#include "Stream.h"

/* LPC17xx supports three I2C interfaces (I2C0/I2C1/I2C1) with configurable pins through
 * the pin connect block. Below are valid GPIO pin configurations for initializing the Wire
 * library used by Arduino sketch programs:
 *
 * [I2C0]
 *     P0.27(SDA0), P0.28(SCL0)
 *         static GPIO_CFG_T gpioSda = { 0, 27, IOCON_MODE_INACT | IOCON_FUNC1, 20 };
 *         static GPIO_CFG_T gpioScl = { 0, 28, IOCON_MODE_INACT | IOCON_FUNC1, 21 };
 * [I2C1]
 *     P0.0(SDA1), P0.1(SCL1)
 *         static GPIO_CFG_T gpioSda = { 0,  0, IOCON_MODE_INACT | IOCON_FUNC3, 20 };
 *         static GPIO_CFG_T gpioScl = { 0,  1, IOCON_MODE_INACT | IOCON_FUNC3, 21 };
 *     P0.19(SDA1), P0.20(SCL1)
 *         static GPIO_CFG_T gpioSda = { 0, 19, IOCON_MODE_INACT | IOCON_FUNC3, 20 };
 *         static GPIO_CFG_T gpioScl = { 0, 20, IOCON_MODE_INACT | IOCON_FUNC3, 21 };
 * [I2C2]
 *     P0.10(SDA2), P0.11(SCL2)
 *         static GPIO_CFG_T gpioSda = { 0, 10, IOCON_MODE_INACT | IOCON_FUNC2, 20 };
 *         static GPIO_CFG_T gpioScl = { 0, 11, IOCON_MODE_INACT | IOCON_FUNC2, 21 };
 *
 * TwoWire Wire({I2C0 | I2C1 | I2C2}, &gpioSda, &gpioScl);
 */

#define BUFFER_LENGTH	32
#define I2C_100KHZ		100000
#define I2C_400KHZ		400000

class TwoWire : public Stream {
public:
	TwoWire(I2C_ID_T id, const GPIO_CFG_T *sda, const GPIO_CFG_T *scl,
	        uint32_t speed = I2C_100KHZ, IOCON_I2CPINS_CONFIG i2c0_mode = I2CPADCFG_STD_MODE);
	void begin();
	void beginTransmission(byte address);
	byte endTransmission(bool stop = true);
	byte requestFrom(byte address, byte quantity, bool stop = true);
	virtual size_t write(byte value);
	virtual size_t write(const byte* data, size_t length);
	virtual int available();
	virtual int read();
	virtual int peek();
	virtual void flush();
	using Print::write;
protected:
	I2C_ID_T i2cId;
	uint32_t i2cSpeed;
	I2C_XFER_T i2cXfer;
	byte txBuff[BUFFER_LENGTH];
	byte rxBuff[BUFFER_LENGTH];
	byte rxBufIndex;
};

extern TwoWire Wire;

#endif
