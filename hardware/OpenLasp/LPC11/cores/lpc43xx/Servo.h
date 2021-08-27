#ifndef Servo_h
#define Servo_h

#include <inttypes.h>
#include "Arduino.h"

#define MIN_PULSE_WIDTH		544		// the shortest pulse sent to a servo
#define MAX_PULSE_WIDTH		2400	// the longest pulse sent to a servo
#define DEFAULT_PULSE_WIDTH	1500	// default pulse width when servo is attached

#define MAX_SERVOS	(Gpio::N_PWM)

class Servo {
public:
	Servo() : pinPwm(-1) {}
	void attach(byte pin, word min = MIN_PULSE_WIDTH, word max = MAX_PULSE_WIDTH);
	void detach();
	int read();
	void write(int angle);
	void writeMicroseconds(int pulse_in_usec);
	bool attached() { return (pinPwm >= 0); }
private:
	int8_t pinPwm;
	word pulseCurrent;
	word pulseMin;
	word pulseMax;
};

#endif
