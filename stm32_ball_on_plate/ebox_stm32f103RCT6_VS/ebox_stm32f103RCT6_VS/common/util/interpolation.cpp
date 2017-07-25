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

int Interpolation::search(float x)
{
	float *x1 = xaxis;
	float *y1 = yaxis;
	//	float *x2 = x+4;          对数组指针做加法 等同于数组标号上直接相加
	int low = 0, high = length - 1;
	int mid = 0;
	while (1)
	{
		if (x < *(xaxis))  //如果x比最小值小
		{

			return  -1;
		}

		else if (x > *(xaxis + length - 1)) //如果x比最大值大
		{

			return  length - 2;
		}

		else
		{
			mid = (low + high) / 2;
			if (*(xaxis + mid) > x)
			{
				high = mid - 1;
				if (*(xaxis + high) < x)
				{

					return  high;
					break;
				}
			}
			else if (*(xaxis + mid) < x)
			{
				low = mid + 1;
				if (*(xaxis + low) > x)
				{

					return  mid;
					break;
				}
			}
			else
			{
				return  mid - 1;

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
	//while ( *(++x1)<x)
	//{
	//	j++;
	//	y1++;
	//}
	j = search(x);
	if (j == -1)   //越下界，取最小值
	{
		y1 = yaxis;
		x1 = xaxis;
	}
	else
	{
		y1 = yaxis + j;
		x1 = xaxis + j;
	}

	x2 = *x1++; x3 = *x1;
	y2 = *y1++; y3 = *y1;
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
			   //找到x区间
			   //while (*(++x1) < x)
			   //{
			   //	j++;
			   //	y1++;
			   //}
	j = search(x);
	if (j <= 0)    //越下界或者取最小值，取第二个值
	{
		x1 = xaxis + 1;
		y1 = yaxis + 1;
	}
	else
	{
		x1 = xaxis + j;
		y1 = yaxis + j;
	}
	arrX[2] = *(++x1);
	arrX[1] = *(--x1);
	arrX[0] = *(--x1);
	arrY[2] = *(++y1);
	arrY[1] = *(--y1);
	arrY[0] = *(--y1);
	//百度例程
	//for (int k = 0; k < 3; k++)
	//{
	//	double t = 1;
	//	for (int j = 0; j < 3; j++)
	//	{
	//		if (k != j)
	//		{
	//			t = t*((x - arrX[j]) / (arrX[k] - arrX[j]));
	//		}
	//	}
	//	y = y + arrY[k] * t;
	//}
	//定义式
	float l0 = (x - arrX[1])*(x - arrX[2]) / ((arrX[0] - arrX[1])*(arrX[0] - arrX[2]));
	float l1 = (x - arrX[0])*(x - arrX[2]) / ((arrX[1] - arrX[0])*(arrX[1] - arrX[2]));
	float l2 = (x - arrX[0])*(x - arrX[1]) / ((arrX[2] - arrX[0])*(arrX[2] - arrX[1]));
	y = arrY[0] * l0 + arrY[1] * l1 + arrY[2] * l2;
	return y;
}

Interpolation2D::Interpolation2D(float *x, float *y, float *z, int lengthX, int lengthY) :
	x(x), y(y), z(z),
	lengthX(lengthX), lengthY(lengthY)
{

}
void Interpolation2D::setSamplePoint(float *x, float *y, float *z, int lengthX, int lengthY)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->lengthX = lengthX;
	this->lengthY = lengthY;
}

int Interpolation2D::search1D(float *xaxis, float x, int length)
{
	float *x1 = xaxis;
	int low = 0, high = length - 1;
	int mid = 0;
	while (1)
	{
		if (x < *(xaxis))  //如果x比最小值小
		{

			return  -1;
		}

		else if (x > *(xaxis + length - 1)) //如果x比最大值大
		{

			return  length - 2;
		}

		else
		{
			mid = (low + high) / 2;
			if (*(xaxis + mid) > x)
			{
				high = mid - 1;
				if (*(xaxis + high) < x)
				{

					return  high;
					break;
				}
			}
			else if (*(xaxis + mid) < x)
			{
				low = mid + 1;
				if (*(xaxis + low) > x)
				{

					return  mid;
					break;
				}
			}
			else
			{
				return  mid - 1;

			}
		}

	}
}
float Interpolation2D::getZ(float x, float y)
{
	//基于X核Y偏移量求出插值点的周围四个点横纵坐标向量组
	float *pointX[2] = { 0 };
	float *pointY[2] = { 0 };
	float *z00, *z01, *z10, *z11;
	int xp = search1D(this->x, x, lengthX);
	int yp = search1D(this->y, y, lengthY);
	if (xp == -1)   //越界情况处理
		xp = 0;
	pointX[0] = (this->x + xp);
	pointX[1] = pointX[0] + 1;
	if (yp == -1)
		yp = 0;
	pointY[0] = (this->y + yp);
	pointY[1] = pointY[0] + 1;

	z00 = z + xp + yp*lengthX;
	z10 = z00 + 1;
	z01 = z00 + lengthX;
	z11 = z01 + 1;
	float k1 = (*z10 - *z00) / (*pointX[1] - *pointX[0]);
	float x0 = k1*(x - *pointX[0]) + *z00;
	float k2 = (*z11 - *z01) / (*pointX[1] - *pointX[0]);
	float x1 = k2*(x - *pointX[0]) + *z01;
	float k3 = (x1 - x0) / (*pointY[1] - *pointY[0]);
	float x2 = k3*(y - *pointY[0]) + x0;
	return x2;
}
