#include "ebox.h"
#include "my_math.h"
#include "resistive_touch_screen.h"

ResistiveTouchSceen touch(&PC0,&PC1,&PC2,&PC3);

FpsCounter fps;

void setup()
{
	ebox_init();
	uart1.begin(115200);
	fps.begin();
}
int main(void)
{
	setup();
	u16 x, y;
	while (1)
	{
		u8 error = touch.getPos(&x, &y);
		uart1.printf("%u\t%u\t%u\t%f\r\n", error, x, y, fps.getFps());
		//delay_ms(20);
	}

}


