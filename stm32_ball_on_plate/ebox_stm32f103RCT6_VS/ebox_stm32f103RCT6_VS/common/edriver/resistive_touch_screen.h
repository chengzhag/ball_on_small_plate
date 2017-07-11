#ifndef __RESISTIVE_TOUCH_SCREEN_H
#define __RESISTIVE_TOUCH_SCREEN_H

#include "ebox.h"

class ResistiveTouchSceen
{
protected:
	Gpio *pinXPos,*pinYPos,
		*pinXNeg,*pinYNeg;
public:

	//构建ResistiveTouchSceen对象
	//其中pinXPos和pinYPos必须为ADC通道
	ResistiveTouchSceen(
		Gpio* pinXPos,
		Gpio* pinYPos,
		Gpio* pinXNeg,
		Gpio* pinYNeg
	);

	void begin();

	u8 getPos(u16 *x, u16 *y);
};


#endif