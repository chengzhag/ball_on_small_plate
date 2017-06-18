#include "accurate_pwm.h"
#include "stm32f10x_tim.h"

AccuratePwm::AccuratePwm(Gpio *pwm_pin) :
	duty(0),
	Pwm(pwm_pin)
{

}

void AccuratePwm::begin(uint32_t frq, uint16_t duty)
{
	this->duty = duty;
	Pwm::begin(frq, duty);
}

void AccuratePwm::set_duty(float duty)
{
	limit<float>(duty, 0, 1000);
	this->duty = duty;
	uint16_t compare = period*(duty / 1000);
	switch (ch)
	{
	case 1:
		TIM_SetCompare1(TIMx, compare);
		break;
	case 2:
		TIM_SetCompare2(TIMx, compare);
		break;
	case 3:
		TIM_SetCompare3(TIMx, compare);
		break;
	case 4:
		TIM_SetCompare4(TIMx, compare);
		break;
	default:
		break;
	}
}

void AccuratePwm::set_frq(uint32_t frq)
{
	uint16_t compare;

	if (frq != 0)
	{
		uint16_t period_old = period;
		uint32_t scaler = get_timer_source_clock() / frq;
		uint16_t psc = scaler / 65536;
		period = scaler / (psc + 1);
		compare = period*(duty / 1000);


		TIMx->PSC = psc;
		uint16_t count = (uint32_t)period*TIMx->CNT / period_old + 1;
		TIMx->EGR = TIM_PSCReloadMode_Immediate;
		TIMx->CNT = count;
		TIMx->ARR = period;
	}
	else
	{
		compare = 0;
	}


	switch (ch)
	{
	case 1:
		TIMx->CCR1 = compare;
		break;
	case 2:
		TIMx->CCR2 = compare;
		break;
	case 3:
		TIMx->CCR3 = compare;
		break;
	case 4:
		TIMx->CCR4 = compare;
		break;
	default:
		break;
	}
}
