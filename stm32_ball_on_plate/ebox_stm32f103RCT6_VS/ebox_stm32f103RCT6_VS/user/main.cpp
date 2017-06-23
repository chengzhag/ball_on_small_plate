#include "ebox.h"
#include "led.h"
#include "servo.h"
#include "uart_num.h"
#include "PID.hpp"
#include "my_math.h"
#include "signal_stream.h"
#include "uart_vcan.h"

using namespace std;


const int filterWindow = 3;
class AverageFilter :public SignalStream<float, filterWindow>
{
public:
	float getFilterOut(float newNum)
	{
		push(newNum);
		float temp = 0;
		for (int i = 0; i < filterWindow; i++)
		{
			temp += operator[](i);
		}
		temp /= filterWindow;
		return temp;
	}
}filterX, filterY/*, filterOutX, filterOutY*/;

Servo servoY(&PB0, 100, 0.8, 2.22);
Servo servoX(&PB1, 100, 0.75, 2.2);
UartNum<int, 2> uartNum(&uart2);
sky::PID pidX, pidY;
UartVscan uartVscan(&uart1);
FpsCounter fps;
const int maxX = 128;
const int maxY = 98;

void posReceiveEvent(UartNum<int, 2>* uartNum)
{
	if (uartNum->getLength() == 2)
	{
		float posX = uartNum->getNum()[0];
		float posY = uartNum->getNum()[1];
		posX = filterX.getFilterOut(posX);
		posY = filterY.getFilterOut(posY);
		//uart1.printf("%f\t%f\r\n", posX, posY);

		if (posX != -1 && posY != -1)
		{
			float outX = 50, outY = 50;
			outX -= pidX.refresh(posX);
			outY -= pidY.refresh(posY);

			//outX = filterOutX.getFilterOut(outX);
			//outY = filterOutY.getFilterOut(outY);

			float vscan[] = { posX,posY,outX,outY,fps.getFps() };
			uartVscan.sendOscilloscope(vscan, 5);

			servoX.setPct(outX);
			servoY.setPct(outY);
		}
		else
		{
			servoX.setPct(50);
			servoY.setPct(50);
		}
	}
}

void setup()
{
	ebox_init();
	uart1.begin(115200);
	servoY.begin();
	servoX.begin();
	uartNum.begin(115200);
	uartNum.attach(posReceiveEvent);

	pidX.setRefreshRate(30);
	pidX.setWeights(0.2, 0.01, 0.08);
	pidX.setOutputLowerLimit(-numeric_limits<float>::max());
	pidX.setOutputUpperLimit(numeric_limits<float>::max());
	pidX.setDesiredPoint(maxX/2);
	pidX.setISeperateThres(50);

	pidY.setRefreshRate(30);
	pidY.setWeights(0.2, 0.01, 0.08);
	pidY.setOutputLowerLimit(-numeric_limits<float>::max());
	pidY.setOutputUpperLimit(numeric_limits<float>::max());
	pidY.setDesiredPoint(maxY / 2);
	pidY.setISeperateThres(50);
}
int main(void)
{
	setup();
	//float pct = 0, increase = 1;
	while (1)
	{
		//pct += increase;
		//if (pct >= 100 || pct <= 0)
		//{
		//	increase = -increase;
		//}

		//servoY.setPct(pct);
		//servoX.setPct(pct);

		//uart1.printf("%f\r\n", servo1.getPct());
		//delay_ms(10);
	}

}


