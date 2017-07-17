#ifndef  __PID
#define  __PID

#include <limits>
#include "my_math.h"
#include "signal_stream.h"
#include <math.h>

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
	PID(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//设置采样间隔时间，单位s
	//调用setPID前必须先设置好interval
	void setInterval(float interval);

	//调用setPID前必须先设置好interval
	void setPID(float kp, float ki, float kd);
	
	//设置输出范围，超过范围停止积分继续增加
	void setOutputLim(float limL, float limH);

	//设置控制目标
	void setTarget(float target);

	//清除积分项、其他暂存项
	void reset();

	//待实现的PID算法
	virtual float refresh(float feedback) = 0;

};

class PIDnorm:public PID
{
public:
	//普通梯形积分PID算法
	PIDnorm(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//普通PID的算法
	float refresh(float feedback);
};

class PIDIntegralSeperate :public PID
{
protected:
	float ISepPoint;
public:
	//积分分离PID算法
	PIDIntegralSeperate(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//设置积分分离点
	void setISepPoint(float ISepPoint);

	//积分分离PID算法
	float refresh(float feedback);
};

class PIDIncompleteDiff :public PID
{
protected:
	Butterworth filter;
public:
	//不完全微分PID算法
	PIDIncompleteDiff(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01, float stopFrq = 50);

	//不完全微分PID算法
	float refresh(float feedback);
};

class PIDIntSepIncDiff :public PIDIntegralSeperate
{
protected:
	Butterworth filter;
public:
	//积分分离不完全微分PID算法
	PIDIntSepIncDiff(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01, float stopFrq = 50);

	//积分分离不完全微分PID算法
	float refresh(float feedback);
};

class PIDGearshiftIntegral :public PID
{
protected:
	float gearshiftPointL, gearshiftPointH;
	float fek(float ek);
public:
	//变速积分PID算法
	//ui(k)=ki*{sum 0,k-1 e(i)+f[e(k)]e(k)}*T
	//f[e(k)]= 	{	1						,|e(k)|<=B
	//					[A-|e(k)|+B]/A	,B<|e(k)|<=A+B
	//					0						,|e(k)|>A+B
	PIDGearshiftIntegral(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//设置变速积分加权曲线参数
	void setGearshiftPoint(float pointL, float pointH);

	//变速积分PID算法
	float refresh(float feedback);

};

class PIDGshifIntIncDiff:public PIDGearshiftIntegral
{
protected:
	Butterworth filter;
public:
	//变速积分不完全微分PID算法
	PIDGshifIntIncDiff(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01, float stopFrq = 50) :
		PIDGearshiftIntegral(kp, ki, kd, interval),
		filter(1 / interval, stopFrq)
	{

	}

	//变速积分不完全微分PID算法
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
			float ek = (err + errOld) / 2;
			integral += ki*fek(ek)*ek;
		}
		output = kp*err + integral + filter.getFilterOut(kd*(err - errOld));
		limit<float>(output, outputLimL, outputLimH);

		errOld = err;
		return output;
	}
};



#endif
