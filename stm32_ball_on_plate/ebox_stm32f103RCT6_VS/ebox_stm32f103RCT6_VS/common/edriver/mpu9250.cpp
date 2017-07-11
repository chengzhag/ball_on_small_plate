#include "mpu9250.h"

MPU9250::MPU9250(I2c* i2c, MPU6500_Model_Typedef model /*= MPU6500_Model_6500*/, uint8_t address6500 /*= MPU6500_ADDRESS*/, uint8_t address8963 /*= AK8963_ADDRESS*/) :
	MPU6500(i2c, model, address6500),
	AK8963(i2c, address8963)
{

}

void MPU9250::begin(uint32_t speed /*= 400000*/, uint16_t sampleRate /*= 100*/, MPU6500_Gyro_Full_Scale_Typedef gyroFsr /*= MPU6500_Gyro_Full_Scale_2000dps*/, MPU6500_Accel_Full_Scale_Typedef accelFsr /*= MPU6500_Accel_Full_Scale_8g*/, MPU6500_DLPF_Bandwidth_Typedef bwDLPF /*= MPU6500_DLPF_Bandwidth_41Hz*/, MPU6500_A_DLPF_Bandwidth_Typedef bwADLPF /*= MPU6500_A_DLPF_Bandwidth_44_8Hz*/)
{
	MPU6500::begin(speed, sampleRate, gyroFsr, accelFsr, bwDLPF);
	AK8963::begin(speed);
}
