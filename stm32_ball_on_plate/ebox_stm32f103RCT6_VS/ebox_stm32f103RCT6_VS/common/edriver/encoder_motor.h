#ifndef  __ENCODER_MOTOR
#define  __ENCODER_MOTOR

#include "ebox.h"
#include "PID.hpp"
#include "encoder_timer.h"
#include "tb6612fng.h"

typedef enum
{
	Encoder_Motor_Target_Position,
	Encoder_Motor_Target_Speed,
	Encoder_Motor_PID_Disabled
}Encoder_Motor_Target_Typedef;


//未重映射定时器ch1和ch2的情况下
//- TIM1 : PA8 PA9
//- TIM2 : PA0 PA1
//- TIM3 : PA6 PA7
//- TIM4 : PB6 PB7
class EncoderMotor
{
	EncoderTimer encoder;
	TB6612FNG driver;
	int mode;
	float percent;
	float refreshInt;
public:
	greg::PID pid;

	EncoderMotor(TIM_TypeDef *TIMx,
		Gpio *motorPinA, Gpio *motorPinB, Gpio *motorPinPwm,
		Encoder_Motor_Target_Typedef controlTarget = Encoder_Motor_Target_Position,
		float refreshInterval = 0.005);

	//配置编码器和驱动
	void begin();

	//配置编码器和驱动，并设置PID
	void begin(const float &Kp, const float &Ki, const float &Kd);

	//刷新编码器、计算PID并输出
	void refresh();

	//获取位置
	long getPos();

	//获取速度
	short getPosDiff();

	//获取输出百分比
	float getPct();

	//设置目标位置（位置控制模式）
	void setPos(long pos);

	//设置目标位置增量（位置控制模式）
	void setPosDiff(short pos);

	//设置目标速度（速度控制模式）
	void setSpd(short spd);

	//设置输出百分比（PID失能模式）
	void setPct(float p);
};

#endif
