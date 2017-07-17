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

PIDnorm::PIDnorm(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/) :
	PID(kp, ki, kd, interval)
{

}

float PIDnorm::refresh(float feedback)
{
	float err;
	err = target - feedback;

	//初始时使微分为0，避免突然的巨大错误微分
	if (isBegin)
	{
		errOld = err;
		isBegin = false;
	}

	//超过输出范围停止积分继续增加
	if ((output > outputLimL && output < outputLimH) ||
		(output == outputLimH && err < 0) ||
		(output == outputLimL && err > 0))
	{
		integral += ki*(err + errOld) / 2;
	}
	output = kp*err + integral + kd*(err - errOld);
	limit<float>(output, outputLimL, outputLimH);

	errOld = err;
	return output;
}

PIDIntegralSeperate::PIDIntegralSeperate(float kp, float ki, float kd, float interval) :
	PID(kp, ki, kd, interval),
	ISepPoint(std::numeric_limits<float>::max())
{

}

void PIDIntegralSeperate::setISepPoint(float ISepPoint)
{
	this->ISepPoint = ISepPoint;
}

float PIDIntegralSeperate::refresh(float feedback)
{
	float err;
	err = target - feedback;

	//初始时使微分为0，避免突然的巨大错误微分
	if (isBegin)
	{
		errOld = err;
		isBegin = false;
	}

	output = kp*err + kd*(err - errOld);

	//超过输出范围停止积分继续增加
	if (err < ISepPoint && err > -ISepPoint)
	{
		if ((output > outputLimL && output < outputLimH) ||
			(output == outputLimH && err < 0) ||
			(output == outputLimL && err > 0))
		{
			integral += ki*(err + errOld) / 2;
		}
		output += integral;
	}

	limit<float>(output, outputLimL, outputLimH);

	errOld = err;
	return output;
}

PIDIncompleteDiff::PIDIncompleteDiff(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/, float stopFrq /*= 50*/) :
	PID(kp, ki, kd, interval),
	filter(1 / interval, stopFrq)
{

}

float PIDIncompleteDiff::refresh(float feedback)
{
	float err;
	err = target - feedback;

	//初始时使微分为0，避免突然的巨大错误微分
	if (isBegin)
	{
		errOld = err;
		isBegin = false;
	}

	//超过输出范围停止积分继续增加
	if ((output > outputLimL && output < outputLimH) ||
		(output == outputLimH && err < 0) ||
		(output == outputLimL && err > 0))
	{
		integral += ki*(err + errOld) / 2;
	}
	output = kp*err + integral + filter.getFilterOut(kd*(err - errOld));
	limit<float>(output, outputLimL, outputLimH);

	errOld = err;
	return output;
}
