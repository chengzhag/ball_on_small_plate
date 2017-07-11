#include "ebox.h"
#include "led.h"
#include "servo.h"
#include "uart_num.h"
#include "PID.hpp"
#include "my_math.h"
#include "signal_stream.h"
#include "uart_vcan.h"
#include "Button.h"
#include "led.h"
#include "oled_i2c.h"
#include <math.h>

using namespace std;


AverageFilter<3> filterX, filterY, filterOutX, filterOutY;

Servo servoY(&PB0, 100, 0.8, 2.16);
Servo servoX(&PB1, 100, 0.8, 2.24);
UartNum<int, 2> uartNum(&uart2);
sky::PID pidX, pidY;
UartVscan uartVscan(&uart1);
FpsCounter fps;
const int maxX = 134;
const int maxY = 134;
float posX = -1;
float posY = -1;

void posReceiveEvent(UartNum<int, 2>* uartNum)
{
	if (uartNum->getLength() == 2)
	{
		posX = uartNum->getNum()[0];
		posY = uartNum->getNum()[1];
		posX = filterX.getFilterOut(posX);
		posY = filterY.getFilterOut(posY);
		//uart1.printf("%f\t%f\r\n", posX, posY);

		if (posX != -1 && posY != -1)
		{
			float outX = 0, outY = 0;
			outX -= pidX.refresh(posX);
			outY += pidY.refresh(posY);

			outX = filterOutX.getFilterOut(outX);
			outY = filterOutY.getFilterOut(outY);

			float vscan[] = { posX,posY,outX,outY,fps.getFps() };
			uartVscan.sendOscilloscope(vscan, 5);

			servoX.setPct(outX);
			servoY.setPct(outY);
		}
		else
		{
			pidX.reset();
			pidY.reset();
			servoX.setPct(0);
			servoY.setPct(0);
		}
	}
}

Button keyL(&PC12, 1);
Button keyR(&PB5, 1);
Button keyU(&PB3, 1);
Button keyD(&PB4, 1);
Led led(&PD2, 1);
OLEDI2C oled(&i2c1);
const float factorPID = 1.5;

void setup()
{
	ebox_init();
	uart1.begin(115200);
	servoY.begin();
	servoX.begin();
	uartNum.begin(115200);
	uartNum.attach(posReceiveEvent);

	pidX.setRefreshRate(30);
	//pidX.setWeights(0.2, 0.2, 0.12);
	//pidX.setWeights(0.18, 0.15, 0.15);
	pidX.setWeights(0.15*factorPID, 0.03*factorPID, 0.15*factorPID);
	pidX.setOutputLowerLimit(-numeric_limits<float>::max());
	pidX.setOutputUpperLimit(numeric_limits<float>::max());
	pidX.setDesiredPoint(maxX / 2);
	pidX.setISeperateThres(numeric_limits<float>::max());

	pidY.setRefreshRate(30);
	//pidY.setWeights(0.2, 0.2, 0.12);
	//pidY.setWeights(0.18, 0.15, 0.15);
	pidY.setWeights(0.15*factorPID, 0.03*factorPID, 0.15*factorPID);
	pidY.setOutputLowerLimit(-numeric_limits<float>::max());
	pidY.setOutputUpperLimit(numeric_limits<float>::max());
	pidY.setDesiredPoint(maxY / 2);
	pidY.setISeperateThres(numeric_limits<float>::max());

	keyD.begin();
	keyL.begin();
	keyR.begin();
	keyU.begin();
	led.begin();
	oled.begin();
}
int main(void)
{
	setup();
	//float pct = 0, increase = 1;
	int index = 0;
	int targetX = maxX / 2, targetY = maxY / 2;
	int circleR = 0;
	float theta = 0;
	while (1)
	{
		keyL.loop();
		keyR.loop();
		keyU.loop();
		keyD.loop();
		
		if (keyR.click())
		{
			index++;
		}
		if (keyL.click())
		{
			index--;
		}
		limit<int>(index, 0, 2);

		int increase = 0;
		if (keyU.click())
		{
			increase++;
		}
		if (keyD.click())
		{
			increase--;
		}
		if (keyU.pressed_for(200, 0))
		{
			increase += 2;
		}
		if (keyD.pressed_for(200, 0))
		{
			increase -= 2;
		}

		switch (index)
		{
		case 0:
			targetX += increase;
			limit<int>(targetX, 30, maxX-30);
			oled.printf(0, 0, 2, "*%d %d %d      ", targetX, targetY, circleR);
			break;
		case 1:
			targetY += increase;
			limit<int>(targetY, 30, maxY - 30);
			oled.printf(0, 0, 2, "%d *%d %d       ", targetX, targetY, circleR);
			break;
		case 2:
			circleR = circleR + increase;
			limit<int>(circleR, 0, (maxY - 60) / 2);
			theta += 2 * PI / 50 * 1.5;//0.5È¦Ò»Ãë
			targetX = maxX / 2 + circleR*sin(theta);
			targetY = maxY / 2 + circleR*cos(theta);
			oled.printf(0, 0, 2, "%d %d *%d       ", targetX, targetY, circleR);
			break;
		default:
			break;
		}

		oled.printf(0, 2, 2, "%d %d       ", (int)posX, (int)posY);
		pidX.setDesiredPoint(targetX);
		pidY.setDesiredPoint(targetY);

		//pct += increase;
		//if (pct >= 100 || pct <= 0)
		//{
		//	increase = -increase;
		//}

		//servoY.setPct(pct);
		//servoX.setPct(pct);

		//uart1.printf("%f\r\n", servo1.getPct());
		delay_ms(20);
	}

}


