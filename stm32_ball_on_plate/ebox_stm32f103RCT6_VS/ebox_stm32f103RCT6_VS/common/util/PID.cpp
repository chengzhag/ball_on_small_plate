#include "PID.h"

PID::PID(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/) :
	isBegin(true),
	output(0)
{
	setBasic(kp, ki, kd, interval);
	setOutputLim(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	setTarget(0);
	resetState();
}

void PID::setInterval(float interval)
{
	this->interval = interval;
}

float PID::getInterval()
{
	return interval;
}

void PID::setPID(float kp, float ki, float kd)
{
	this->kp = kp;
	this->ki = ki*interval;
	this->kd = kd / interval;
}

void PID::getPID(float *kp, float *ki, float *kd)
{
	*kp = this->kp;
	*ki = this->ki / interval;
	*kd = this->kd*interval;
}

void PID::setBasic(float kp, float ki, float kd, float interval)
{
	setInterval(interval);
	setPID(kp, ki, kd);
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

float PID::getTarget()
{
	return target;
}

void PID::resetState()
{
	errOld = 0;
	isBegin = true;
	output = 0;
}

PIDPosition::PIDPosition(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/) :
	PID(kp, ki, kd, interval)
{
	resetState();
}

float PIDPosition::refresh(float feedback)
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

void PIDPosition::resetState()
{
	integral = 0;
	PID::resetState();
}

PIDIntegralSeperate::PIDIntegralSeperate(float kp, float ki, float kd, float interval) :
	PIDPosition(kp, ki, kd, interval),
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
	PIDPosition(kp, ki, kd, interval),
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
	PIDPosition(kp, ki, kd, interval),
	PIDIntegralSeperate(kp, ki, kd, interval),
	PIDIncompleteDiff(kp, ki, kd, interval, stopFrq)
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
	PIDPosition(kp, ki, kd, interval),
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
	PIDPosition(kp, ki, kd, interval),
	PIDGearshiftIntegral(kp, ki, kd, interval),
	PIDIncompleteDiff(kp, ki, kd, interval, stopFrq)
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

PIDDifferentialAhead::PIDDifferentialAhead(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/) :
	PIDPosition(kp, ki, kd, interval),
	feedbackOld(0)
{

}

float PIDDifferentialAhead::refresh(float feedback)
{
	float err;
	err = target - feedback;

	//初始时使微分为0，避免突然的巨大错误微分
	if (isBegin)
	{
		feedbackOld = feedback;
		isBegin = false;
	}

	//超过输出范围停止积分继续增加
	if ((output > outputLimL && output < outputLimH) ||
		(output == outputLimH && err < 0) ||
		(output == outputLimL && err > 0))
	{
		integral += ki*(err + errOld) / 2;
	}
	output = kp*err + integral + kd*(feedbackOld - feedback);
	limit<float>(output, outputLimL, outputLimH);

	errOld = err;
	feedbackOld = feedback;
	return output;
}

PIDFeedforward::PIDFeedforward(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/) :
	PIDPosition(kp, ki, kd, interval),
	feedforward(0)
{

}

void PIDFeedforward::attachFeedForwardH(float(*feedforwardH)(float input))
{
	this->feedforwardH.attach(feedforwardH);
}

float PIDFeedforward::refresh(float feedback)
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
	feedforward = feedforwardH.call(target);
	output = kp*err + integral + kd*(err - errOld)
		+ feedforward;//FunctionPointer未绑定时默认返回0

	limit<float>(output, outputLimL, outputLimH);

	errOld = err;
	return output;
}

float PIDFeedforward::getFeedforward()
{
	return feedforward;
}

PIDFeforGshifIntIncDiff::PIDFeforGshifIntIncDiff(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/, float stopFrq /*= 50*/) :
	PIDPosition(kp, ki, kd, interval),
	PIDFeedforward(kp, ki, kd, interval),
	PIDGearshiftIntegral(kp, ki, kd, interval),
	PIDIncompleteDiff(kp, ki, kd, interval, stopFrq)
{

}

float PIDFeforGshifIntIncDiff::refresh(float feedback)
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
	feedforward = feedforwardH.call(target);
	output = kp*err + integral + filter.getFilterOut(kd*(err - errOld))
		+ feedforward;//FunctionPointer未绑定时默认返回0
	limit<float>(output, outputLimL, outputLimH);

	errOld = err;
	return output;
}

PIDDeadzone::PIDDeadzone(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/, float deadzone /*= 0*/) :
	PIDPosition(kp, ki, kd, interval),
	deadzone(deadzone)
{

}

float PIDDeadzone::refresh(float feedback)
{
	float err;
	err = target - feedback;

	//初始时使微分为0，避免突然的巨大错误微分
	if (isBegin)
	{
		errOld = err;
		isBegin = false;
	}

	//如果在死区外
	if (err > deadzone || err < -deadzone)
	{
		//超过输出范围停止积分继续增加
		if ((output > outputLimL && output < outputLimH) ||
			(output == outputLimH && err < 0) ||
			(output == outputLimL && err > 0))
		{
			integral += ki*(err + errOld) / 2;
		}
		output = kp*err + integral + kd*(err - errOld);
	}
	else
	{
		output = integral;//在死区内保留旧积分项
	}
	limit<float>(output, outputLimL, outputLimH);


	errOld = err;
	return output;
}

PIDFeforGshifIntIncDiffDezone::PIDFeforGshifIntIncDiffDezone(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/, float stopFrq /*= 50*/, float deadzone /*= 0*/) :
	PIDPosition(kp, ki, kd, interval),
	PIDFeforGshifIntIncDiff(kp, ki, kd, interval, stopFrq),
	PIDDeadzone(kp, ki, kd, interval, deadzone)
{

}

float PIDFeforGshifIntIncDiffDezone::refresh(float feedback)
{
	float err;
	err = target - feedback;

	//初始时使微分为0，避免突然的巨大错误微分
	if (isBegin)
	{
		errOld = err;
		isBegin = false;
	}

	//如果在死区外
	if (err > deadzone || err < -deadzone)
	{
		//超过输出范围停止积分继续增加
		if ((output > outputLimL && output < outputLimH) ||
			(output == outputLimH && err < 0) ||
			(output == outputLimL && err > 0))
		{
			float ek = (err + errOld) / 2;
			integral += ki*fek(ek)*ek;
		}
		feedforward = feedforwardH.call(target);
		output = kp*err + integral + filter.getFilterOut(kd*(err - errOld))
			+ feedforward;//FunctionPointer未绑定时默认返回0
	}
	else
	{
		feedforward = feedforwardH.call(target);
		output = integral + feedforward;//FunctionPointer未绑定时默认返回0
	}
	limit<float>(output, outputLimL, outputLimH);

	errOld = err;
	return output;
}

PIDIncrease::PIDIncrease(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/) :
	PID(kp, ki, kd, interval)
{
	resetState();
}

float PIDIncrease::refresh(float feedback)
{
	//计算err、deltaErr
	//参与控制的变量还有errOld、deltaErrOld
	float err;
	err = target - feedback;
	//初始时使微分为0，避免突然的巨大错误微分
	if (isBegin)
	{
		errOld = err;
		isBegin = false;
	}
	float deltaErr = err - errOld;

	output = kp*deltaErr + ki*err + kd*(deltaErr - deltaErrOld);
	limit<float>(output, outputLimL, outputLimH);

	errOld = err;
	deltaErrOld = deltaErr;
	return output;
}

void PIDIncrease::resetState()
{
	deltaErrOld = 0;
	PID::resetState();
}

RepetitiveController::RepetitiveController(int length, int forwardCorrect, float k, float frq, float fh) :
	err(length),
	filter(frq, fh),
	k(k),
	forwardCorrect(forwardCorrect)
{

}

void RepetitiveController::clear()
{
	err.clear();
	filter.clear();
}

float RepetitiveController::refresh(float feedback)
{
	float oldestErr = err.getOldest();
	float filterOut = filter.getFilterOut(feedback);
	err.push(k*filterOut + oldestErr);

	//float output = sz->getY(err.getOldest());

	return err[err.getLength() - 1 - forwardCorrect];
}

void RepetitiveController::setLength(int length)
{
	err.setLength(length);
}

PIDFuzzy::PIDFuzzy(float kp /*= 0*/, float ki /*= 0*/, float kd /*= 0*/, float interval /*= 0.01*/) :
	PIDPosition(kp, ki, kd, interval),
	kpF(0), kiF(0), kdF(0),
	fuzzyTableKp(NULL), fuzzyTableKi(NULL), fuzzyTableKd(NULL)
{

}
