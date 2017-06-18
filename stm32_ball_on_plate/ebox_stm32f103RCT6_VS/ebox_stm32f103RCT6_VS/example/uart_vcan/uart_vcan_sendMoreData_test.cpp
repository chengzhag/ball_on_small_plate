#include "ebox.h"
#include "uart_vcan.h"
#include "math.h"
UartVscan uart(&uart1);
void setup()
{
	ebox_init();
	uart.begin(115200);
}
int main(void)
{
	setup();
	float x, y;
	while(1)
	{
		for (x = 0; x < 100; x += 0.01)
		{
			y = sin(x);
			float M[] = { x,y };
			uart.sendOscilloscope(M,2); 
		}
	}
}