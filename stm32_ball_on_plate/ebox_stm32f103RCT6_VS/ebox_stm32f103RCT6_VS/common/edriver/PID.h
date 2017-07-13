#ifndef  __PID
#define  __PID

#include <limits>
#include "my_math.h"

class PID
{
protected:
	bool isBegin;
	float kp, ki, kd;
	float integral;
	float interval;
	float outputLimL, outputLimH;
	float target;
	float errOld;
	float output;
public:
	PID(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01) :
		isBegin(true),
		output(0)
	{
		setInterval(interval);
		setPID(kp, ki, kd);
		setOutputLim(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		setTarget(0);
		reset();
	}

	//设置采样间隔时间，单位s
	//调用setPID前必须先设置好interval
	void setInterval(float interval)
	{
		this->interval = interval;
	}

	//调用setPID前必须先设置好interval
	void setPID(float kp, float ki, float kd)
	{
		this->kp = kp;
		this->ki = ki*interval;
		this->kd = kd / interval;
	}
	
	//设置输出范围，超过范围停止积分继续增加
	void setOutputLim(float limL, float limH)
	{
		this->outputLimL = limL;
		this->outputLimH = limH;
	}

	//设置控制目标
	void setTarget(float target)
	{
		this->target = target;
	}

	//清除积分项、其他暂存项
	void reset()
	{
		integral = 0;
		errOld = 0;
		isBegin = true;
	}

	//待实现的PID算法
	virtual float refresh(float feedback) = 0;

};

class PIDnorm:public PID
{
public:
	PIDnorm(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01) :
		PID(kp, ki, kd, interval)
	{

	}

	//普通PID的算法
	float refresh(float feedback)
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
		if ((output > outputLimL && output < outputLimH)||
			(output == outputLimH && err < 0)||
			(output == outputLimL && err > 0))
		{
			integral += ki*err;
		}
		output = kp*err + integral + kd*(err - errOld);
		limit<float>(output, outputLimL, outputLimH);

		errOld = err;
		return output;
	}
};

class PIDIntegralSeperate :public PID
{
protected:
	float ISepPoint;
public:
	PIDIntegralSeperate(float kp, float ki, float kd, float interval) :
		PID(kp, ki, kd, interval),
		ISepPoint(std::numeric_limits<float>::max())
	{

	}

	//设置积分分离点
	void setISepPoint(float ISepPoint)
	{
		this->ISepPoint = ISepPoint;
	}

	//积分分离PID算法
	float refresh(float feedback)
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
		if (err > ISepPoint || err < -ISepPoint)
		{
			if ((output > outputLimL && output < outputLimH) ||
				(output == outputLimH && err < 0) ||
				(output == outputLimL && err > 0))
			{
				integral += ki*err;
			}
			output += integral;
		}
		
		limit<float>(output, outputLimL, outputLimH);

		errOld = err;
		return output;
	}
};

template <float cutOffFre>
class PIDIncompleteDiff :public PID
{
public:
	PIDIncompleteDiff(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01) :
		PID(kp, ki, kd, interval)
	{

	}

	//不完全微分PID算法
	float refresh(float feedback)
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
			integral += ki*err;
		}
		output = kp*err + integral + kd*(err - errOld);
		limit<float>(output, outputLimL, outputLimH);

		errOld = err;
		return output;
	}
};

#endif
