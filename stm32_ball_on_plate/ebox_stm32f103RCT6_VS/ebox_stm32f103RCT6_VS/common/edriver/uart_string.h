#ifndef __UART_STRING
#define __UART_STRING

#include "ebox.h"

#define UART_STRING_BUFFER_SIZE 100
class UartString
{
	Uart *uart;
	uint16_t bufferIndex;
	char buffer[UART_STRING_BUFFER_SIZE];

	//串口收到字节中断处理函数
	void rxEvent();
	//字符串接收完成处理函数
	FunctionPointerArg1<void,char*> stringEvent;
public:
	//构建基于uartX的UartString类
	UartString(Uart *uartX);
	//初始化uart、绑定中断函数
	void begin(uint32_t baud_rate, uint8_t _use_dma = 1);
	//初始化uart、绑定中断函数
	void begin(uint32_t baud_rate, uint8_t data_bit, uint8_t parity, float stop_bit, uint8_t _use_dma);

	void printf(const char *fmt, ...);
	
	//绑定字符串处理函数
	void attach(void(*stringEvent)(char *str));
	//绑定字符串处理成员函数
	template<typename T>
	void attach(T *pObj, void (T::*classStringEvent)(char *str));
};

template<typename T>
void UartString::attach(T *pObj, void (T::*classStringEvent)(char *str))
{
	this->stringEvent.attach(pObj, classStringEvent);
}


#endif