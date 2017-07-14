#include "ebox.h"
#include <math.h>
#include "my_math.h"
#include "uart_vcan.h"
#include "signal_stream.h"
#define  pi  3.14159265
FpsCounter fps;
AverageFilter filter(100, 1);
UartVscan vscan(&uart1);

void setup()
{
	uart1.begin(115200);
	vscan.begin(115200);
	PA8.mode(OUTPUT_PP);
}

int main()
{
	float signal[3];
	float t[500];
	int f0 = 1;
	int fs = 100;
	float y[3];
	setup();
	while (1)
	{
		for (int i = 0; i < 500; i++)
		{
			signal[0] = sin(2 * pi*f0*i / fs) * 100;
			signal[1] = rand() % 200 - 100;
			y[0] = 0.2*signal[1] + signal[0];
			y[1] = filter.getFilterOut(y[0]);
			vscan.sendOscilloscope(y, 2);
			//uart1.printf("%.4f\r\n", signal[1]);
		}
		PA8.toggle();
		//printf("%.3f", fps.getFps());
	}
	return 1;
}