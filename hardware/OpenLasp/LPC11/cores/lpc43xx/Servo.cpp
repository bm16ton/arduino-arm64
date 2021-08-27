#include "Servo.h"

void Servo::attach(byte pin, word min, word max)
{
	int prop = gpioMapper.getProp(pin);
	if (prop >= 0 && (IOPWM(prop) != 0)) {
		pinPwm = pin;
		pulseMin = min, pulseMax = max;
		writeMicroseconds(pulseCurrent = DEFAULT_PULSE_WIDTH);
	}
}

void Servo::detach()
{
	if (attached()) {
		writeMicroseconds(DEFAULT_PULSE_WIDTH);
		pinPwm = -1;
	}
}

int Servo::read()
{
	return attached()? map(pulseCurrent, pulseMin, pulseMax, 0, 180) : -1;
}

void Servo::write(int angle)
{
	angle = constrain(angle, 0, 180);
	writeMicroseconds((uint32_t)map(angle, 0, 180, pulseMin, pulseMax));
}

void Servo::writeMicroseconds(int pulse_in_usec)
{
	pulse_in_usec = constrain(pulse_in_usec, pulseMin, pulseMax);
	if (attached())
		gpioMapper.pwmSetDutyCycle(pinPwm, (float)pulse_in_usec / LPC_PWM1->MR0);
}
