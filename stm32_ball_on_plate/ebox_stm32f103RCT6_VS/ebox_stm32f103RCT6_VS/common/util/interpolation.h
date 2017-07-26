
#ifndef  INTERPOLATION_H
#define  INTERPOLATION_H
#include"ebox.h"
class Interpolation
{
public:
	//构造函数，传入样本点参数：恒做坐标首地址，数组长度
	Interpolation(float *x, float *y, int n);
	//更改样本点参数：恒做坐标首地址，数组长度
	void setSamplePoint(float *x, float *y, int n);
	//用二分查找查找输入x的区间下限,返回坐标
	int search(float x);
	//纯虚函数，获取插值后任意坐标下的值
	virtual float getY(float x) = 0;
	//获取measureY
private:
	float *xaxis;
	float *yaxis;
	int length;

};


class LinearInterpolation :public Interpolation
{
public:
	LinearInterpolation(float *x, float *y, int n);
	float getY(float x);

private:
	float *xaxis;
	float *yaxis;
	int length;
};



class QuadraticInterpolation :public Interpolation
{
public:
	//构造函数
	QuadraticInterpolation(float *x, float *y, int n);
	//具体基类纯虚函数
	float getY(float x);

private:
	float *xaxis;
	float *yaxis;
	int length;

};


class Interpolation2D
{
protected:
	float *x, *y, *z;
	int lengthX, lengthY;
public:
	Interpolation2D(float *x, float *y, float *z, int lengthX, int lengthY);

	void setSamplePoint(float *x, float *y, float *z, int lengthX, int lengthY);

	//寻找插值点的X轴和Y轴的偏移量
	int search1D(float *xaxis, float x, int length);

	//寻找插值点Z值
	float getZ(float x, float y);

	//获取x的取值范围
	void getXLim(float *limL, float *limH)
	{
		*limL = x[0];
		*limH = x[lengthX - 1];
	}

	//获取y的取值范围
	void getYLim(float *limL, float *limH)
	{
		*limL = y[0];
		*limH = y[lengthY - 1];
	}
};

#endif