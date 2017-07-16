#include "PID.h"

PID::PID(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/) :
	isBegin(true),
	output(0)
{
	setInterval(interval);
	setPID(kp, ki, kd);
	setOutputLim(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	setTarget(0);
	reset();
}

void PID::setInterval(float interval)
{
	this->interval = interval;
}

void PID::setPID(float kp, float ki, float kd)
{
	this->kp = kp;
	this->ki = ki*interval;
	this->kd = kd / interval;
}

void PID::setOutputLim(float limL, float limH)
{
	this->outputLimL = limL;
	this->outputLimH = limH;
}

void PID::setTarget(float target)
{
	this->target = target;
}

void PID::reset()
{
	integral = 0;
	errOld = 0;
	isBegin = true;
}
