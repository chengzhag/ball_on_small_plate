#include "ebox.h"
#include "servo.h"
Servo servo(&PB8);
void setup()
{
	ebox_init();
	servo.begin();
	uart1.begin(115200);
}
int main(void)
{
	setup();
	servo.setPct(0);
	float pct = 0, increase = 1;
	while (1)
	{
		pct += increase;
		if (pct >= 100 || pct <= -100)
		{
			increase = -increase;
		}
		servo.setPct(pct);
		uart1.printf("%f\n", servo.getPct());
		delay_ms(20);
	}
}