#ifndef SPI_h
#define SPI_h

#include "Arduino.h"

#define SPI_MODE0		(SPI_CLOCK_MODE0)
#define SPI_MODE1		(SPI_CLOCK_MODE1)
#define SPI_MODE2		(SPI_CLOCK_MODE2)
#define SPI_MODE3		(SPI_CLOCK_MODE3)

class SPISettings {
public:
	friend class SPIClass;
	// Default speed set to 12.5MHz, SPI mode set to MODE 0 and Bit order set to MSB first
	SPISettings(uint32_t speedMaximum = 12500000, byte bitOrder = MSBFIRST, SPI_CLOCK_MODE_T dataMode  = SPI_MODE0) :
		bitRate(speedMaximum),
		dataOrder((bitOrder == MSBFIRST)? SPI_DATA_MSB_FIRST : SPI_DATA_LSB_FIRST),
		clockMode(dataMode) {}
private:
	uint32_t bitRate;
	SPI_DATA_ORDER_T dataOrder;
	SPI_CLOCK_MODE_T clockMode;
};

class SPIClass {
public:
	static void begin();
	static void end();
	static void beginTransaction(SPISettings settings);
	static void endTransaction();
	static byte transfer(byte data);
	static void transfer(void *buf, size_t length);
};

extern SPIClass SPI;

#endif
