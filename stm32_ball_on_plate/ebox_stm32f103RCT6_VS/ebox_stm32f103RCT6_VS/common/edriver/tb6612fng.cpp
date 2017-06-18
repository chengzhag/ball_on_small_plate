#include "tb6612fng.h"

TB6612FNG::TB6612FNG(Gpio *motorPinA, Gpio *motorPinB,
	Gpio *motorPinPwm, uint32_t pwmFrequency) :
	pinA(motorPinA),
	pinB(motorPinB),
	pinP(motorPinPwm),
	pwm(motorPinPwm),
	frq(pwmFrequency),
	pct(0)
{

}

void TB6612FNG::begin()
{
	pinA->mode(OUTPUT_PP);
	pinB->mode(OUTPUT_PP);
	pwm.begin(frq, 0);
	pwm.set_oc_polarity(1);
}

void TB6612FNG::setPercent(float p)
{
	if (p>100)
	{
		pct = 100;
	}
	else if (p < -100)
	{
		pct = -100;
	}
	else
	{
		pct = p;
	}
	
	uint16_t duty = abs(pct * 10);
	if (p > 0)
	{
		pinA->set();
		pinB->reset();
	}
	else if (p < 0)
	{
		pinB->set();
		pinA->reset();
	}
	else
	{
		pinB->reset();
		pinA->reset();
	}
	pwm.set_duty(duty);
}

float TB6612FNG::getPercent()
{
	return pct;
}
