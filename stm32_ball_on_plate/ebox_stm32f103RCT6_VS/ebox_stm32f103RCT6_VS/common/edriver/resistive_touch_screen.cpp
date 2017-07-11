#include "resistive_touch_screen.h"

ResistiveTouchSceen::ResistiveTouchSceen(Gpio* pinXPos, Gpio* pinYPos, Gpio* pinXNeg, Gpio* pinYNeg) :
	pinXPos(pinXPos),
	pinYPos(pinYPos),
	pinXNeg(pinXNeg),
	pinYNeg(pinYNeg)
{

}

void ResistiveTouchSceen::begin()
{

}

u8 ResistiveTouchSceen::getPos(u16 *x, u16 *y)
{
	//判断是否有触摸
	pinYPos->mode(INPUT_PU);
	pinXNeg->mode(OUTPUT_OD);
	pinXPos->reset();
	pinXPos->mode(OUTPUT_OD);
	pinXPos->set();
	pinYNeg->mode(OUTPUT_OD);
	pinYNeg->set();
	delay_us(800);
	if (pinYPos->read() != 0)
	{
		return 0;
	}

	//测量X坐标值
	pinYPos->mode(OUTPUT_PP);
	pinYPos->set();
	pinYNeg->mode(OUTPUT_OD);
	pinYNeg->reset();
	pinXNeg->mode(OUTPUT_OD);
	pinXNeg->set();
	pinXPos->mode(AIN);
	delay_us(200);
	*x = analog_read(pinXPos);

	//测量Y坐标值
	pinXPos->mode(OUTPUT_PP);
	pinXPos->set();
	pinXNeg->mode(OUTPUT_OD);
	pinXNeg->reset();
	pinYNeg->mode(OUTPUT_OD);
	pinYNeg->set();
	pinYPos->mode(AIN);
	delay_us(200);
	*y = analog_read(pinYPos);


	return 1;
}
