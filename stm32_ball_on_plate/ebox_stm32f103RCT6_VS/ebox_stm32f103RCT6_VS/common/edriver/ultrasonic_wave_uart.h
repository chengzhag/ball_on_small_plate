#ifndef __ULRRASONIC_WAVE_UART
#define __ULRRASONIC_WAVE_UART

#include "ebox.h"
#include "FunctionPointer.h"


//基础超声波模块驱动
class UltrasonicWaveUart
{
    Uart *uart;
    uint16_t dis;
    bool isHigh;
    bool isReady;

	//uart字节处理中断函数
	void rxEvent();

	FunctionPointerArg1<void,uint16_t> dataRecieveEvent;

public:

    //构造函数，确定Uart
    UltrasonicWaveUart(Uart *uartX);

    //初始化函数，初始化uart口
    void begin();

	//发送触发指令，开始测距
	void trig();

    //基于忙等的read函数
    uint16_t read();

	//绑定数据接收中断函数
	//如果未绑定，则只能使用read函数
	void attach(void(*dataRecieveEvent)(uint16_t));

	//绑定数据接收中断函数
	//如果未绑定，则只能使用read函数
	template<typename T>
	void attach(T *pObj, void (T::*classDataRecieveEvent)(uint16_t));
};

template<typename T>
void UltrasonicWaveUart::attach(T *pObj, void (T::*classDataRecieveEvent)(uint16_t))
{
	this->dataRecieveEvent.attach(pObj, classDataRecieveEvent);
}

#endif