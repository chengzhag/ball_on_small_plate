#include "ebox.h"

//调试
#include "led.h"
#include "my_math.h"
#include "uart_vcan.h"
//PID
#include "PID.h"
#include "signal_stream.h"
#include <math.h>
//动力
#include "servo.h"
//定位
#include "uart_num.h"
//底座
#include "mpu9250.h"
//交互
#include "Button.h"
#include "oled_i2c.h"
//操作系统
#include "freertos.h"
#include "task.h"
#include "queue.h"
//照明
#include "ws2812.h"




using namespace std;

//调试
UartVscan uartVscan(&uart1);
FpsCounter fpsPID, fpsUI, fpsMPU;
float fpsPIDtemp, fpsUItemp, fpsMPUtemp;

//定位
UartNum<float, 2> uartNum(&uart2);
const float maxX = 200;
const float maxY = 200;
float posX = -1;
float posY = -1;

//PID
const float ratePID = 33;
//前馈补偿PID前馈系统：
//H(s)=s^2/gk
//tustin: H(z)=4/gkT^2*(z^2-2z+1)/(z^2+2z+1)
//向后差分: H(z)=200/gkpiT^2(1-2z^-1+z^-2)
//k为舵机摇臂与平板摇臂之比
//g为重力加速度
class FeedforwardSys
{
protected:
	float xn1, xn2, yn1, yn2;
	float k, T, factor;
	bool isBegin;
	RcFilter filter;
public:
	FeedforwardSys(float k, float T) :
		xn1(0), xn2(0),
		k(k), T(T), factor(300 / 9.8 / k / M_PI / T / T),
		isBegin(true),
		filter(30, 3)
	{

	}

	float getY(float x)
	{
		float y = 0;
		x /= 1000;//转换为标准单位

		if (isBegin)
		{
			xn1 = x;
			xn2 = x;
			isBegin = false;
		}
		y = (x - 2 * xn1 + xn2)*factor;

		xn2 = xn1;
		xn1 = x;
		y = filter.getFilterOut(y);
		return y;
	}
}feedforwardSysX(1.9 / 9, 1 / ratePID),
feedforwardSysY(1.9 / 9, 1 / ratePID);

float targetX = maxX / 2, targetY = maxY / 2,
targetXraw = targetX, targetYraw = targetY;
const float factorPID = 1.24;
//PIDIntSepIncDiff
//pidX(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID, 15),
//pidY(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID, 15);
PIDFeforGshifIntIncDiff
pidX(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID, 8),
pidY(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID, 8);
//PIDDifferentialAhead
//pidX(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID),
//pidY(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID);
//PIDFeedforward
//pidX(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID),
//pidY(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID);

RcFilter filterX(ratePID, 7), filterY(ratePID, 7), filterOutX(ratePID, 10), filterOutY(ratePID, 10),
filterTargetX(100, 2), filterTargetY(100, 2);
float outX, outY;

//动力
Servo servoX(&PB9, 200, 0.7, 2.3);
Servo servoY(&PB8, 200, 0.75, 2.35);

//底座
const float factorServo = 6.5;
float angle[3];
SoftI2c si2c3(&PB3, &PB11);
MPU9250AHRS mpu(&si2c3, MPU6500_Model_6555);

//交互
Button keyL(&PB4, 1);
Button keyR(&PB1, 1);
Button keyU(&PC5, 1);
Button keyD(&PC2, 1);
Led led(&PD2, 1);
OLEDI2C oled(&i2c1);

//照明
WS2812 ws2812(&PB0);

//收到定位坐标立即进行PID运算
//将输出存入outXY到舵机刷新程序输出
//UI交互
int index = 0;
float circleR = 0;
float theta = 0;
void posReceiveEvent(UartNum<float, 2>* uartNum)
{
	//按键响应
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

	//按键响应
	float increase = 0;
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
		increase += 1;
	}
	if (keyD.pressed_for(200, 0))
	{
		increase -= 1;
	}

	//功能
	switch (index)
	{
	case 0:
		targetXraw += increase;
		limit<float>(targetXraw, 30, maxX - 30);
		break;
	case 1:
		targetYraw += increase;
		limit<float>(targetYraw, 30, maxY - 30);
		break;
	case 2:
		circleR = circleR + increase;
		limit<float>(circleR, 0, (maxY - 100) / 2);
		theta += 2 * PI / 35 * 0.5;//0.5圈一秒
		targetXraw = maxX / 2 + circleR*sin(theta);
		targetYraw = maxY / 2 + circleR*cos(theta);
		break;
	default:
		break;
	}


	//对定位PID的目标坐标进行滤波
	targetX = filterTargetX.getFilterOut(targetXraw);
	targetY = filterTargetY.getFilterOut(targetYraw);
	pidX.setTarget(targetX);
	pidY.setTarget(targetY);

	if (uartNum->getLength() == 2)
	{
		posX = uartNum->getNum()[0];
		posY = uartNum->getNum()[1];
		//uart1.printf("%f\t%f\r\n", posX, posY);

		if (!isnan(posX) && !isnan(posY))
		{
			//posX = filterX.getFilterOut(posX);
			//posY = filterY.getFilterOut(posY);

			outX = 0, outY = 0;
			outX += pidX.refresh(posX);
			outY -= pidY.refresh(posY);

			//outX = filterOutX.getFilterOut(outX);
			//outY = filterOutY.getFilterOut(outY);


			//servoX.setPct(outX);
			//servoY.setPct(outY);
		}
		else
		{
			pidX.reset();
			pidY.reset();
			outX = 0; outY = 0;
			//servoX.setPct(0);
			//servoY.setPct(0);
		}
	}
	fpsPIDtemp = fpsPID.getFps();
	float vscan[] = { posX,posY,outX,outY
		//,fpsPIDtemp,fpsUItemp
		,pidX.getFeedforward(),pidY.getFeedforward()
		,targetX,targetY };
	uartVscan.sendOscilloscope(vscan, 8);
}

//底座平衡
void mpuRefresh(void *pvParameters)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		//对mpu角度进行反馈
		mpu.getAngle(angle, angle + 1, angle + 2);
		servoX.setPct(outX - angle[1] * factorServo);
		servoY.setPct(outY - angle[0] * factorServo);
		fpsMPUtemp = fpsMPU.getFps();
		vTaskDelayUntil(&xLastWakeTime, (10 / portTICK_RATE_MS));
	}

}

//UI交互
void uiRefresh(void *pvParameters)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while (1)
	{
		switch (index)
		{
		case 0:
			oled.printf(0, 0, 2, "*%.1f %.1f %.0f  ", targetXraw, targetYraw, circleR);
			break;
		case 1:
			oled.printf(0, 0, 2, "%.1f *%.1f %.0f  ", targetXraw, targetYraw, circleR);
			break;
		case 2:
			oled.printf(0, 0, 2, "%.1f %.1f *%.0f  ", targetXraw, targetYraw, circleR);
			break;
		default:
			break;
		}
		oled.printf(0, 2, 2, "%.1f %.1f   ", (float)posX, (float)posY);
		oled.printf(0, 4, 2, "%.1f %.1f   ", angle[0], angle[1]);
		fpsUItemp = fpsUI.getFps();
		oled.printf(0, 6, 2, "%.0f %.0f %.0f ", fpsPIDtemp, fpsUItemp, fpsMPUtemp);

		vTaskDelayUntil(&xLastWakeTime, (100 / portTICK_RATE_MS));
	}

}



void setup()
{
	ebox_init();

	//调试
	uart1.begin(115200);
	fpsPID.begin();
	fpsUI.begin();
	fpsMPU.begin();

	//PID
	pidX.setTarget(maxX / 2);
	pidX.setOutputLim(-100, 100);
	//pidX.setISepPoint(15);
	pidX.setGearshiftPoint(10, 50);
	pidX.attach(&feedforwardSysX, &FeedforwardSys::getY);

	pidY.setTarget(maxY / 2);
	pidY.setOutputLim(-100, 100);
	//pidY.setISepPoint(15);
	pidY.setGearshiftPoint(10, 50);
	pidY.attach(&feedforwardSysY, &FeedforwardSys::getY);

	//动力
	servoX.begin();
	servoX.setPct(0);
	servoY.begin();
	servoY.setPct(0);

	//定位
	uartNum.begin(115200);
	uartNum.attach(posReceiveEvent);

	//底座
	mpu.setGyroBias(-0.0151124271, -0.00376615906, 0.0124653624);
	mpu.setAccelBias(-0.0590917952, -0.0075366213, 0.120300293);
	mpu.setMagBiasSens(
		-18.786200, 17.835992, 14.496549,
		0.986133, 1.038038, 0.975829);
	mpu.setOrien(1, 2, 3);
	mpu.begin(400000, 100, MPU6500_Gyro_Full_Scale_500dps, MPU6500_Accel_Full_Scale_4g);
	//float bias[3];
	//mpu.getAccelBias(bias, bias + 1, bias + 2);

	//交互
	keyD.begin();
	keyL.begin();
	keyR.begin();
	keyU.begin();
	led.begin();
	oled.begin();

	//照明
	ws2812.begin();
	ws2812.setAllDataHSV(90, 0, 0.7);

	//操作系统
	set_systick_user_event_per_sec(configTICK_RATE_HZ);
	attach_systick_user_event(xPortSysTickHandler);

	xTaskCreate(mpuRefresh, "mpuRefresh", 512, NULL, 0, NULL);
	xTaskCreate(uiRefresh, "uiRefresh", 512, NULL, 0, NULL);
	vTaskStartScheduler();
}


int main(void)
{
	setup();


	while (1)
	{

	}

}


