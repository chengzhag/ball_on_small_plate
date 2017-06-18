#include "ebox.h"
#include "uart_vcan.h"
#include "math.h"
UartVscan uart(&uart1);
void setup()
{
	ebox_init();
	uart.begin(115200);
}
void main()
{
	setup();
	float x, y;
	while (1)
	{
		for (x = 0; x < 100; x += 0.01)
		{
			y = sin(x);
			uart.sendOscilloscope(y);
		}
	}
}