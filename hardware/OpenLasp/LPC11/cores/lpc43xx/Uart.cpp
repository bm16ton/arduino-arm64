#include "Arduino.h"

Uart Serial0(LPC_UART0, UART0_IRQn);
Uart Serial1(LPC_UART1, UART1_IRQn);
Uart Serial2(LPC_UART2, UART2_IRQn);
Uart Serial3(LPC_UART3, UART3_IRQn);

void UART_IRQHandler(Uart* p)
{
	Chip_UART_RXIntHandlerRB(p->pUart, &p->rxRingBuf);
}

#ifdef __cplusplus
extern "C" {
#endif
void UART0_IRQHandler(void)
{
	UART_IRQHandler(&Serial0);
}

void UART1_IRQHandler(void)
{
	UART_IRQHandler(&Serial1);
}

void UART2_IRQHandler(void)
{
	UART_IRQHandler(&Serial2);
}

void UART3_IRQHandler(void)
{
	UART_IRQHandler(&Serial3);
}
#ifdef __cplusplus
}
#endif

Uart::Uart(LPC_USART_T *p, IRQn_Type irq)
{
	Chip_UART_Init(pUart = p);
	irqNo = irq;
	RingBuffer_Init(&rxRingBuf, rxData, sizeof(rxData[0]), sizeof(rxData));
}

void Uart::begin(unsigned baudrate, unsigned config)
{
	Chip_UART_SetBaud(pUart, baudrate);
	Chip_UART_ConfigData(pUart, config);
	Chip_UART_SetupFIFOS(pUart, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(pUart);
	Chip_UART_IntEnable(pUart, (UART_IER_RBRINT | UART_IER_RLSINT));
	NVIC_EnableIRQ(irqNo);
}

void Uart::end()
{
	NVIC_DisableIRQ(irqNo);
	Chip_UART_DeInit(pUart);
}

int Uart::available()
{
	return RingBuffer_GetCount(&rxRingBuf);
}

int Uart::peek()
{
	return available()? rxData[rxRingBuf.tail & (sizeof(rxData)-1)] : EOF;
}

int Uart::read()
{
	uint8_t data;
	return RingBuffer_Pop(&rxRingBuf, &data)? data : EOF;
}

size_t Uart::write(const uint8_t data)
{
	Chip_UART_SendBlocking(pUart, &data, sizeof(data));
	return 1;
}

void Uart::flush()
{
	RingBuffer_Flush(&rxRingBuf);
}
