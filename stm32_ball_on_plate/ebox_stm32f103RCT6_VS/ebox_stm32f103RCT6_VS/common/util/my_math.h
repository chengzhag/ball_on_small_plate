#ifndef __MY_MATH_H
#define __MY_MATH_H

#include "ebox.h"
#include <limits>
#include <math.h>
#define M_PI		3.14159265358979323846
//#include <limits.h>
//#include <float.h>
 
//设置一个二进制数中的某位或区间
//dst: 目标二进制数
//bits: 设置值，右对齐
//high: 设置区间高位
//low: 设置区间低位
template<typename T>
void replaceBits(T& dst, T bits, uint8_t high, uint8_t low)
{
	T mask = 1;//例：high=4,low=2,bits='b0000_0011,dst='b1011_0100,mask='b0000_0001
	mask <<= (high - low + 1);//mask='b0000_1000
	mask -= 1;//mask='b0000_0111
	mask <<= low;//mask='b0001_1100
	bits <<= low;//bits='b0000_1100
	bits &= mask;//确保bits的其他位为0
	mask = ~mask;//mask='b1110_0011
	dst &= mask;//清除dst对应位，dst='b1010_0000
	dst |= bits;//设置dst的对应位，dst='b1010_1100
}

//限制某个数的下界
template<typename T>
void limitLow(T &num, T limL)
{
	if (num < limL)
	{
		num = limL;
	}
}

//限制某个数的上界
template<typename T>
void limitHigh(T &num, T limH)
{
	if (num > limH)
	{
		num = limH;
	}
}


//限制某个数的上下界
template<typename T>
void limit(T &num, T limL, T limH)
{
	limitLow(num, limL);
	limitHigh(num, limH);
}

//面向对象的模仿matlab的tictoc类，单位ms
class TicToc
{
	unsigned long ticTime;
public:
	TicToc();
	//开始计时
	void tic();

	//返回从上次开始计时到现在的时间差
	unsigned long toc();

};


//帧率计数器
class FpsCounter:private TicToc
{
public:
	FpsCounter();

	//开始计时
	void begin();

	//计算帧率
	float getFps();
};

//二维插值基类
class Interpolation2D
{
protected:
	float *x, *y, *z;
	int lengthX, lengthY;
	float *search1D(float *xaxis,float x,int length)
	{
		int low = 0, high = length - 1;
		int mid = 0;
		while (1)
		{
			if (x < *(xaxis))  //如果x比最小值小
			{
				return xaxis + 1;
			}
			else if (x > *(xaxis + length - 1)) //如果x比最大值大
			{

				return xaxis + length - 1;
			}
			else
			{
				mid = (low + high) / 2;
				if (*(xaxis + mid) > x)
				{
					high = mid - 1;
					if (*(xaxis + high) < x)
					{
						return xaxis + mid;
						break;
					}
				}
				else if (*(xaxis + mid) < x)
				{
					low = mid + 1;
					if (*(xaxis + low) > x)
					{
						return xaxis + low;
						break;
					}
				}
				else
				{
					return xaxis + mid + 1;
				}
			}
		}
	}
public:
	//x、y是递增的一维数组，对应z的横纵坐标
	//z是lengthX*lengthY长度的一维数组
	Interpolation2D(float *x, float *y, float *z, int lengthX, int lengthY) :
		x(x), y(y), z(z),
		lengthX(lengthX), lengthY(lengthY)
	{

	}

	void setSamplePoint(float *x, float *y, float *z, int lengthX, int lengthY)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->lengthX = lengthX;
		this->lengthY = lengthY;
	}

	//返回曲面中[x,y]左下方的点
	float *search(float x, float y)
	{
		float *xp = search1D(this->x, x, lengthX);
		float *yp = search1D(this->x, x, lengthY);
		float *zp = z + (xp - this->x) + (yp - this->y)*lengthX;
		return zp;
	}
};






#endif