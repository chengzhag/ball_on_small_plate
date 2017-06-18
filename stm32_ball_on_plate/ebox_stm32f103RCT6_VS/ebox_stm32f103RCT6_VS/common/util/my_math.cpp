#include "my_math.h"

TicToc::TicToc() :
	ticTime(0)
{

}

void TicToc::tic()
{
	ticTime = millis();
}

unsigned long TicToc::toc()
{
	return millis() - ticTime;
}

FpsCounter::FpsCounter()
{

}

void FpsCounter::begin()
{
	tic();
}

float FpsCounter::getFps()
{
	float fps = 1000.0f / toc();
	tic();
	return fps;
}
