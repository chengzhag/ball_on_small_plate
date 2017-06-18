#ifndef __UART_VCAN_H
#define __UART_VCAN_H

#include "ebox.h"

//配合山外多功能调试助手的串口发送类
//支持线性CCD、灰度图像、单/多条曲线
class UartVscan
{
	uint8_t cmd;
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
	T sendOscilloscope(T x)
	{
		cmd = 3;
		uart->write(cmd);
		uart->write(~cmd);
		union
		{
			T f;
			uint8_t c[sizeof(f)];
		}dataBuf;
		dataBuf.f = x;
		for (int i = 0; i < sizeof(x); i++)
			uart->write(dataBuf.c[i]);
		uart->write(~cmd);
		uart->write(cmd);
	}

	//虚拟示波器，多曲线
	template<typename T>
	T sendOscilloscope(T *y,int n)
	{
		cmd = 3;
		uart->write(cmd);
		uart->write(~cmd);
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
		uart->write(~cmd);
		uart->write(cmd);
	}
};

#endif
