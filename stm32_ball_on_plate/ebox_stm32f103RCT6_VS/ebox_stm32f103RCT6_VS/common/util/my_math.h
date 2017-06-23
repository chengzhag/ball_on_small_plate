#ifndef __MY_MATH_H
#define __MY_MATH_H

#include "ebox.h"
#include <limits>
//#include <limits.h>
//#include <float.h>
 

//限制某个数的下界
template<typename T>
void limitLow(T &num, T limL)
{
	if (num < limL)
	{
		num = limL;
	}
}

//限制某个数的上界
template<typename T>
void limitHigh(T &num, T limH)
{
	if (num > limH)
	{
		num = limH;
	}
}


//限制某个数的上下界
template<typename T>
void limit(T &num, T limL, T limH)
{
	limitLow(num, limL);
	limitHigh(num, limH);
}

//面向对象的模仿matlab的tictoc类，单位ms
class TicToc
{
	unsigned long ticTime;
public:
	TicToc();
	//开始计时
	void tic();

	//返回从上次开始计时到现在的时间差
	unsigned long toc();

};


//帧率计数器
class FpsCounter:private TicToc
{
public:
	FpsCounter();

	//开始计时
	void begin();

	//计算帧率
	float getFps();
};






#endif