#ifndef __SIGNAL_STREAM
#define __SIGNAL_STREAM

#include "ebox.h"
#include "my_math.h"

//用circle buffer实现的试试信号处理类
//仅做信号存储，通过继承或实例化实现信号处理函数
//注意模板类的函数实现必须写在头文件中
template<typename T>
class SignalStream
{
protected:
	int indexHead;
	int length;
public:
	T* buf;

	//动态分配内存，初始化buf为0
	SignalStream(int length);

	//释放内存
	~SignalStream()
	{
		ebox_free(buf);
	}

	//清空buf为0
	void clear();

	//返回buf的长度
	int getLength();

	//压入最新的数据并弹出最旧的数据
	void push(T signal);

	//重新设置队列长度
	void setLength(int length);

	//获取最旧的数据
	float getOldest();

	//重载[]操作符，从0开始为从新到旧的数据
	T &operator [](int index);
};

template<typename T>
float SignalStream<T>::getOldest()
{
	return operator [](length - 1);
}

template<typename T>
void SignalStream<T>::setLength(int length)
{
	this->length = length;
	ebox_free(buf);
	buf = (T *)ebox_malloc(sizeof(T)*length);
	clear();
}

template<typename T>
SignalStream<T>::SignalStream(int length) :
	indexHead(0),
	length(length)
{
	buf = (T *)ebox_malloc(sizeof(T)*length);
	clear();
}

template<typename T>
void SignalStream<T>::clear()
{
	for (int i = 0; i < length; i++)
	{
		buf[i] = 0;
	}
}

template<typename T>
int SignalStream<T>::getLength()
{
	return length;
}

template<typename T>
void SignalStream<T>::push(T signal)
{
	indexHead--;
	indexHead %= length;
	if (indexHead < 0)
	{
		indexHead += length;
	}
	buf[indexHead] = signal;
}

template<typename T>
T & SignalStream<T>::operator[](int index)
{
	return buf[(index + indexHead) % length];
}

//用SignalStream实现的均值滤波类，减少运算量
class AverageFilter :public SignalStream<float>
{
protected:
	float sumTemp;
public:
	AverageFilter(float sampleFrq,float stopFrq);
	float getFilterOut(float newNum);
};

class RcFilter
{
protected:
	float k;
	float sampleFrq;
	float yOld;

public:
	RcFilter(float sampleFrq, float stopFrq);
	
	//获取滤波器输出
	float getFilterOut(float x);

	//设置采样率和截止频率
	void setParams(float sampleFrq, float stopFrq);

	//设置截止频率
	void setStopFrq(float stopFrq);

	//清除上一次输出
	void clear();
};

//z变换表示的离散系统
class SysZ
{
public:
	virtual float getY(float x) = 0;
};

//只有零点的系统
class SysWithOnlyZero:public SysZ
{
	SignalStream<float> xn;
	float *args;
	int num;
public:
	//只有零点的系统
	//args是z^0, z^-1,z^-2...的系数
	SysWithOnlyZero(float *args, int num):
		xn(num),
		args(args),
		num(num)
	{
		
	}

	//传入输入，获取输出
	float getY(float x);

	//清除系统历史数据
	void clear();
};

#endif
