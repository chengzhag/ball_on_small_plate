#include "interpolation.h"
Interpolation::Interpolation(float *x, float *y, int n)
{
	this->xaxis = x;
	this->yaxis = y;
	this->length = n;

}

void Interpolation::setSamplePoint(float *x, float *y, int n)
{
	this->xaxis = x;
	this->yaxis = y;
	this->length = n;
}

float *Interpolation::search(float x)
{
	float *x1 = xaxis;
	float *y1 = yaxis;
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


LinearInterpolation::LinearInterpolation(float *x, float *y, int n) :Interpolation(x, y, n)
{
	this->xaxis = x;
	this->yaxis = y;
	this->length = n;
}

float LinearInterpolation::getY(float x)
{
	float *x1 = xaxis;
	float *y1 = yaxis;
	float y2, y3;
	float x2, x3;
	float k;
	float y;
	int j = 0;

	x1 = search(x);
	j = x1 - xaxis;
	y1 = y1 + j;
	x3 = *x1--; x2 = *x1;
	y3 = *y1--; y2 = *y1;
	k = (y3 - y2) / (x3 - x2);
	y = y2 + k*(x - x2);
	return y;
}



QuadraticInterpolation::QuadraticInterpolation(float *x, float *y, int n) :Interpolation(x, y, n)
{
	this->xaxis = x;
	this->yaxis = y;
	this->length = n;
}

float QuadraticInterpolation::getY(float x)
{
	float *x1 = xaxis;
	float *y1 = yaxis;
	float arrY[3];
	float arrX[3];
	float y;
	int j = 0; //小于X的数最大横坐标标号

	x1 = search(x);
	j = x1 - xaxis;
	y1 = y1 + j;
	if (j < length - 2)//x在区间范围内,且下一个数不是边界
	{
		arrX[2] = *(++x1);
		arrX[1] = *(--x1);
		arrX[0] = *(--x1);
		arrY[2] = *(++y1);
		arrY[1] = *(--y1);
		arrY[0] = *(--y1);
	}
	else if (j >= length - 2) //x在区间范围内,且下一个数是上界或就是上界
	{
		arrX[2] = *(x1--);
		arrX[1] = *(x1--);
		arrX[0] = *(x1);
		arrY[2] = *(y1--);
		arrY[1] = *(y1--);
		arrY[0] = *(y1);
	}

	//定义式
	float l0 = (x - arrX[1])*(x - arrX[2]) / ((arrX[0] - arrX[1])*(arrX[0] - arrX[2]));
	float l1 = (x - arrX[0])*(x - arrX[2]) / ((arrX[1] - arrX[0])*(arrX[1] - arrX[2]));
	float l2 = (x - arrX[0])*(x - arrX[1]) / ((arrX[2] - arrX[0])*(arrX[2] - arrX[1]));
	y = arrY[0] * l0 + arrY[1] * l1 + arrY[2] * l2;
	return y;
}
