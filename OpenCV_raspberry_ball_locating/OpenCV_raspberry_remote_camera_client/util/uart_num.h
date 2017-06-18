#pragma once

#include <wiringSerial.h>
#include <iostream>
//注意在visualGDB选项CMake project settings中的library names后添加wiringPi
//https://visualgdb.com/tutorials/raspberry/wiringPi/

template<typename NumType = float>
	class UartNum
	{
		int fd;
	public:
		//构建基于uartX的UartNum类
		UartNum()
		{
		}
		
		~UartNum()
		{
			serialClose(fd);
		}

		//初始化uart、绑定中断函数，树莓派zero w的uart对应device = "/dev/ttyS0"
		void begin(const int baud = 115200, const char *device = "/dev/ttyS0")
		{
			if ((fd = serialOpen(device, baud)) < 0)
			{
				std::cout << "serial err\n";
				return;
			}
		}
		
		void printf(const char *fmt, ...)
		{
			serialPrintf(fd, fmt);
		}

		//发送数组
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
						serialPutchar(fd, '\\');
						k++;
					}
					serialPutchar(fd, num2char.c[j]);
					k++;
				}
			}
			serialPutchar(fd, '\n');
		}
	};