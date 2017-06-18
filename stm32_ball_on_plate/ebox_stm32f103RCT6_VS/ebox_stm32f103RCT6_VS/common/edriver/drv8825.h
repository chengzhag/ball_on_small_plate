#ifndef  __DRV8825_H
#define  __DRV8825_H
#include "ebox.h"
#include "accurate_pwm.h"


class DRV8825
{
	AccuratePwm pwm;
	Gpio* pinDir;
	float pct;
	int maxFre;

	//设置输出频率，负则反向，限制在maxFre范围内
	void setFre(int frequency);

public:
	//DRV8825步进电机驱动
	//pinStep：输出到STEP脚，必须为pwm口
	//pinDir：输出到DIR脚
	DRV8825(Gpio* pinStep,Gpio* pinDir, int maxFre=25000);

	//初始化pwm输出和dir输出
	void begin();

	//设置输出百分比
	void setPct(float percent);
	
	//获取百分比
	float getPct();

	//获取频率
	int getFre();

	//获取最大频率
	uint32_t getMaxFre();
};


#endif
