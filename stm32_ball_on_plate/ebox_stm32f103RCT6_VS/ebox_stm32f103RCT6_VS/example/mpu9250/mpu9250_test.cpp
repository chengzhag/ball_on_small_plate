#include "ebox.h"
#include "mpu9250.h"
#include "my_math.h"
#include "uart_vcan.h"



FpsCounter fps;
MPU9250AHRS mpu(&i2c1, MPU6500_Model_6555);
UartVscan vscan(&uart1);

#define SEND_ANGLE
//#define SEND_RAW

void setup()
{
	ebox_init();
	uart1.begin(115200);
	mpu.setGyroBias(-0.0151124271, -0.00376615906, 0.0124653624);
	mpu.setAccelBias(-0.00981201138, -0.00825439487, 0.146179199);
	mpu.setMagBiasSens(
		-18.786200, 17.835992, 14.496549,
		0.986133, 1.038038, 0.975829);
	mpu.setOrien(1, 2, 3);
	mpu.begin(200000, 200, MPU6500_Gyro_Full_Scale_500dps, MPU6500_Accel_Full_Scale_4g);
	fps.begin();
}
int main(void)
{
	setup();
#ifdef SEND_RAW
	float t, gx, gy, gz, ax, ay, az, mx, my, mz;
#endif // SEND_RAW
#ifdef SEND_ANGLE
	float data[5];
#endif // SEND_ANGLE


	while (1)
	{
#ifdef SEND_ANGLE
		mpu.getAngle(data, data + 1, data + 2);
		data[3] = fps.getFps();
		vscan.sendOscilloscope(data, 4);
#endif // SEND_ANGLE

#ifdef SEND_RAW
		mpu.getGyro(&gx, &gy, &gz);
		mpu.getAccel(&ax, &ay, &az);
		mpu.getMag(&mx, &my, &mz);
		t = mpu.getTemp();
		//uart1.printf("%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\r\n",
		//	t, gx, gy, gz, ax, ay, az, fps.getFps());//读取六轴标准单位数据
		//uart1.printf("%.3f\t%.3f\t%.3f\t%.3f\r\n",
		//	mx, my, mz, fps.getFps());//读取磁力计标准单位数据
		//uart1.printf("%.3f\t%.3f\t%.3f\r\n",
		//	mx, my, mz);//读取磁力计标准单位数据
		uart1.printf("%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\r\n",
			t, gx, gy, gz, ax, ay, az, mx, my, mz, fps.getFps());//读取九轴标准单位数据
		//uart1.printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%.3f\r\n",
		//	t, gx, gy, gz, ax, ay, az, fps.getFps());//读取六轴原始数据
		//uart1.printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%.3f\r\n",
		//	t, gx, gy, gz, ax, ay, az, mx, my, mz, fps.getFps());//读取九轴原始数据

#endif // SEND_RAW

		delay_ms(2);
	}

}


