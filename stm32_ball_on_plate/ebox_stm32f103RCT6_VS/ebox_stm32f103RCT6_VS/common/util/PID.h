#ifndef  __PID
#define  __PID

#include <limits>
#include "my_math.h"
#include "signal_stream.h"
#include <math.h>
#include "FunctionPointer.h"
#include "interpolation.h"


//PID基类
class PID
{
protected:
	bool isBegin;
	float kp, ki, kd;
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
	//获取间隔时间
	float getInterval();

	//设置PID，调用setPID前必须先设置好interval
	void setPID(float kp, float ki, float kd);
	//获取PID
	virtual void getPID(float *kp, float *ki, float *kd);

	//同时设置interval和PID
	void setBasic(float kp, float ki, float kd, float interval);
	
	//设置输出范围，超过范围停止积分继续增加
	void setOutputLim(float limL, float limH);

	//设置控制目标
	void setTarget(float target);
	//获取控制目标
	float getTarget();

	//清除暂存项
	virtual void resetState();

	//待实现的PID算法
	virtual float refresh(float feedback) = 0;

};

//普通梯形积分PID基类
class PIDPosition : public PID
{
protected:
	float integral;
public:
	//普通梯形积分PID算法
	PIDPosition(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//普通PID的算法
	virtual float refresh(float feedback);

	//清除积分项、其他暂存项
	virtual void resetState();
};

//普通增量PID基类
class PIDIncrease : public PID
{
protected:
	float deltaErrOld;
public:
	//增量PID算法
	PIDIncrease(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//增量PID的算法
	virtual float refresh(float feedback);

	//清除暂存项
	virtual void resetState();
};

//积分分离PID
class PIDIntegralSeperate :virtual public PIDPosition
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
class PIDIncompleteDiff :virtual public PIDPosition
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
class PIDGearshiftIntegral :virtual public PIDPosition
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
class PIDDifferentialAhead :virtual public PIDPosition
{
	float feedbackOld;
public:
	//微分先行PID算法
	PIDDifferentialAhead(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//微分先行PID算法
	virtual float refresh(float feedback);
};

//前馈补偿PID
class PIDFeedforward :virtual public PIDPosition
{
protected:
	FunctionPointerArg1<float, float> feedforwardH;
	float feedforward;
public:
	//前馈补偿PID算法
	PIDFeedforward(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//绑定系统函数feedforwardH，参数为时域离散信号，返回系统输出信号
	void attachFeedForwardH(float(*feedforwardH)(float input));

	//绑定系统函数feedforwardH，参数为时域离散信号，返回系统输出信号
	template<typename T>
	void attachFeedForwardH(T *pObj, float (T::*feedforwardH)(float input));

	virtual float refresh(float feedback);

	//获取当前前馈补偿值
	float getFeedforward();
};
template<typename T>
void PIDFeedforward::attachFeedForwardH(T *pObj, float (T::*feedforwardH)(float input))
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
class PIDDeadzone:virtual public PIDPosition
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

//专家PID
class PIDExpert :virtual public PIDPosition
{
	float deltaErrOld;
	u8 rule;//指示当前属于哪条规则
	//对应《先进PID控制及其MATLAB仿真》中的五个参数：
	float m1, m2, k1, k2, e;
public:
	//专家PID算法
	PIDExpert(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01):
		PIDPosition(kp, ki, kd, interval),
		rule(0)
	{
		resetState();
		setParams(0, 0, 0, 0, 0);
	}

	//设置专家PID的五个参数
	//m1：认为很大的误差，超过m1以最大输出
	//m2：认为较大的误差，m2<m1
	//k1：增益放大系数，k1>1
	//k2：抑制系数，0<k2<1
	//e：认为较小的误差
	void setParams(float m1, float m2, float k1, float k2, float e)
	{
		this->m1 = m1;
		this->m2 = m2;
		this->k1 = k1;
		this->k2 = k2;
		this->e = e;
	}

	//专家PID算法
	virtual float refresh(float feedback)
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

		rule = 0;

		//1. 误差绝对值已经很大，应考虑最大（最小）输出（开环控制）
		if (abs(err) > m1)
		{
			output = err > 0 ? outputLimH : outputLimL;
			rule = 1;
		}

		//2. 误差在朝误差绝对值增大的方向变化，或误差为常数
		else if (err*deltaErr > 0 || deltaErr == 0)
		{
			//误差绝对值较大，可以考虑实施较强的控制作用，
			//以扭转到误差绝对值减小的方向
			if (abs(err) >= m2)
			{
				output = k1*kp*err;
			}
			//误差绝对值并不大，考虑实施一般的控制作用，
			//以扭转到误差绝对值减小的方向
			else
			{
				output = k2*kp*err;
			}
			rule = 2;
		}

		//3. 误差在朝误差绝对值减小的方向变化，或达到平衡
		//考虑保持输出不变
		else if (
			((err*deltaErr < 0) && (deltaErr*deltaErrOld > 0))
			|| (err == 0)
			)
		{
			//超过输出范围停止积分继续增加
			if ((output > outputLimL && output < outputLimH) ||
				(output == outputLimH && err < 0) ||
				(output == outputLimL && err > 0))
			{
				integral += ki*(err + errOld) / 2;
			}
			output = kp*err + integral + kd*deltaErr;
			rule = 3;
		}

		//4. 误差处于极值状态
		else if ((err*deltaErr < 0) && (deltaErr*deltaErrOld < 0))
		{
			//误差绝对值较大，可以考虑实施较强的控制作用
			if (abs(err) >= m2)
			{
				output = k1*kp*errOld;
			}
			else
			{
				output = k2*kp*errOld;
			}
			rule = 4;
		}

		//5. 误差绝对值很小
		if (abs(err) <= e)
		{
			//超过输出范围停止积分继续增加
			if ((output > outputLimL && output < outputLimH) ||
				(output == outputLimH && err < 0) ||
				(output == outputLimL && err > 0))
			{
				integral += ki*(err + errOld) / 2;
			}
			output = kp*err + integral;
			rule = 5;
		}

		limit<float>(output, outputLimL, outputLimH);
		errOld = err;
		deltaErrOld = deltaErr;
		return output;
	}

	void resetState()
	{
		deltaErrOld = 0;
		PIDPosition::resetState();
	}

	u8 getCurrentRule()
	{
		return rule;
	}
};

//模糊自适应整定PID
class PIDFuzzy :virtual public PIDPosition
{
protected:
	float kpF, kiF, kdF;
	Interpolation2D *fuzzyTableKp, *fuzzyTableKi, *fuzzyTableKd;
	void refreshPID(float err,float deltaErr)
	{
		///通过模糊控制查询表插值得出kp、ki、kd的调整量
		float x, y;
		float limL, limH;

		//获取deltaKp
		x = err; y = deltaErr;
		fuzzyTableKp->getXLim(&limL, &limH);
		limit(x, limL, limH);
		fuzzyTableKp->getYLim(&limL, &limH);
		limit(y, limL, limH);
		float deltaKp = fuzzyTableKp->getZ(x, y);

		//获取deltaKi
		x = err; y = deltaErr;
		fuzzyTableKi->getXLim(&limL, &limH);
		limit(x, limL, limH);
		fuzzyTableKi->getYLim(&limL, &limH);
		limit(y, limL, limH);
		float deltaKi = fuzzyTableKi->getZ(x, y);

		//获取deltaKd
		x = err; y = deltaErr;
		fuzzyTableKd->getXLim(&limL, &limH);
		limit(x, limL, limH);
		fuzzyTableKd->getYLim(&limL, &limH);
		limit(y, limL, limH);
		float deltaKd = fuzzyTableKd->getZ(x, y);

		//设置新PID
		kpF = kp + deltaKp;
		kiF = ki + deltaKi * interval;
		kdF = kd + deltaKd / interval;
		limitLow<float>(kpF, 0);
		limitLow<float>(kiF, 0);
		limitLow<float>(kdF, 0);
	}
public:
	PIDFuzzy(float kp = 0, float ki = 0, float kd = 0, float interval = 0.01);

	//设置模糊控制查询表
	void setFuzzyTable(
		Interpolation2D *fuzzyTableKp
		, Interpolation2D *fuzzyTableKi
		, Interpolation2D *fuzzyTableKd)
	{
		this->fuzzyTableKp = fuzzyTableKp;
		this->fuzzyTableKi = fuzzyTableKi;
		this->fuzzyTableKd = fuzzyTableKd;
	}

	float refresh(float feedback)
	{
		float err;
		err = target - feedback;
		float deltaErr = err - errOld;


		//初始时使微分为0，避免突然的巨大错误微分
		if (isBegin)
		{
			errOld = err;
			isBegin = false;
		}

		refreshPID(err, deltaErr);

		//超过输出范围停止积分继续增加
		if ((output > outputLimL && output < outputLimH) ||
			(output == outputLimH && err < 0) ||
			(output == outputLimL && err > 0))
		{
			integral += kiF*(err + errOld) / 2;
		}
		output = kpF*err + integral + kdF*(err - errOld);
		limit<float>(output, outputLimL, outputLimH);

		errOld = err;
		return output;
	}

	//获取模糊自适应后的PID值
	void getPID(float *kp, float *ki, float *kd)
	{
		*kp = this->kpF;
		*ki = this->kiF / interval;
		*kd = this->kdF*interval;
	}
};

//重复控制器，与PID联合使用
//作用在PID之前，与实际测量值相减输入到PID中
class RepetitiveController
{
protected:
	SignalStream<float> err;
	RcFilter filter;
	float k;
	int forwardCorrect;
public:
	//重复控制器，与PID联合使用
	//作用在PID之前，与实际测量值相减输入到PID中
	//length是扰动或控制信号的周期
	//k是输出增益
	//frq、fh是输出低通滤波器的参数
	//Sz是重复控制器的补偿系统，为受控对象的逆
	RepetitiveController(int length,int forwardCorrect, float k, float frq, float fh);

	//清除历史误差
	void clear();

	//输出补偿信号
	float refresh(float feedback);

	//设置周期
	void setLength(int length);

};


#endif
