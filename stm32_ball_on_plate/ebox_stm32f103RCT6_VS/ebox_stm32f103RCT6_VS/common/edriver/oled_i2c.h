#ifndef _OLED_I2C_H
#define _OLED_I2C_H

#include "ebox.h"

class OLEDI2C
{
	uint32_t speed;
	uint8_t adress;
	I2c* i2c;
	char *printf_buf;

	void write_byte(uint8_t reg_address, uint8_t data);
	//写命令
	void write_cmd(unsigned char cmd);
	//写数据
	void write_data(unsigned char dat);

public:
	OLEDI2C(I2c* i2c, uint8_t adress = 0x78);

	void begin(uint32_t speed = 400000);

	//设置起始点坐标
	void set_xy(unsigned char x, unsigned char y);

	//全屏填充
	void fill(unsigned char fill_Data);

	//清屏
	void clear(void);

	//将OLED从休眠中唤醒
	void display_on(void);

	//让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
	void display_off(void);

	void show_char(unsigned char x, unsigned char y, char ch, unsigned char TextSize = 1);

	//x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
	//显示codetab.h中的ASCII字符,有6*8和8*16可选择
	void show_string(unsigned char x, unsigned char y, char ch[], unsigned char TextSize = 1);

	void printf(uint8_t x, uint8_t y, unsigned char TextSize, const char *fmt, ...);

	//x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
	//显示codetab.h中的汉字,16*16点阵
	void show_chinese(unsigned char x, unsigned char y, unsigned char N);

	//x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
	//显示BMP位图
	void draw_bmp(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);

};


#endif


