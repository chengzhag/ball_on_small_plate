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
	ISepPoint(std::numeric_limits<float>::max())//初始化使I默认全程有效
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

PIDIntSepIncDiff::PIDIntSepIncDiff(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/, float stopFrq /*= 50*/) :
	PIDIntegralSeperate(kp, ki, kd, interval),
	filter(1 / interval, stopFrq)
{

}

float PIDIntSepIncDiff::refresh(float feedback)
{
	float err;
	err = target - feedback;

	//初始时使微分为0，避免突然的巨大错误微分
	if (isBegin)
	{
		errOld = err;
		isBegin = false;
	}

	output = kp*err + filter.getFilterOut(kd*(err - errOld));

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

float PIDGearshiftIntegral::fek(float ek)
{
	if (ek <= gearshiftPointL)
	{
		return 1;
	}
	else if (ek > gearshiftPointL + gearshiftPointH)
	{
		return 0;
	}
	else
	{
		return (gearshiftPointH - abs(ek) + gearshiftPointL)
			/ gearshiftPointH;
	}
}

PIDGearshiftIntegral::PIDGearshiftIntegral(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/) :
	PID(kp, ki, kd, interval),
	gearshiftPointL(std::numeric_limits<float>::max()),
	gearshiftPointH(std::numeric_limits<float>::max())////初始化使I默认全程有效
{

}

void PIDGearshiftIntegral::setGearshiftPoint(float pointL, float pointH)
{
	gearshiftPointL = pointL;
	gearshiftPointH = pointH;
}

float PIDGearshiftIntegral::refresh(float feedback)
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
		float ek = (err + errOld) / 2;
		integral += ki*fek(ek)*ek;
	}
	output = kp*err + integral + kd*(err - errOld);
	limit<float>(output, outputLimL, outputLimH);

	errOld = err;
	return output;
}

PIDGshifIntIncDiff::PIDGshifIntIncDiff(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/, float stopFrq /*= 50*/) :
	PIDGearshiftIntegral(kp, ki, kd, interval),
	filter(1 / interval, stopFrq)
{

}

float PIDGshifIntIncDiff::refresh(float feedback)
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
		float ek = (err + errOld) / 2;
		integral += ki*fek(ek)*ek;
	}
	output = kp*err + integral + filter.getFilterOut(kd*(err - errOld));
	limit<float>(output, outputLimL, outputLimH);

	errOld = err;
	return output;
}
