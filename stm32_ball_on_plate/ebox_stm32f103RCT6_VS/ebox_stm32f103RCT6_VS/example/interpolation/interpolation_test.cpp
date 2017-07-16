#include "ebox.h"
#include "my_math.h"
#include "uart_vcan.h"
#include "interpolation.h"
//#define LinearInterpolationDebug
#define  QuadraticInterpolationDebug
#ifdef LinearInterpolationDebug
float x[5] = { 1,2,3,4,5 };
float y[5] = { 5,3,2,4,5 };
int length = 5;
FpsCounter fps;
LinearInterpolation liner(x, y, length);
#endif

#ifdef QuadraticInterpolationDebug
float x[10] = { 1.1,2.1,3.1,4.1,5.1,6.1,7.1,8.1,9.1,13 };
float y[10] = { 18,19,20,7,4,5,7,8,3,1 };
int length = 10;
//float x[3] = { 4,5,6 };
//float y[3] = { 10,5.25,1 };
//int length = 3;
FpsCounter fps;
QuadraticInterpolation Quadratic(x, y, length);
#endif

UartVscan vscan(&uart1);

void setup()
{
	ebox_init();
	uart1.begin(115200);
	vscan.begin(115200);
	PA8.mode(OUTPUT_PP);
}

#ifdef LinearInterpolationDebug
int main()
{
	float y;

	setup();

	//验证合理外推
	float x = -1;
	for (int i = 0; i < 100; i++)
	{
		y = liner.getY(x);
		x += 0.1;
		uart1.printf("%.3f\r\n", y);
		delay_ms(1);
	}
	PA8.toggle();

	while (1)
	{
	}
	return 1;

}
#endif

#ifdef QuadraticInterpolationDebug
int main()
{
	float y;
	setup();

	//验证样本点
	//float t=1.1
	//for (int i = 0; i < 10; i++)
	//{
	//	y = Quadratic.getY(t);
	//	t = t + 1;
	//	uart1.printf("%.5f\r\n", y);
	//}


	//验证插值结果
	//float t = 1.1;
	//for (int i = 1; i < 200; i++)
	//{
	//	t += 0.05;
	//	y = Quadratic.getY(t);
	//	uart1.printf("%.5f\r\n", y);
	//}
	//PA8.toggle();


	//验证边界之外的插值结果
	float t = 0.1;
	for (int i = 1; i < 120; i++)
	{
		t += 0.2;
		y = Quadratic.getY(t);
		uart1.printf("%.5f\r\n", y);
		delay_ms(1);
	}
	PA8.toggle();
	while (1)
	{
	}

	return 0;

}

#endif