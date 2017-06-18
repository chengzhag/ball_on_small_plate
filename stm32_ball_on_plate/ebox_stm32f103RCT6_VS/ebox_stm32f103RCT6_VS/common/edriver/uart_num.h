#ifndef __UART_NUM
#define __UART_NUM

#include "ebox.h"

#define __UART_NUM_DEBUG

template<typename NumType = float, int numBufSize = 3>
class UartNum
{
	Uart *uart;
	FunctionPointerArg1<void, UartNum*> numReceivedEvent;

	//用于存储接收数据位，转换为数字
	union Char2num
	{
		NumType f;
		unsigned char c[sizeof(NumType)];
	}char2num;

	float numBuf[numBufSize];//用于存储接收的数据

	int recievedLength;

	int charBufIndex;//接收数据位的index
	int numBufIndex;//接收数字的index
	bool isTrans;//标志上一次是否是转义字符
	
	bool isBegin;//标志是否处于帧头

	void rxEvent()
	{
		uint8_t c;
		c = uart->read();
		if (isBegin)
		{
			recievedLength = 0;
			charBufIndex = 0;
			numBufIndex = 0;
			isTrans = false;
			isBegin = false;
		}
		//不在反转义状态
		if (!isTrans)
		{
			if (c == '\\')
			{
				isTrans = true;
				return;
			}
			else if (c == '\n')
			{
				//当未进入反转义时，遇到'\n'即结束该帧的结束
				recievedLength = numBufIndex;
				if (recievedLength >numBufSize)
				{
#ifdef __UART_NUM_DEBUG
					printf("数字缓存溢出!!!recieved %d nums\r\n", recievedLength);
#endif
					recievedLength = numBufSize;
				}
				isBegin = true;
				numReceivedEvent.call(this);

#ifdef __UART_NUM_DEBUG
				printf("转换结果%d个：", recievedLength);
				for (int j = 0; j < recievedLength; j++)
				{
					printf("%f ", numBuf[j]);
				}
				printf("\r\n");
#endif
				return;
			}
			else
			{
				//当前位是数据位
				char2num.c[charBufIndex] = c;
				charBufIndex++;
			}
		}
		//在反转义状态
		else
		{
			//当前位是数据位
			char2num.c[charBufIndex] = c;
			charBufIndex++;
			isTrans = false;
		}

		//判断已接收数据位个数，如果单个数据接收完毕，存储数据
		if (charBufIndex >= sizeof(NumType))
		{
			if (numBufIndex < numBufSize)
			{
				numBuf[numBufIndex] = char2num.f;
			}
			numBufIndex++;
			charBufIndex = 0;
		}
	}

public:

	//构建基于uartX的UartNum类
	UartNum(Uart *uartX) :
		uart(uartX),
		isTrans(false),
		charBufIndex(0),
		numBufIndex(0),
		recievedLength(0),
		isBegin(true)
	{

	}

	//初始化uart、绑定中断函数
	void begin(uint32_t baud_rate, uint8_t data_bit, uint8_t parity, float stop_bit, uint8_t _use_dma)
	{
		uart->begin(baud_rate, data_bit, parity, stop_bit, _use_dma);
		uart->attach(this, &UartNum::rxEvent, RxIrq);
	}

	//初始化uart、绑定中断函数
	void begin(uint32_t baud_rate, uint8_t _use_dma = 1)
	{
		uart->begin(baud_rate, _use_dma);
		uart->attach(this, &UartNum::rxEvent, RxIrq);
	}

	void printf(const char *fmt, ...)
	{
		uart->printf(fmt);
	}

	void sendNum(NumType* num, int length)
	{
		union Num2Char {
			float num;
			unsigned char c[sizeof(NumType)];
		}num2char;
		int i = 0, j = 0, k = 0;
		for (i = 0; i < length; i++)
		{
			num2char.num = num[i];
			for (j = 0; j < sizeof(NumType); j++)
			{
				if (num2char.c[j] == '\n' || num2char.c[j] == '\\')
				{
					uart->wait_busy();
					uart->write('\\');
					k++;
				}
				uart->wait_busy();
				uart->write(num2char.c[j]);
				k++;
			}
		}
		uart->wait_busy();
		uart->write('\n');
	}

	//是否开始接收
	bool busy()
	{
		return !isBegin;
	}

	void waitBusy()
	{
		while (busy());
	}

	NumType* getNum()
	{
		return numBuf;
	}

	int getLength()
	{
		return recievedLength;
	}

	//绑定数据处理函数
	void attach(void(*numReceivedEvent)(UartNum<NumType, numBufSize>*))
	{
		this->numReceivedEvent.attach(numReceivedEvent);
	}

	//绑定数据处理成员函数
	template<typename T>
	void attach(T *pObj, void (T::*numReceivedEvent)(UartNum<NumType,numBufSize>*))
	{
		this->numReceivedEvent.attach(pObj, numReceivedEvent);
	}
};

#endif