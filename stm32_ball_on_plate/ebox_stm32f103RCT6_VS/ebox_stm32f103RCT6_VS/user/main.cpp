#include "ebox.h"
#include "led.h"
#include "servo.h"

Servo servo1(&PB0, 100, 0.8, 2.2);
Servo servo2(&PB1, 100, 0.8, 2.2);

void setup()
{
	ebox_init();
	uart1.begin(115200);
	servo1.begin();
	servo2.begin();
}
int main(void)
{
	setup();
	float pct = 0, increase = 1;
	while (1)
	{
		pct += increase;
		if (pct >= 100 || pct <= 0)
		{
			increase = -increase;
		}
		uart1.begin(115200);
		servo1.setPct(pct);
		servo2.setPct(pct);
		//servo1.setPct(50);
		//servo2.setPct(50);

		uart1.printf("%f\r\n", servo1.getPct());
		delay_ms(10);
	}

}


