#ifndef __UART_VCAN_H
#define __UART_VCAN_H

#include "ebox.h"

//配合山外多功能调试助手的串口发送类
//支持线性CCD、灰度图像、单/多条曲线
class UartVscan
{
	Uart *uart;	
public:
	UartVscan(Uart *uartX);
	//初始化
	void begin(uint32_t baud_rate, uint8_t data_bit, uint8_t parity, float stop_bit, uint8_t _use_dma);
	void begin(uint32_t baud_rate, uint8_t _use_dma = 1);

	//发送灰度图像
	void sendGreyImage(uint8_t* imgaddr, uint32_t width,uint32_t height);

	//发送线性ccd
	void sendLinearCCD(uint8_t  *ccdaddr,int ccdsize);
	void sendLinearCCD(uint8_t  *ccdaddr, uint8_t *ccdaddr2,int ccdsize);


	//虚拟示波器，单曲线
	template<typename T>
	void sendOscilloscope(T x)
	{
		uart->write(3);
		uart->write(~3);
		union
		{
			T f;
			uint8_t c[sizeof(f)];
		}dataBuf;
		dataBuf.f = x;
		for (int i = 0; i < sizeof(x); i++)
			uart->write(dataBuf.c[i]);
		uart->write(~3);
		uart->write(3);
	}

	//虚拟示波器，多曲线
	template<typename T>
	void sendOscilloscope(T *y,int n)
	{
		uart->write(3);
		uart->write(~3);
		union
		{
			T f;
			uint8_t c[sizeof(f)];
		}dataBuf;
		for (int m = 0; m < n; m++)
		{
			dataBuf.f = y[m];
			for (int i = 0; i < sizeof(dataBuf.f); i++)
				uart->write(dataBuf.c[i]);
		}
		uart->write(~3);
		uart->write(3);
	}
};

#endif
