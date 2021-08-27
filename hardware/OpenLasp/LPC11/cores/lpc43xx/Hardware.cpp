#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif
static void __empty() {
	// Empty
}
void init(void) __attribute__ ((weak, alias("__empty")));
void yield(void) __attribute__ ((weak, alias("__empty")));

static LPC_TIMER_T* timer_csr[] = { LPC_TIMER0, LPC_TIMER1, LPC_TIMER2, LPC_TIMER3 };
static volatile int toneOnPin = -1;
static volatile uint32_t toneOnInMsec;
static volatile uint32_t _ulMilliSecond = 0;

void RIT_IRQHandler(void)
{
	Chip_RIT_ClearInt(LPC_RITIMER);
	_ulMilliSecond++;
	if (toneOnPin >= 0 && --toneOnInMsec == 0)
		noTone(toneOnPin);
}

unsigned long millis(void)
{
	return _ulMilliSecond;
}

void delay(unsigned long ms)
{
	if (ms > 0) {
		uint32_t start = _ulMilliSecond;
		do {
			__WFI();
		} while (_ulMilliSecond - start < ms);
	}
}

unsigned long micros(void)
{
	return (_ulMilliSecond * 1000) + (Chip_RIT_GetCounter(LPC_RITIMER) / 100);
}

void delayMicroseconds(unsigned int us)
{
	unsigned long t = micros() + us;
	while (micros() < t);
}
#ifdef __cplusplus
}
#endif

void pinMode(uint8_t pin, uint8_t mode)
{
	int prop = gpioMapper.getProp(pin);
	if (prop >= 0 && IOFUNC(prop) == IOCON_FUNC0)
		Chip_GPIO_SetPinDIR(LPC_GPIO, IOGRP(prop), IONUM(prop), mode);
}

void digitalWrite(uint8_t pin, uint8_t val)
{
	int prop = gpioMapper.getProp(pin);
	if (prop >= 0) {
		if (!(prop & GPIO_PWM))
			Chip_GPIO_SetPinState(LPC_GPIO, IOGRP(prop), IONUM(prop), val);
		else
			gpioMapper.pwmSetDutyCycle(pin, val);
	}
}

int digitalRead(uint8_t pin)
{
	int prop = gpioMapper.getProp(pin);
	if (prop >= 0 && !(prop & GPIO_PWM))
		return Chip_GPIO_GetPinState(LPC_GPIO, IOGRP(prop), IONUM(prop));
}

void analogWrite(uint8_t pin, uint8_t val)
{
	int prop = gpioMapper.getProp(pin);
	if (prop >= 0 && (prop & GPIO_PWM))
		gpioMapper.pwmSetDutyCycle(pin, val / 255.f);
}

static ADC_CLOCK_SETUP_T ADCSetup;

int analogRead(uint8_t pin)
{
	uint16_t data = 0;
	int prop = gpioMapper.getProp(pin);
	if (prop >= 0 && (prop & GPIO_ADC)) {
		byte adc_ch = IOADC(prop);
		bool burst = ADCSetup.burstMode;
		Chip_ADC_SetBurstCmd(LPC_ADC, burst? ENABLE : DISABLE);
		/* Start A/D conversion if not using burst mode */
		if (!burst)
			Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		/* Waiting for A/D conversion complete */
		while (Chip_ADC_ReadStatus(LPC_ADC, adc_ch, ADC_DR_DONE_STAT) != SET);
		/* Read ADC value */
		Chip_ADC_ReadValue(LPC_ADC, adc_ch, &data);
		if (burst)
			Chip_ADC_SetBurstCmd(LPC_ADC, DISABLE);
	}
	return map(data, 0, 0x0fff, 0, 1023);
}

void tone(uint8_t pin, unsigned int hz, unsigned long duration_in_msec)
{
	int prop = (toneOnPin < 0 || pin == toneOnPin)? gpioMapper.getProp(pin) : -1;
	if (prop >= 0 && (prop & GPIO_CLKOUT)) {
		LPC_TIMER_T* pTMR = timer_csr[IOTIMER(prop)];
		byte match = IOMATCH(prop);
		Chip_TIMER_Reset(pTMR);
		if (hz) {
			Chip_TIMER_SetMatch(pTMR, match, ((Chip_Clock_GetSystemClockRate() >> 3) / hz));
			Chip_TIMER_ResetOnMatchEnable(pTMR, match);
		}
		Chip_TIMER_Enable(pTMR);
		toneOnPin = pin, toneOnInMsec = duration_in_msec;
	}
}

void noTone(uint8_t pin)
{
	int prop = (pin == toneOnPin)? gpioMapper.getProp(pin) : -1;
	if (prop >= 0 && (prop & GPIO_CLKOUT)) {
		LPC_TIMER_T* pTMR = timer_csr[IOTIMER(prop)];
		Chip_TIMER_Disable(pTMR);
		toneOnPin = -1;
	}
}

GPIO_CFG_T gpioCfgDefault[] = {
		{ 0,  2, IOCON_MODE_INACT | IOCON_FUNC1, GPIO_CFG_UNASSIGNED }, /* TXD0 */
		{ 0,  3, IOCON_MODE_INACT | IOCON_FUNC1, GPIO_CFG_UNASSIGNED }, /* RXD0 */
		{ 2,  0, IOCON_MODE_INACT | IOCON_FUNC0, 13 }, /* LED0 */
		{ 2, 10, IOCON_MODE_INACT | IOCON_FUNC0,  2 }, /* INT Button */
		{ 0, 15, IOCON_MODE_PULLDOWN | IOCON_FUNC3, GPIO_CFG_UNASSIGNED }, /* SCK */
		{ 0, 17, IOCON_MODE_INACT | IOCON_FUNC3, GPIO_CFG_UNASSIGNED }, /* MISO */
		{ 0, 18, IOCON_MODE_INACT | IOCON_FUNC3, GPIO_CFG_UNASSIGNED }, /* MOSI */
		{ GPIO_CFG_UNASSIGNED }
};

volatile uint32_t* Gpio::pwm_mr[] = {
		&LPC_PWM1->MR1, &LPC_PWM1->MR2, &LPC_PWM1->MR3,
		&LPC_PWM1->MR4, &LPC_PWM1->MR5, &LPC_PWM1->MR6,
};

Gpio::Gpio(const GPIO_CFG_T* cfg)
{
	for (; cfg && cfg->port != GPIO_CFG_UNASSIGNED; cfg++)
		add(cfg);
}

void Gpio::add(const GPIO_CFG_T* cfg, byte pwm_ch)
{
	if (cfg && cfg->port != GPIO_CFG_UNASSIGNED) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, cfg->port, cfg->bit, cfg->modefunc);
		if (cfg->pin != GPIO_CFG_UNASSIGNED) {
			uint32_t prop = ((uint32_t)pwm_ch << 12) | ((uint32_t)cfg->modefunc << 8) | IOMUX(cfg->port, cfg->bit);
			if (0 < pwm_ch && pwm_ch <= N_PWM) {
				*pwm_mr[pwm_ch - 1] = LPC_PWM1->MR0;
				LPC_PWM1->LER |= (1 << pwm_ch);         // Update match register on next reset
				LPC_PWM1->PCR |= (1 << (pwm_ch + 8));   // Enable PWM.1n output
				prop |= GPIO_PWM;
			}
			gpioMapper.mapGpio(cfg->pin, prop);
		}
	}
}

void Gpio::add(const GPIO_CFG_T* cfg, ADC_CHANNEL_T adc_ch)
{
	if (cfg && cfg->port != GPIO_CFG_UNASSIGNED) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, cfg->port, cfg->bit, cfg->modefunc);
		if (cfg->pin != GPIO_CFG_UNASSIGNED) {
			uint32_t prop = ((uint32_t)adc_ch << 15) | ((uint32_t)cfg->modefunc << 8) | IOMUX(cfg->port, cfg->bit);
			if (ADC_CH0 <= adc_ch && adc_ch <= ADC_CH7) {
				Chip_ADC_EnableChannel(LPC_ADC, adc_ch, ENABLE);
				prop |= GPIO_ADC;
			}
			gpioMapper.mapGpio(cfg->pin, prop);
		}
	}
}

void Gpio::addClockOut(const GPIO_CFG_T* cfg, byte timer, byte match, uint32_t hz)
{
	if (cfg && cfg->port != GPIO_CFG_UNASSIGNED) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, cfg->port, cfg->bit, cfg->modefunc);

		timer &= 3, match &= 3;
		LPC_TIMER_T* pTMR = timer_csr[timer];
		/* Enable clock to the timer */
		Chip_TIMER_Init(pTMR);
		/* Reset the Timer Counter and the Prescale Counter on next PCLK */
		Chip_TIMER_Reset(pTMR);
		/* Sets the prescaler value */
		Chip_TIMER_PrescaleSet(pTMR, 0);
		if (hz) {
			/* Sets a timer match value */
			Chip_TIMER_SetMatch(pTMR, match, ((Chip_Clock_GetSystemClockRate() >> 3) / hz));
			/* Reset the timer counter on match */
			Chip_TIMER_ResetOnMatchEnable(pTMR, match);
		}
		/* Start counting */
		Chip_TIMER_Enable(pTMR);

		if (cfg->pin != GPIO_CFG_UNASSIGNED) {
			uint32_t prop = GPIO_CLKOUT | ((uint32_t)((timer << 2) | match) << 18) |
					((uint32_t)cfg->modefunc << 8) | IOMUX(cfg->port, cfg->bit);
			gpioMapper.mapGpio(cfg->pin, prop);
			Chip_TIMER_ExtMatchControlSet(pTMR, 0, TIMER_EXTMATCH_TOGGLE, match);
		}
	}
}

void Gpio::mapGpio(uint8_t pin, uint32_t prop)
{
	if (map.count(pin)) {
		/* pin already mapped */
		map.at(pin) = prop;
	} else {
		/* insert a new mapping */
		map.insert(std::pair<uint8_t, uint32_t>(pin, prop));
	}
}

void Gpio::unmapGpio(uint8_t pin)
{
	if (map.count(pin)) map.erase(pin);
}

int Gpio::getProp(uint8_t pin)
{
	return map.count(pin)? map.at(pin) : -1;
}

void Gpio::pwmSetDutyCycle(uint8_t pin, float ratio)
{
	byte pwm_ch;
	int prop = getProp(pin);
	if (prop >= 0 && (pwm_ch = IOPWM(prop)) > 0 && pwm_ch <= N_PWM) {
		*pwm_mr[pwm_ch - 1] = ratio * LPC_PWM1->MR0;
		LPC_PWM1->LER |= (1 << pwm_ch);
	}
}

void Board::init()
{
#if defined(__USE_LPCOPEN)
#if !defined(NO_BOARD_LIB)
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
	// Turn off all peripheral clocks
	LPC_SYSCTL->PCONP = 0;
	// Set up and initialize all required blocks and
	// functions related to the board hardware
	Board_Init();
#endif
#endif
	setupTimer();
	setupPWM();
	setupADC();
}

void Board::setupTimer()
{
	/* Initialize RITimer for delay APIs */
	Chip_RIT_Init(LPC_RITIMER);
	Chip_RIT_SetCOMPVAL(LPC_RITIMER, Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_RIT) / 1000);
	Chip_RIT_EnableCTRL(LPC_RITIMER, RIT_CTRL_ENCLR);
	Chip_RIT_TimerDebugDisable(LPC_RITIMER);
	NVIC_EnableIRQ(RITIMER_IRQn);
}

void Board::setupPWM(uint32_t cycle_in_usec)
{
	// Power on the PWM peripheral
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PWM1);

	LPC_PWM1->TCR = 2;              // reset the Timer Counter and the Prescale Counter
	LPC_PWM1->IR  = 0x7ff;          // clear any pending interrupts

	// Set prescale to 1 usec resolution (1000000Hz)
	LPC_PWM1->PR  = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_PWM1) / 1000000 - 1;
	LPC_PWM1->MR0 = cycle_in_usec;
	LPC_PWM1->LER = 1;
	LPC_PWM1->MCR = 2;       		// reset on MR0
	LPC_PWM1->TCR = (1 << 3) | 1;   // enable PWM mode and counting
}

void Board::setupADC(bool burst, uint32_t sampleRate)
{
	/* Initialize the ADC and load the ADC setup structure with default value */
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
	/* Set the ADC Sample rate & burst mode */
	ADCSetup.burstMode = burst;
	Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, sampleRate);
	Chip_ADC_SetBurstCmd(LPC_ADC, burst? ENABLE : DISABLE);
}
