#include "ebox.h"

//调试
#include "led.h"
#include "my_math.h"
#include "uart_vcan.h"
#include "math.h"
//PID
#include "PID.h"
#include "signal_stream.h"
#include <math.h>
#include "interpolation.h"
#include "fuzzyTable.h"
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



//#define SYSTEM_IDENTIFICATION
#define BALL_BALANCE
#define ENABLE_REPETITIVE_CONTROLLER

#ifdef SYSTEM_IDENTIFICATION
#include "signal_table.h"
#endif // SYSTEM_IDENTIFICATION

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
const float ratePID = 32;
const float intervalPID = 1 / ratePID;
//前馈系统
const float feedForwardSysK = 1;
const float gzDenominator 
= feedForwardSysK*26.4106* intervalPID *intervalPID;
float feedforwardSysH[] = {
	1 / gzDenominator,
	-2/ gzDenominator,
	1 / gzDenominator
};
SysWithOnlyZero feedforwardSysX(feedforwardSysH, 3)
, feedforwardSysY(feedforwardSysH, 3);

//pid参数
float targetX = maxX / 2, targetY = maxY / 2,
targetXraw = targetX, targetYraw = targetY;
const float factorPID = 1.24;
//PIDIntSepIncDiff
//pidX(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID, 15),
//pidY(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID, 15);
PIDFeforGshifIntIncDiff
pidX(0.25f*factorPID, 0.2f*factorPID, 0.14f*factorPID, 1.f / ratePID, 30),
pidY(0.27f*factorPID, 0.18f*factorPID, 0.17f*factorPID, 1.f / ratePID, 30);
//PIDFeforGshifIntIncDiffDezone
//pidX(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID, 8, 2),
//pidY(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID, 8, 2);
//PIDDifferentialAhead
//pidX(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID),
//pidY(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID);
//PIDFeedforward
//pidX(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID),
//pidY(0.3f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID);
//PIDPosition
//pidX(0.2f*factorPID, 0.f*factorPID, 0.12f*factorPID, 1.f / ratePID),
//pidY(0.24f*factorPID, 0.f*factorPID, 0.14f*factorPID, 1.f / ratePID);
//Interpolation2D
//fuzzyPIDDeltaKpTable((float*)fuzzyPIDDeltaKpX, (float*)fuzzyPIDDeltaKpY, (float*)fuzzyPIDDeltaKpZ, sizeof(fuzzyPIDDeltaKpX) / sizeof(float), sizeof(fuzzyPIDDeltaKpY) / sizeof(float))
//, fuzzyPIDDeltaKiTable((float*)fuzzyPIDDeltaKiX, (float*)fuzzyPIDDeltaKiY, (float*)fuzzyPIDDeltaKiZ, sizeof(fuzzyPIDDeltaKiX) / sizeof(float), sizeof(fuzzyPIDDeltaKiY) / sizeof(float))
//, fuzzyPIDDeltaKdTable((float*)fuzzyPIDDeltaKdX, (float*)fuzzyPIDDeltaKdY, (float*)fuzzyPIDDeltaKdZ, sizeof(fuzzyPIDDeltaKdX) / sizeof(float), sizeof(fuzzyPIDDeltaKdY) / sizeof(float));
//PIDFuzzy
//pidX(0.25f*factorPID, 0.2f*factorPID, 0.16f*factorPID, 1.f / ratePID),
//pidY(0.27f*factorPID, 0.18f*factorPID, 0.19f*factorPID, 1.f / ratePID);
RcFilter filterX(ratePID, 15), filterY(ratePID, 15)
, filterOutX(ratePID, 10), filterOutY(ratePID, 10)
, filterTargetX(ratePID, 1), filterTargetY(ratePID, 1);
float outX, outY;
//重复控制补偿系统
const float rateCircle = 0.5;
#ifdef ENABLE_REPETITIVE_CONTROLLER
const int lengthRepetitiveController = ratePID / rateCircle;
RepetitiveController repetitiveControllerX(lengthRepetitiveController, 12, 0.25, ratePID, 3)
, repetitiveControllerY(lengthRepetitiveController, 12, 0.25, ratePID, 3);
float outRepetitiveControllerX = 0, outRepetitiveControllerY = 0;
bool isBallCircle = false, isBallCircleOld = false;
#endif // ENABLE_REPETITIVE_CONTROLLER

//动力
Servo servoX(&PB9, 200, 0.7, 2.3);
Servo servoY(&PB8, 200, 0.79, 2.39);

//底座
const float rateMPU = 100;
const float intervalMPU = 1 / rateMPU;
const uint32_t mpuRefreshDelay = ((1000 * intervalMPU + 0.5) / portTICK_RATE_MS);
const float factorServo = 6.5;
float angle[3];
SoftI2c si2c3(&PB3, &PB11);
MPU9250AHRS mpu(&si2c3, MPU6500_Model_6555);

//交互
const float rateUI = 10;
const float intervalUI = 1 / rateUI;
const uint32_t uiRefreshDelay = ((1000 * intervalUI + 0.5) / portTICK_RATE_MS);
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
bool isBallBack = true;
bool isShutdown = false;
void posReceiveEvent(UartNum<float, 2>* uartNum)
{
	isShutdown = false;
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
	if (keyR.pressed_for(5000, 1))
	{
		uartNum->printf("s");
	}
	if (keyL.pressed_for(5000, 1))
	{
		uartNum->printf("k");
	}

	//功能
#ifdef ENABLE_REPETITIVE_CONTROLLER
	isBallCircleOld = isBallCircle;
#endif // ENABLE_REPETITIVE_CONTROLLER

	
	switch (index)
	{
	case 0:
		targetXraw += increase;
		limit<float>(targetXraw, 30, maxX - 30);
#ifdef ENABLE_REPETITIVE_CONTROLLER
		isBallCircle = false;
#endif // ENABLE_REPETITIVE_CONTROLLER
		break;
	case 1:
		targetYraw += increase;
		limit<float>(targetYraw, 30, maxY - 30);
#ifdef ENABLE_REPETITIVE_CONTROLLER
		isBallCircle = false;
#endif // ENABLE_REPETITIVE_CONTROLLER
		break;
	case 2:
		circleR = circleR + increase;
		limit<float>(circleR, 0, (maxY - 100) / 2);
		theta += 2 * PI *intervalPID * rateCircle;//0.5圈一秒
		targetXraw = maxX / 2 + circleR*sin(theta);
		targetYraw = maxY / 2 + circleR*cos(theta);
#ifdef ENABLE_REPETITIVE_CONTROLLER
		if (circleR > 5)
		{
			isBallCircle = true;
		}
		else
		{
			isBallCircle = false;
		}
#endif // ENABLE_REPETITIVE_CONTROLLER
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
			if (isBallBack == false)
			{
#ifdef ENABLE_REPETITIVE_CONTROLLER
				repetitiveControllerX.clear();
				repetitiveControllerY.clear();
#endif // ENABLE_REPETITIVE_CONTROLLER

				pidX.resetState();
				pidY.resetState();

				isBallBack = true;
			}
			

			//posX = filterX.getFilterOut(posX);
			//posY = filterY.getFilterOut(posY);

#ifdef ENABLE_REPETITIVE_CONTROLLER
			//进行重复补偿控制
			if (isBallCircle == true && isBallCircleOld == true)
			{
				outRepetitiveControllerX =
					repetitiveControllerX.refresh(targetX - posX);
				outRepetitiveControllerY =
					repetitiveControllerY.refresh(targetY - posY);
				//outX += outRepetitiveControllerX;
				//outY -= outRepetitiveControllerY;
			}
			else if (isBallCircle == false && isBallCircleOld == true)
			{
				repetitiveControllerX.clear();
				repetitiveControllerY.clear();
				outRepetitiveControllerX = 0;
				outRepetitiveControllerY = 0;
			}

			outX = 0, outY = 0;
			outX += pidX.refresh(posX - outRepetitiveControllerX);
			outY -= pidY.refresh(posY - outRepetitiveControllerY);
#else
			outX = 0, outY = 0;
			outX += pidX.refresh(posX);
			outY -= pidY.refresh(posY);
#endif // ENABLE_REPETITIVE_CONTROLLER


			//outX = filterOutX.getFilterOut(outX);
			//outY = filterOutY.getFilterOut(outY);


			//servoX.setPct(outX);
			//servoY.setPct(outY);
		}
		else
		{
			outX = 0; outY = 0;
			isBallBack = false;
		}
	}
	fpsPIDtemp = fpsPID.getFps();
	//float kpX, kiX, kdX;
	//pidX.getPID(&kpX, &kiX, &kdX);
	//float kpY, kiY, kdY;
	//pidX.getPID(&kpY, &kiY, &kdY);
	float vscan[] = { 
		posX,posY
		,outX,outY
		//,fpsPIDtemp,fpsUItemp
		//,pidX.getFeedforward(),pidY.getFeedforward()
		,outRepetitiveControllerX,outRepetitiveControllerY
		//,(float)pidX.getCurrentRule(),(float)pidY.getCurrentRule()
		//, kpX, kiX, kdX, kpY, kiY, kdY
		,targetX,targetY 
	};
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
		vTaskDelayUntil(&xLastWakeTime, mpuRefreshDelay);
	}

}

//UI交互
void uiRefresh(void *pvParameters)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while (1)
	{
		//判断是否树莓派已关机
		if (isShutdown = true)
		{
			fpsPIDtemp = 0;
		}

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
		
		//把关机置1，等待串口中断来置0
		isShutdown = true;

		vTaskDelayUntil(&xLastWakeTime, uiRefreshDelay);
	}

}

#ifdef SYSTEM_IDENTIFICATION
void systemIdentificationTask(void *pvParameters)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	float timeStamp = 0, input = 0, output;
	const float sampleInterval = 1000 / (int)ratePID / 1000.f;
	float angle[3];

	bool isStable = false;
	bool isEnd = false;
	int index = 0;

	while (1)
	{
		timeStamp += sampleInterval;
		mpu.getAngle(angle, angle + 1, angle + 2);
		output = -angle[0];

		if (!isStable)
		{
			if (timeStamp > 10)
			{
				isStable = true;
				timeStamp = 0;
			}
		}
		else
		{
			if (!isEnd)
			{
				input = 30 * sweepSignal[index];

				servoX.setPct(input);
				index++;
				if (index >= sweepSignalLength)
				{
					isEnd = true;
				}

				uart1.printf("%f\t%f\t%f\r\n", timeStamp, input, output);
			}

		}

		//float dataSend[] = { timeStamp,input,output };
		//uartVscan.sendOscilloscope(dataSend, 3);

		

		vTaskDelayUntil(&xLastWakeTime, (1000 * intervalPID / portTICK_RATE_MS));
	}

}
#endif // SYSTEM_IDENTIFICATION

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
	pidX.setOutputLim(-50, 50);
	//pidX.setISepPoint(15);
	pidX.setGearshiftPoint(10, 50);
	pidX.attachFeedForwardH(&feedforwardSysX, &SysWithOnlyZero::getY);
	//pidX.setParams(80, 30, 1.5, 0.5, 10);
	//pidX.setFuzzyTable(&fuzzyPIDDeltaKpTable, &fuzzyPIDDeltaKiTable, &fuzzyPIDDeltaKdTable);

	pidY.setTarget(maxY / 2);
	pidY.setOutputLim(-50, 50);
	//pidY.setISepPoint(15);
	pidY.setGearshiftPoint(10, 50);
	pidY.attachFeedForwardH(&feedforwardSysY, &SysWithOnlyZero::getY);
	//pidY.setParams(80, 30, 1.5, 0.5, 10);
	//pidY.setFuzzyTable(&fuzzyPIDDeltaKpTable, &fuzzyPIDDeltaKiTable, &fuzzyPIDDeltaKdTable);


	//动力
	servoX.begin();
	servoX.setPct(0);
	servoY.begin();
	servoY.setPct(0);

	//定位
	uartNum.begin(115200);

	//底座
	mpu.setGyroBias(-0.0151124271, -0.00376615906, 0.0124653624);
	mpu.setAccelBias(-0.0590917952, -0.0075366213, 0.120300293);
	mpu.setMagBiasSens(
		-18.786200, 17.835992, 14.496549,
		0.986133, 1.038038, 0.975829);
	mpu.setOrien(1, 2, 3);


	//交互
	keyD.begin();
	keyL.begin();
	keyR.begin();
	keyU.begin();
	led.begin();
	oled.begin();

	//照明
	ws2812.begin();




#ifdef SYSTEM_IDENTIFICATION
	mpu.begin(400000, ratePID, MPU6500_Gyro_Full_Scale_500dps, MPU6500_Accel_Full_Scale_4g
		,MPU6500_DLPF_Bandwidth_250Hz,MPU6500_A_DLPF_Bandwidth_420Hz);
	//操作系统
	set_systick_user_event_per_sec(configTICK_RATE_HZ);
	attach_systick_user_event(xPortSysTickHandler);
	xTaskCreate(systemIdentificationTask, "mpuRefresh", 512, NULL, 0, NULL);
#endif // SYSTEM_IDENTIFICATION

#ifdef BALL_BALANCE
	mpu.begin(400000, rateMPU, MPU6500_Gyro_Full_Scale_500dps, MPU6500_Accel_Full_Scale_4g);
	//float bias[3];
	//mpu.getAccelBias(bias, bias + 1, bias + 2);
	uartNum.attach(posReceiveEvent);
	ws2812.setAllDataHSV(60, 1, 0.7);
	//操作系统
	set_systick_user_event_per_sec(configTICK_RATE_HZ);
	attach_systick_user_event(xPortSysTickHandler);
	xTaskCreate(mpuRefresh, "mpuRefresh", 512, NULL, 0, NULL);
	xTaskCreate(uiRefresh, "uiRefresh", 512, NULL, 0, NULL);
#endif // BALL_BALANCE
	vTaskStartScheduler();
}


int main(void)
{
	setup();


	while (1)
	{

	}

}


