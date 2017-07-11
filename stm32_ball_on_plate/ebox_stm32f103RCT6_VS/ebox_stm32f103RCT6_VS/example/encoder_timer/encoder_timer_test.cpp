#include "ebox.h"
#include "encoder_timer.h"
EncoderTimer encoder(TIM4);
void setup()
{
	ebox_init();
	encoder.begin();
	uart1.begin(115200);
}
int main(void)
{
	setup();

	while (1)
	{
		encoder.refresh();
		long pos = encoder.getPos();
		short diff = encoder.getDiff();
		uart1.printf("%ld\t%d\t\r\n", pos, diff);
		delay_ms(50);
	}
}
