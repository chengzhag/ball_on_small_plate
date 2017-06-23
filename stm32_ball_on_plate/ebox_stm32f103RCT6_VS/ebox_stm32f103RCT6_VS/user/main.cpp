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

Button keyL(&PC12, 1);
Button keyR(&PB5, 1);
Button keyU(&PB3, 1);
Button keyD(&PB4, 1);
Led led(&PD2, 1);
OLEDI2C oled(&i2c2);

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
	char index = 0;
	unsigned char targetX = maxX / 2, targetY = maxY / 2;
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
		limit<char>(index, 0, 1);

		char increase = 0;
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
			limit<uint8_t>(targetX, 10, maxX-10);
			oled.printf(0, 0, 2, "*%u %u       ", targetX, targetY);
			break;
		case 1:
			targetY += increase;
			limit<uint8_t>(targetY, 10, maxY - 10);
			oled.printf(0, 0, 2, "%u *%u       ", targetX, targetY);
			break;
		default:
			break;
		}

		oled.printf(0, 2, 2, "%d %d       ", targetX, targetY);
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


