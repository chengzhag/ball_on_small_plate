#include "signal_stream.h"



AverageFilter::AverageFilter(float sampleFrq, float stopFrq) :
	SignalStream(int(0.443*sampleFrq / stopFrq + 1))
{

}

float AverageFilter::getFilterOut(float newNum)
{
	sumTemp = sumTemp + newNum
		- SignalStream<float>::operator[](length - 1);
	SignalStream<float>::push(newNum);
	return sumTemp / length;
}


RcFilter::RcFilter(float sampleFrq, float stopFrq):
		yOld(0)
{
	setParams(sampleFrq, stopFrq);
}

float RcFilter::getFilterOut(float x)
{
	float y = yOld*k + (1 - k)*x;
	yOld = y;
	return y;
}

void RcFilter::setParams(float sampleFrq, float stopFrq)
{
	this->sampleFrq = sampleFrq;
	setStopFrq(stopFrq);
}

void RcFilter::setStopFrq(float stopFrq)
{
	k = 1 / (1 + (2 * M_PI *stopFrq / sampleFrq));
}

void RcFilter::clear()
{
	yOld = 0;
}


float SysWithOnlyZero::getY(float x)
{
	float y = 0;
	xn.push(x);
	for (int i = 0; i < num; i++)
	{
		y += args[i] * xn[i];
	}
	return y;
}

void SysWithOnlyZero::clear()
{
	xn.clear();
}


