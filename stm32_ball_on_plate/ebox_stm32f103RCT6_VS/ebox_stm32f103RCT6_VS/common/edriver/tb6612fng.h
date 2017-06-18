#ifndef  __TB6612FNG
#define  __TB6612FNG

#include "ebox.h"
#include <math.h>

//motorPinPwm需要支持PWM输出，注意查表确定timer是否被占用
class TB6612FNG
{
	Gpio *pinA;
	Gpio *pinB;
	Gpio *pinP;
	Pwm pwm;
	uint32_t frq;
	float pct;
public:
	TB6612FNG(Gpio *motorPinA, Gpio *motorPinB,
		Gpio *motorPinPwm, uint32_t pwmFrequency = 15000);

	//配置PWM和控制脚输出
	void begin();

	//设置输出百分比
	void setPercent(float p);

	//获取输出百分比
	float getPercent();
};




#endif