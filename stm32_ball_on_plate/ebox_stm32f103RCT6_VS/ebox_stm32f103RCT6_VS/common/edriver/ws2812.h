#ifndef __WS2812_H
#define __WS2812_H
#include "ebox.h"
#include "color_convert.h"

#define TIM_PERIOD		29					// Number of CPU cycles that will constitute 1 period
#define PWM_HIGH_WIDTH		17				// Duty cycle of pwm signal for a logical 1 to be read by the ws2812 chip. 
//Duty cycle = PWM_HIGH_WIDTH/TIM_PERIOD*100
#define PWM_LOW_WIDTH		9					// Duty cycle of pwm signal for a logical 0 to be read by the ws2812 chip.
//Duty cycle = PWM_LOW_WIDTH/TIM_PERIOD*100
//9/29*100

#define LED_COUNT		16

#define LED_BUFFER_SIZE		24*LED_COUNT+42				// Buffer size needs to be the number of LEDs times 24 bits plus 42 trailing bit to signify the end of the data being transmitted.


class WS2812
{
	Gpio *pin;

	void RCC_Config(void);
	void TIM_Config(void);
	void DMA_Config(void);
public:
	uint8_t rgbData[LED_COUNT][3];						//Array that will store color data
	uint8_t dmaBuff[LED_BUFFER_SIZE];					//Array of data to be sent to leds.

	WS2812(Gpio *p_pin)
	{
		pin = p_pin;
	}
	void	begin();
	void reset();
	void sendData();
	void rainbowLoop(float brightness);

	//设置所有灯的颜色
	//uint8_t r uint8_t g uint8_t b
	void setAllDataRGB(uint8_t r, uint8_t g, uint8_t b)
	{
		uint16_t len = LED_COUNT;

		while (len)
		{
			len--;
			rgbData[len][0] = g;
			rgbData[len][1] = r;
			rgbData[len][2] = b;
		}
		sendData();
	}

	//设置所有灯的颜色
	//COLOR_RGB &rgb
	void setAllDataRGB(COLOR_RGB &rgb)
	{
		setAllDataRGB(rgb.r, rgb.g, rgb.b);
	}

	//设置所有灯的颜色
	//int h [0,360] float s [0,1] float v [0,1]
	void setAllDataHSV(int h, float s, float v)
	{
		COLOR_HSV hsv;
		hsv.h = h;
		hsv.s = s;
		hsv.v = v;
		setAllDataHSV(hsv);
	}

	//设置所有灯的颜色
	//COLOR_HSV &hsv
	void setAllDataHSV(COLOR_HSV &hsv)
	{
		COLOR_RGB rgb;
		hsv.h = hsv.h % 360;
		HSV_to_RGB(hsv, rgb);
		setAllDataRGB(rgb);
	}
};
#endif
