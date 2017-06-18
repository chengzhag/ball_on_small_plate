#include "encoder_exti.h"

void EncoderExti::eventA()
{
	int state = pinA->read() << 1 | pinB->read();
	switch (state)
	{
	case 0:
	case 3:
		pos++;
		break;
	case 1:
	case 2:
		pos--;
		break;
	}
}

void EncoderExti::eventB()
{
	int state = pinA->read() << 1 | pinB->read();
	switch (state)
	{
	case 0:
	case 3:
		pos--;
		break;
	case 1:
	case 2:
		pos++;
		break;
	}
}

EncoderExti::EncoderExti(Gpio *encoderPinA, Gpio *encoderPinB) :
	pinA(encoderPinA), pinB(encoderPinB),
	extiA(encoderPinA, EXTI_Trigger_Rising_Falling),
	extiB(encoderPinB, EXTI_Trigger_Rising_Falling),
	pos(0), posOld(0)
{
}

void EncoderExti::begin()
{
	extiA.begin();
	extiA.attach(this, &EncoderExti::eventA);
	extiA.interrupt(ENABLE);
	
	extiB.begin();
	extiB.attach(this, &EncoderExti::eventB);
	extiB.interrupt(ENABLE);
}

long EncoderExti::getPos()
{
	return pos;
}

void EncoderExti::resetPos()
{
	pos = 0;
}

void EncoderExti::refreshDiff()
{
	diff = pos - posOld;
	posOld = pos;
}

long EncoderExti::getDiff()
{
	return diff;
}
