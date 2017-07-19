#include "signal_stream.h"
#include "my_math.h"


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
	sampleFrq(sampleFrq),
	k(1 / (1 + (2 * M_PI *stopFrq / sampleFrq))),
		yOld(0)
{

}

float RcFilter::getFilterOut(float x)
{
	float y = yOld*k + (1 - k)*x;
	yOld = y;
	return y;
}

