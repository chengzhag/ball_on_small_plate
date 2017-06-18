#ifndef  __ENCODER_EXTI
#define  __ENCODER_EXTI

#include "ebox.h"

class EncoderExti
{
	long pos;
	long posOld;
	long diff;
	Gpio *pinA;
	Gpio *pinB;
	Exti extiA;
	Exti extiB;
	void eventA();
	void eventB();
public:

	//初始化正交编码器，Apin、Bpin分别为编码器的A、B相
	EncoderExti(Gpio *encoderPinA, Gpio *encoderPinB);

	//开始配置ebox选项
	void begin();

	//获取位置
	long getPos();

	//重置位置为0
	void resetPos();

	//计算与上次位置的差值
	void refreshDiff();

	//获取计算的差值
	long getDiff();
};




#endif