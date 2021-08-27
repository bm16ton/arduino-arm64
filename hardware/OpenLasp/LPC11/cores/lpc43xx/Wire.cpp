#include "Wire.h"

#ifdef __cplusplus
extern "C" {
#endif
STATIC void i2c_state_handling(I2C_ID_T id)
{
	if (Chip_I2C_IsMasterActive(id)) {
		Chip_I2C_MasterStateHandler(id);
	} else {
		Chip_I2C_SlaveStateHandler(id);
	}
}

void I2C0_IRQHandler(void)
{
	i2c_state_handling(I2C0);
}

void I2C1_IRQHandler(void)
{
	i2c_state_handling(I2C1);
}

void I2C2_IRQHandler(void)
{
	i2c_state_handling(I2C2);
}
#ifdef __cplusplus
}
#endif

TwoWire::TwoWire(I2C_ID_T id, const GPIO_CFG_T *sda, const GPIO_CFG_T *scl, uint32_t speed, IOCON_I2CPINS_CONFIG i2c0_mode)
{
	i2cId = id;
	i2cSpeed = speed;
	/* Setup I2C interface */
	if (sda) {
		gpioMapper.add(sda);
		/* I2C1/2 pins are not fully I2C-bus compliant open-drain pins but can
		   be configured to be open-drain via the PINMODE and PINMODE_OD registers */
		if (id != I2C0)
			Chip_IOCON_EnableOD(LPC_IOCON, sda->port, sda->bit);
	}
	if (scl) {
		gpioMapper.add(scl);
		if (id != I2C0)
			Chip_IOCON_EnableOD(LPC_IOCON, scl->port, scl->bit);
	}
	if (id == I2C0) {
		/* SDA0 and SCL0 are open-drain outputs and fully I2C-bus compliant.
		   I2C0 can be further configured through the I2CPADCFG register */
		Chip_IOCON_SetI2CPad(LPC_IOCON, i2c0_mode);
	}
}

void TwoWire::begin()
{
	/* Initialize I2C */
	Chip_I2C_Init(i2cId);
	Chip_I2C_SetClockRate(i2cId, i2cSpeed);
	/* Enable I2C interrupt */
	Chip_I2C_SetMasterEventHandler(i2cId, Chip_I2C_EventHandler);
	NVIC_EnableIRQ((i2cId == I2C0)? I2C0_IRQn : ((i2cId == I2C1)? I2C1_IRQn : I2C2_IRQn));
}

void TwoWire::beginTransmission(byte address)
{
	i2cXfer.slaveAddr = address;
	i2cXfer.txBuff = txBuff;
	i2cXfer.rxBuff = rxBuff;
	i2cXfer.txSz = i2cXfer.rxSz = 0;
}

size_t TwoWire::write(byte value)
{
	size_t bytes_written = 0;
	if (i2cXfer.txSz < BUFFER_LENGTH) {
		txBuff[i2cXfer.txSz++] = value;
		bytes_written++;
	}
	return bytes_written;
}

size_t TwoWire::write(const byte* data, size_t length)
{
	size_t bytes_written;
	for (bytes_written = 0; length > 0 && i2cXfer.txSz < BUFFER_LENGTH; length--) {
		txBuff[i2cXfer.txSz++] = *data++;
		bytes_written++;
	}
	return bytes_written;
}

void TwoWire::flush(void)
{
}

byte TwoWire::endTransmission(bool stop)
{
	switch (Chip_I2C_MasterTransfer(i2cId, &i2cXfer)) {
	case I2C_STATUS_DONE:
		return 0;
	case I2C_STATUS_NAK:
		return 3;
	default:
		return 4;
	}
}

byte TwoWire::requestFrom(byte address, byte quantity, bool stop)
{
	rxBufIndex = 0;
	i2cXfer.slaveAddr = address;
	i2cXfer.rxSz = quantity;
	Chip_I2C_MasterTransfer(i2cId, &i2cXfer);
	return (i2cXfer.rxSz = quantity - i2cXfer.rxSz);
}

int TwoWire::available(void)
{
	return i2cXfer.rxSz;
}

int TwoWire::read(void)
{
	int byte = EOF;
	if (i2cXfer.rxSz > 0) {
		byte = rxBuff[rxBufIndex++];
		i2cXfer.rxSz--;
	}
	return byte;
}

int TwoWire::peek(void)
{
	return (i2cXfer.rxSz > 0)? rxBuff[rxBufIndex] : EOF;
}
