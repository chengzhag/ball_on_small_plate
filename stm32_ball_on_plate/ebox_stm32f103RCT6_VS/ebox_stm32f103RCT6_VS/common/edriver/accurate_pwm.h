#ifndef __ACCURATE_PWM_H
#define __ACCURATE_PWM_H

#include "ebox.h"
#include "my_math.h"

class AccuratePwm :public Pwm
{
	float duty;
	uint16_t counter;
public:
	AccuratePwm(Gpio *pwm_pin);

	void begin(uint32_t frq, uint16_t duty);

	//以1000为满占空比
	//以浮点类型保证最高的精度
	void set_duty(float duty);

	//直接对寄存器进行操作
	//并且在更改预分频系数前暂存计数器的值
	//解决了ebox_pwm在改变频率时相位变化的问题
	void set_frq(uint32_t frq);
};

#endif
