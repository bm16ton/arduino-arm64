#include <string.h>
#include "SPI.h"

SPIClass SPI;

void SPIClass::begin()
{
	Chip_SPI_Init(LPC_SPI);
}

void SPIClass::end()
{
	Chip_SPI_DeInit(LPC_SPI);
}

void SPIClass::beginTransaction(SPISettings settings)
{
	SPI_CONFIG_FORMAT_T config;
	config.bits = SPI_BITS_8;
	config.clockMode = settings.clockMode;
	config.dataOrder = settings.dataOrder;
	Chip_SPI_SetFormat(LPC_SPI, &config);
	Chip_SPI_SetMode(LPC_SPI, SPI_MODE_MASTER);
	Chip_SPI_SetBitRate(LPC_SPI, settings.bitRate);
}

void SPIClass::endTransaction()
{
}

byte SPIClass::transfer(byte data)
{
	uint8_t buf[2] = { data, 0 };
	SPI_DATA_SETUP_T xfrSetup;
	memset(&xfrSetup, 0, sizeof(SPI_DATA_SETUP_T));
	xfrSetup.cnt = 0;
	xfrSetup.length = sizeof(byte);
	xfrSetup.pTxData = buf;
	xfrSetup.pRxData = buf + 1;
	Chip_SPI_RWFrames_Blocking(LPC_SPI, &xfrSetup);
	return buf[1];
}

void SPIClass::transfer(void *buf, size_t length)
{
	uint8_t* rxBuf = (uint8_t*)malloc(length);
	if (rxBuf) {
		SPI_DATA_SETUP_T xfrSetup;
		memset(&xfrSetup, 0, sizeof(SPI_DATA_SETUP_T));
		xfrSetup.cnt = 0;
		xfrSetup.length = length;
		xfrSetup.pTxData = (uint8_t*)buf;
		xfrSetup.pRxData = rxBuf;
		if (Chip_SPI_RWFrames_Blocking(LPC_SPI, &xfrSetup) == length)
			memcpy(buf, rxBuf, xfrSetup.cnt);
		free(rxBuf);
	}
}
