#ifndef __SIGNAL_STREAM
#define __SIGNAL_STREAM

#include "ebox.h"


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

	//重载[]操作符，从0开始为从新到旧的数据
	T &operator [](int index);
};

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
public:
	RcFilter(float sampleFrq, float stopFrq);
	float getFilterOut(float x);
private:
	float k;
	float sampleFrq;
	float yOld;
};

#endif
