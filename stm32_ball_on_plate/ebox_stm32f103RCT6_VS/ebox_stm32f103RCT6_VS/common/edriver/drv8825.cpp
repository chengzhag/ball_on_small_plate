#include "drv8825.h"
#include "my_math.h"

void DRV8825::setFre(int frequency)
{
	limit(frequency, -maxFre, maxFre);
	pct = (float)frequency / maxFre * 100;
	if (frequency > 0)
	{
		pinDir->set();
		pwm.set_frq(frequency);
	}
	else
	{
		pinDir->reset();
		pwm.set_frq(-frequency);
	}
}

DRV8825::DRV8825(Gpio* pinStep, Gpio* pinDir, int maxFre/*=25000*/) :
	pwm(pinStep),
	pinDir(pinDir),
	pct(0)
{
	limitLow(maxFre, 0);
	this->maxFre = maxFre;
}

void DRV8825::begin()
{
	pwm.begin(0, 500);
	pinDir->mode(OUTPUT_PP);
	pinDir->reset();
}

void DRV8825::setPct(float percent)
{
	setFre(maxFre*percent / 100);
}

float DRV8825::getPct()
{
	return pct;
}

int DRV8825::getFre()
{
	return pct*maxFre / 100;
}

uint32_t DRV8825::getMaxFre()
{
	return maxFre;
}
