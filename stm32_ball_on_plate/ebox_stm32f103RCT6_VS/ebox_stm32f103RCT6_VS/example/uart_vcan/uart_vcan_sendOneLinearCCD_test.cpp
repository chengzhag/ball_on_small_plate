#include "ebox.h"
#include "uart_vcan.h"
#include "math.h"
UartVscan uart(&uart1);
void setup()
{
	ebox_init();
	uart.begin(115200);
}
int main()
{
	setup();
	uint8_t x[128];
	for (int i = 0; i < 128; i++)
	{
		x[i] = 256 * sin(float(i)/10);
	}
	while (1)
	{
		uart.sendLinearCCD(x, 128);
	}
}