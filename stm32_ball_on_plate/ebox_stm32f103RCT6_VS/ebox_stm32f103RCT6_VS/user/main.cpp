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
float targetX = maxX / 2, targetY = maxY / 2,
targetXraw = targetX, targetYraw = targetY;
const float factorPID = 1.24;
//PIDIntSepIncDiff
//pidX(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / 30.f, 15),
//pidY(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / 30.f, 15);
PIDGearshiftIntegral
pidX(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / 30.f),
pidY(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / 30.f);
Butterworth filterX(30, 7), filterY(30, 7), filterOutX(30, 10), filterOutY(30, 10),
filterTargetX(100, 3), filterTargetY(100, 3);
float outX, outY;

//动力
Servo servoX(&PB8, 100, 0.81, 2.35);
Servo servoY(&PB9, 100, 0.72, 2.35);

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
void posReceiveEvent(UartNum<float, 2>* uartNum)
{
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
			outY += pidY.refresh(posY);

			//outX = filterOutX.getFilterOut(outX);
			//outY = filterOutY.getFilterOut(outY);

			fpsPIDtemp = fpsPID.getFps();
			float vscan[] = { posX,posY,outX,outY ,fpsUItemp,fpsMPUtemp,targetX,targetY };
			uartVscan.sendOscilloscope(vscan, 8);

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
}

//底座平衡
void mpuRefresh(void *pvParameters)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		//对定位PID的目标坐标进行滤波
		targetX = filterTargetX.getFilterOut(targetXraw);
		targetY = filterTargetY.getFilterOut(targetYraw);
		pidX.setTarget(targetX);
		pidY.setTarget(targetY);

		//对mpu角度进行反馈
		mpu.getAngle(angle, angle + 1, angle + 2);
		servoX.setPct(outX + angle[1] * factorServo);
		servoY.setPct(outY - angle[0] * factorServo);
		fpsMPUtemp = fpsMPU.getFps();
		vTaskDelayUntil(&xLastWakeTime, (10 / portTICK_RATE_MS));
	}

}

//UI交互
int index = 0;
float circleR = 0;
float theta = 0;
void uiRefresh(void *pvParameters)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
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
			increase += 5;
		}
		if (keyD.pressed_for(200, 0))
		{
			increase -= 5;
		}

		//功能
		switch (index)
		{
		case 0:
			targetXraw += increase;
			limit<float>(targetXraw, 30, maxX - 30);
			oled.printf(0, 0, 2, "*%.1f %.1f %.0f  ", targetX, targetY, circleR);
			break;
		case 1:
			targetYraw += increase;
			limit<float>(targetYraw, 30, maxY - 30);
			oled.printf(0, 0, 2, "%.1f *%.1f %.0f  ", targetX, targetY, circleR);
			break;
		case 2:
			circleR = circleR + increase;
			limit<float>(circleR, 0, (maxY - 100) / 2);
			theta += 2 * PI / 50 * 1.5;//0.5圈一秒
			targetXraw = maxX / 2 + circleR*sin(theta);
			targetYraw = maxY / 2 + circleR*cos(theta);
			oled.printf(0, 0, 2, "%.1f %.1f *%.0f  ", targetX, targetY, circleR);
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
	pidX.setGearshiftPoint(10, 20);
	pidY.setTarget(maxY / 2);
	pidY.setOutputLim(-100, 100);
	//pidY.setISepPoint(15);
	pidX.setGearshiftPoint(10, 20);

	//动力
	servoY.begin();
	servoX.begin();

	//定位
	uartNum.begin(115200);
	uartNum.attach(posReceiveEvent);

	//底座
	mpu.setGyroBias(-0.0151124271, -0.00376615906, 0.0124653624);
	mpu.setAccelBias(-0.0271704104, -0.00390625, 0.132741705);
	mpu.setMagBiasSens(
		-18.786200, 17.835992, 14.496549,
		0.986133, 1.038038, 0.975829);
	mpu.setOrien(1, 2, 3);
	mpu.begin(400000, 100, MPU6500_Gyro_Full_Scale_500dps, MPU6500_Accel_Full_Scale_4g);


	//交互
	keyD.begin();
	keyL.begin();
	keyR.begin();
	keyU.begin();
	led.begin();
	oled.begin();

	//照明
	ws2812.begin();
	ws2812.setAllDataHSV(90, 0, 0.3);

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


