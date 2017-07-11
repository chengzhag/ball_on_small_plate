#ifndef __MPU6500_H
#define __MPU6500_H

#include "ebox.h"


////////////////////////////////////////////////////////////////////////////////// 	
//如果AD0脚(9脚)接地,IIC地址为0X68(不包含最低位).
//如果接V3.3,则IIC地址为0X69(不包含最低位).
#define MPU6500_ADDRESS            0xD0  //MPU6500的器件IIC地址 0X68<<1
#define MPU6500_ID				0X71  	//MPU6500的器件ID
#define MPU6555_ID				0X73

//MPU6500的内部寄存器
#define MPU6500_SELF_TESTX 		0X0D	//自检寄存器X
#define MPU6500_SELF_TESTY 		0X0E	//自检寄存器Y
#define MPU6500_SELF_TESTZ 		0X0F	//自检寄存器Z
#define MPU6500_SELF_TESTA 		0X10	//自检寄存器A
#define MPU6500_XG_OFFSET_H      0x13  // User-defined trim values for gyroscope
#define MPU6500_XG_OFFSET_L      0x14
#define MPU6500_YG_OFFSET_H      0x15
#define MPU6500_YG_OFFSET_L      0x16
#define MPU6500_ZG_OFFSET_H      0x17
#define MPU6500_ZG_OFFSET_L      0x18
#define MPU6500_SAMPLE_RATE 		0X19	//采样频率分频器
#define MPU6500_CFG 				0X1A	//配置寄存器
#define MPU6500_GYRO_CFG 		0X1B	//陀螺仪配置寄存器
#define MPU6500_ACCEL_CFG 		0X1C	//加速度计配置寄存器
#define MPU6500_MOTION_DET 		0X1F	//运动检测阀值设置寄存器
#define MPU6500_FIFO_EN 			0X23	//FIFO使能寄存器
#define MPU6500_I2CMST_CTRL 		0X24	//IIC主机控制寄存器
#define MPU6500_I2CSLV0_ADDR 	0X25	//IIC从机0器件地址寄存器
#define MPU6500_I2CSLV0 			0X26	//IIC从机0数据地址寄存器
#define MPU6500_I2CSLV0_CTRL 	0X27	//IIC从机0控制寄存器
#define MPU6500_I2CSLV1_ADDR 	0X28	//IIC从机1器件地址寄存器
#define MPU6500_I2CSLV1 			0X29	//IIC从机1数据地址寄存器
#define MPU6500_I2CSLV1_CTRL 	0X2A	//IIC从机1控制寄存器
#define MPU6500_I2CSLV2_ADDR 	0X2B	//IIC从机2器件地址寄存器
#define MPU6500_I2CSLV2 			0X2C	//IIC从机2数据地址寄存器
#define MPU6500_I2CSLV2_CTRL 	0X2D	//IIC从机2控制寄存器
#define MPU6500_I2CSLV3_ADDR 	0X2E	//IIC从机3器件地址寄存器
#define MPU6500_I2CSLV3 			0X2F	//IIC从机3数据地址寄存器
#define MPU6500_I2CSLV3_CTRL 	0X30	//IIC从机3控制寄存器
#define MPU6500_I2CSLV4_ADDR 	0X31	//IIC从机4器件地址寄存器
#define MPU6500_I2CSLV4 			0X32	//IIC从机4数据地址寄存器
#define MPU6500_I2CSLV4_DO 		0X33	//IIC从机4写数据寄存器
#define MPU6500_I2CSLV4_CTRL 	0X34	//IIC从机4控制寄存器
#define MPU6500_I2CSLV4_DI 		0X35	//IIC从机4读数据寄存器

#define MPU6500_I2CMST_STA 		0X36	//IIC主机状态寄存器
#define MPU6500_INTBP_CFG 		0X37	//中断/旁路设置寄存器
#define MPU6500_INT_EN 			0X38	//中断使能寄存器
#define MPU6500_INT_STA 			0X3A	//中断状态寄存器

#define MPU6500_ACCEL_XOUTH 		0X3B	//加速度值,X轴高8位寄存器
#define MPU6500_ACCEL_XOUTL 		0X3C	//加速度值,X轴低8位寄存器
#define MPU6500_ACCEL_YOUTH 		0X3D	//加速度值,Y轴高8位寄存器
#define MPU6500_ACCEL_YOUTL 		0X3E	//加速度值,Y轴低8位寄存器
#define MPU6500_ACCEL_ZOUTH 		0X3F	//加速度值,Z轴高8位寄存器
#define MPU6500_ACCEL_ZOUTL 		0X40	//加速度值,Z轴低8位寄存器

#define MPU6500_TEMP_OUTH 		0X41	//温度值高八位寄存器
#define MPU6500_TEMP_OUTL 		0X42	//温度值低8位寄存器

#define MPU6500_GYRO_XOUTH 		0X43	//陀螺仪值,X轴高8位寄存器
#define MPU6500_GYRO_XOUTL 		0X44	//陀螺仪值,X轴低8位寄存器
#define MPU6500_GYRO_YOUTH 		0X45	//陀螺仪值,Y轴高8位寄存器
#define MPU6500_GYRO_YOUTL 		0X46	//陀螺仪值,Y轴低8位寄存器
#define MPU6500_GYRO_ZOUTH 		0X47	//陀螺仪值,Z轴高8位寄存器
#define MPU6500_GYRO_ZOUTL 		0X48	//陀螺仪值,Z轴低8位寄存器

#define MPU6500_I2CSLV0_DO 		0X63	//IIC从机0数据寄存器
#define MPU6500_I2CSLV1_DO 		0X64	//IIC从机1数据寄存器
#define MPU6500_I2CSLV2_DO 		0X65	//IIC从机2数据寄存器
#define MPU6500_I2CSLV3_DO 		0X66	//IIC从机3数据寄存器

#define MPU6500_I2CMST_DELAY 	0X67	//IIC主机延时管理寄存器
#define MPU6500_SIGPATH_RST 		0X68	//信号通道复位寄存器
#define MPU6500_MDETECT_CTRL 	0X69	//运动检测控制寄存器
#define MPU6500_USER_CTRL 		0X6A	//用户控制寄存器
#define MPU6500_PWR_MGMT1 		0X6B	//电源管理寄存器1
#define MPU6500_PWR_MGMT2 		0X6C	//电源管理寄存器2 
#define MPU6500_FIFO_CNTH 		0X72	//FIFO计数寄存器高八位
#define MPU6500_FIFO_CNTL 		0X73	//FIFO计数寄存器低八位
#define MPU6500_FIFO_RW 			0X74	//FIFO读写寄存器
#define MPU6500_DEVICE_ID 		0X75	//器件ID寄存器

#define MPU6500_XA_OFFSET_H      0x77
#define MPU6500_XA_OFFSET_L      0x78
#define MPU6500_YA_OFFSET_H      0x7A
#define MPU6500_YA_OFFSET_L      0x7B
#define MPU6500_ZA_OFFSET_H      0x7D
#define MPU6500_ZA_OFFSET_L      0x7E

typedef enum
{
	MPU6500_Model_6500 = 0,
	MPU6500_Model_6555 = 1
}MPU6500_Model_Typedef;

typedef enum
{
	MPU6500_Gyro_Full_Scale_250dps = 0,
	MPU6500_Gyro_Full_Scale_500dps = 1,
	MPU6500_Gyro_Full_Scale_1000dps = 2,
	MPU6500_Gyro_Full_Scale_2000dps = 3
}MPU6500_Gyro_Full_Scale_Typedef;

typedef enum
{
	MPU6500_Accel_Full_Scale_2g = 0,
	MPU6500_Accel_Full_Scale_4g = 1,
	MPU6500_Accel_Full_Scale_8g = 2,
	MPU6500_Accel_Full_Scale_16g = 3
}MPU6500_Accel_Full_Scale_Typedef;

typedef enum
{
	MPU6500_DLPF_Bandwidth_250Hz = 0,
	MPU6500_DLPF_Bandwidth_184Hz = 1,
	MPU6500_DLPF_Bandwidth_92Hz = 2,
	MPU6500_DLPF_Bandwidth_41Hz = 3,
	MPU6500_DLPF_Bandwidth_20Hz = 4,
	MPU6500_DLPF_Bandwidth_10Hz = 5,
	MPU6500_DLPF_Bandwidth_5Hz = 6,
	MPU6500_DLPF_Bandwidth_3600Hz = 7
}MPU6500_DLPF_Bandwidth_Typedef;

typedef enum
{
	MPU6500_A_DLPF_Bandwidth_218_1Hz = 1,
	MPU6500_A_DLPF_Bandwidth_99Hz = 2,
	MPU6500_A_DLPF_Bandwidth_44_8Hz = 3,
	MPU6500_A_DLPF_Bandwidth_21_2Hz = 4,
	MPU6500_A_DLPF_Bandwidth_10_2Hz = 5,
	MPU6500_A_DLPF_Bandwidth_5_05Hz = 6,
	MPU6500_A_DLPF_Bandwidth_420Hz = 7
}MPU6500_A_DLPF_Bandwidth_Typedef;

class MPU6500:public I2cIC
{
protected:
	//配置参数
	uint8_t address;
	u8 mpuID;//不同型号的ID，用于验证是否连接正确
	u16 sampleRate;

	//存储ID与型号的对应关系
	static const uint8_t mpuIDList[];

	//存储两者量程，用于转换标准单位
	uint16_t gyroFsr;
	uint8_t accelFsr;

	//量程寄存器值与量程的对应关系
	static const uint16_t gyroFsrList[];
	static const uint8_t accelFsrList[];

	//校准偏差
	float gyroBiasSub[3], accelBiasSub[3];

public:
	MPU6500(I2c* i2c, MPU6500_Model_Typedef model = MPU6500_Model_6500, uint8_t address = MPU6500_ADDRESS);

	void begin(uint32_t speed = 400000, uint16_t sampleRate = 1000,
		MPU6500_Gyro_Full_Scale_Typedef gyroFsr = MPU6500_Gyro_Full_Scale_2000dps,
		MPU6500_Accel_Full_Scale_Typedef accelFsr = MPU6500_Accel_Full_Scale_8g,
		MPU6500_DLPF_Bandwidth_Typedef bwDLPF = MPU6500_DLPF_Bandwidth_41Hz,
		MPU6500_A_DLPF_Bandwidth_Typedef bwADLPF = MPU6500_A_DLPF_Bandwidth_44_8Hz);

	//设置陀螺仪传感器满量程范围
	//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
	void setGyroFsr(MPU6500_Gyro_Full_Scale_Typedef fsr);

	//设置加速度传感器满量程范围
	//fsr:0,±2g;1,±4g;2,±8g;3,±16g
	void setAccelFsr(MPU6500_Accel_Full_Scale_Typedef fsr);

	//设置Gyro、Accel数字低通滤波器
	//lpf:数字低通滤波频率(Hz)
	void setBothLPF(MPU6500_DLPF_Bandwidth_Typedef bwDLPF, MPU6500_A_DLPF_Bandwidth_Typedef bwADLPF);

	//设置Gyro数字低通滤波器
	void setDLPF(MPU6500_DLPF_Bandwidth_Typedef bwDLPF);

	//设置Accel数字低通滤波器
	void setADLPF(MPU6500_A_DLPF_Bandwidth_Typedef bwADLPF);

	//设置采样率4~1000(Hz)
	void setSampleRate(u16 sampleRate);
	//获取采样率
	u16 getSampleRate();

	//设置bypass模式
	void setBypass();

	//得到温度值
	float getTemp(void);

	//得到陀螺仪值(原始值)
	//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
	void getGyro(short *gx, short *gy, short *gz);
	//gx,gy,gz:陀螺仪x,y,z轴的标准单位读数
	void getGyro(float *gx, float *gy, float *gz);

	//得到加速度值(原始值)
	//gx,gy,gz:加速度x,y,z轴的原始读数(带符号)
	void getAccel(short *ax, short *ay, short *az);
	//gx, gy, gz:加速度x, y, z轴的标准单位读数(g)
	void getAccel(float *ax, float *ay, float *az);


	///校准

	//在静止的情况下直接调用caliGyro
	void caliGyro();

	//设置GyroBias
	void setGyroBias(float bx, float by, float bz);
	//获取sampleNum次采样的校准前数据，作为偏差
	void getGyroBias(float* bx, float* by, float* bz, int sampleNum = 100);

	//在水平放置的情况下直接调用caliAccel
	void caliAccel();

	//设置GyroBias
	void setAccelBias(float bx, float by, float bz);
	//获取sampleNum次采样的校准前数据，作为偏差
	void getAccelBias(float* bx, float* by, float* bz, int sampleNum = 100);
};


#endif


