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
	uint8_t x[128];
	for (int i = 0; i < 128; i++)
		x[i] =256* sin(float(i)/10);
	uint8_t y[128];
	for (int i = 0; i < 128; i++)
		y[i] =256* cos(float(i)/10);
	while (1)
	{
		uart.sendLinearCCD(x, y, 128);
	}
}


