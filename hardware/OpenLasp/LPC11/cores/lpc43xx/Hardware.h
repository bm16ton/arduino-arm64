#ifndef Hardware_h
#define Hardware_h

/* LPC17xx supports 6 single edge controlled PWM outputs and 8 channels 12-bit ADC inputs,
 * these are configurable pins through the pin connect block. The overload function
 * gpioMapper.add() should be called from the init() to associate the PWM/ADC channel number
 * with the GPIO pins like the following example:
 *
 * void init() {
 *    // Define pin9 as PWM1.2 on P1.20
 *    GPIO_CFG_T gpioPwm = { 1, 20, IOCON_MODE_INACT | IOCON_FUNC2, 9 };
 *    gpioMapper.add(&gpioPwm, 2);
 *    // Define pin3 as AD0.0 on P0.23
 *    GPIO_CFG_T gpioAin = { 0, 23, IOCON_MODE_INACT | IOCON_FUNC1, 3 };
 *    gpioMapper.add(&gpioAin, ADC_CH0);
 * }
 */

#include <map>

#define IOMUX(pingrp,pinnum)	(((pingrp)<<5)|(pinnum))
#define IOGRP(prop)				(((prop)>>5)&7)
#define IONUM(prop)				((prop)&0x1f)
#define IOFUNC(prop)			(((prop)>>8)&3)
#define IOPWM(prop)				(((prop)>>12)&7)
#define IOADC(prop)				(((prop)>>15)&7)
#define IOTIMER(prop)			(((prop)>>20)&3)
#define IOMATCH(prop)			(((prop)>>18)&3)

#define GPIO_INVALID			(1 << 31)
#define GPIO_PWM				(1 << 30)
#define GPIO_ADC				(1 << 29)
#define GPIO_CLKOUT				(1 << 28)

#define GPIO_CFG_UNASSIGNED	0xFF
typedef struct {
	byte port;
	byte bit;
	byte modefunc;
	byte pin;
} GPIO_CFG_T;

extern GPIO_CFG_T gpioCfgDefault[];

class Gpio {
public:
	static const int N_PWM = 6;
	Gpio(const GPIO_CFG_T* cfg = gpioCfgDefault);
	void add(const GPIO_CFG_T* cfg, byte pwm_ch = 0);
	void add(const GPIO_CFG_T* cfg, ADC_CHANNEL_T adc_ch);
	void addClockOut(const GPIO_CFG_T* cfg, byte timer, byte match, uint32_t hz);
	void mapGpio(uint8_t pin, uint32_t prop);
	void unmapGpio(uint8_t pin);
	int getProp(uint8_t pin);
	void pwmSetDutyCycle(uint8_t pin, float ratio);
private:
	static volatile uint32_t* pwm_mr[N_PWM];
	std::map<uint8_t, uint32_t> map;
};

extern Gpio gpioMapper;

class Board {
public:
	virtual void init();
	virtual void setupTimer();
	virtual void setupPWM(uint32_t cycle_in_usec = 20000);
	virtual void setupADC(bool burst = false, uint32_t sampleRate = ADC_MAX_SAMPLE_RATE);
};

class TargetBoard {
public:
	TargetBoard(Board* board = new Board) {
		board->init();
	}
};

#endif
