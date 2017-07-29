#include "ebox.h"
#include "uart_vcan.h"
#include "my_math.h"
#include "oled_i2c.h"




OLEDI2C oled(&i2c2);
FpsCounter fps;


void setup()
{
	ebox_init();
	uart1.begin(115200);

	oled.begin(400000);
	
	fps.begin();
}
int main(void)
{
	setup();

	float test = 0;
	while (1)
	{
		//oled.show_string(0, 0, "test!!!", 2);
		//oled.show_string(0, 2, "test!!!", 2);
		//oled.show_string(0, 4, "test!!!", 2);
		test += 0.1;
		oled.printf(0, 0, 2, "%.1f %.1f    ", test, 1.45);
		oled.printf(0, 2, 2, "%.1f %.1f    ", 1.545, 1.45);
		oled.printf(0, 4, 2, "%.1f %.1f    ", 1.545, 1.45);
		oled.printf(0, 6, 2, "%.1f    ", fps.getFps());

	}

}


