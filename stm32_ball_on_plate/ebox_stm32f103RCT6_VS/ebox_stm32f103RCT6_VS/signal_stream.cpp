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


Butterworth::Butterworth(float sampleFrq, float stopFrq)
{
	this->sampleFrq = sampleFrq;
	this->stopFrq = (2 * stopFrq / sampleFrq);
	this->tempY = 0;

}

float Butterworth::getFilterOut(float x)
{
	float y = tempY*(1 - stopFrq) + stopFrq*x;
	tempY = y;
	return y;
}

