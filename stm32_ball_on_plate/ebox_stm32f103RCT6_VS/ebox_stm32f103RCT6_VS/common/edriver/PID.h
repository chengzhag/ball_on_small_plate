#ifndef  __PID
#define  __PID

#include <limits>
#include "my_math.h"
#include "signal_stream.h"
#include <math.h>
#include "FunctionPointer.h"

//PID基类
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

	//同时设置interval和PID
	void setBasic(float kp, float ki, float kd, float interval);
	
	//设置输出范围，超过范围停止积分继续增加
	void setOutputLim(float limL, float limH);

	//设置控制目标
	void setTarget(float target);

	//清除积分项、其他暂存项
	void resetState();

	//待实现的PID算法
	virtual float refresh(float feedback) = 0;

};

//普通梯形积分PID
class PIDnorm:virtual public PID
{
public:
	//普通梯形积分PID算法
	PIDnorm(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//普通PID的算法
	virtual float refresh(float feedback);
};

//积分分离PID
class PIDIntegralSeperate :virtual public PID
{
protected:
	float ISepPoint;
public:
	//积分分离PID算法
	PIDIntegralSeperate(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//设置积分分离点
	void setISepPoint(float ISepPoint);

	//积分分离PID算法
	virtual float refresh(float feedback);
};

//不完全微分PID
class PIDIncompleteDiff :virtual public PID
{
protected:
	RcFilter filter;
public:
	//不完全微分PID算法
	PIDIncompleteDiff(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01, float stopFrq = 50);

	//不完全微分PID算法
	virtual float refresh(float feedback);
};

//积分分离不完全微分PID
class PIDIntSepIncDiff : public PIDIntegralSeperate, public PIDIncompleteDiff
{
public:
	//积分分离不完全微分PID算法
	PIDIntSepIncDiff(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01, float stopFrq = 50);

	//积分分离不完全微分PID算法
	virtual float refresh(float feedback);
};

//变速积分PID
class PIDGearshiftIntegral :virtual public PID
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
	virtual float refresh(float feedback);

};

//变速积分不完全微分PID
class PIDGshifIntIncDiff:public PIDGearshiftIntegral,public PIDIncompleteDiff
{
public:
	//变速积分不完全微分PID算法
	PIDGshifIntIncDiff(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01, float stopFrq = 50);

	//变速积分不完全微分PID算法
	virtual float refresh(float feedback);
};

//微分先行PID
class PIDDifferentialAhead :virtual public PID
{
	float feedbackOld;
public:
	//微分先行PID算法
	PIDDifferentialAhead(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//微分先行PID算法
	virtual float refresh(float feedback);
};

//前馈补偿PID
class PIDFeedforward :virtual public PID
{
protected:
	FunctionPointerArg1<float, float> feedforwardH;
	float feedforward;
public:
	//前馈补偿PID算法
	PIDFeedforward(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//绑定系统函数feedforwardH，参数为时域离散信号，返回系统输出信号
	void attach(float(*feedforwardH)(float input));

	//绑定系统函数feedforwardH，参数为时域离散信号，返回系统输出信号
	template<typename T>
	void attach(T *pObj, float (T::*feedforwardH)(float input));

	virtual float refresh(float feedback);

	//获取当前前馈补偿值
	float getFeedforward();
};
template<typename T>
void PIDFeedforward::attach(T *pObj, float (T::*feedforwardH)(float input))
{
	this->feedforwardH.attach(pObj, feedforwardH);
}

//前馈补偿变速积分不完全微分PID
class PIDFeforGshifIntIncDiff :public PIDFeedforward, public PIDGearshiftIntegral, public PIDIncompleteDiff
{
public:
	PIDFeforGshifIntIncDiff(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01, float stopFrq = 50);

	virtual float refresh(float feedback);
};

//带死区的PID
class PIDDeadzone:virtual public PID
{
protected:
	float deadzone;
public:
	PIDDeadzone(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01, float deadzone = 0);

	virtual float refresh(float feedback);
};

//带死区的前馈补偿变速积分不完全微分PID
class PIDFeforGshifIntIncDiffDezone:public PIDFeforGshifIntIncDiff,public PIDDeadzone
{
public:
	PIDFeforGshifIntIncDiffDezone(float kp = 0, float ki = 0, float kd = 0, 
		float interval = 0.01, float stopFrq = 50, float deadzone = 0);

	virtual float refresh(float feedback);
};


#endif
