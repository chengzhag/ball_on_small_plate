#include "ebox.h"
#include "led.h"
#include "servo.h"
#include "uart_num.h"

Servo servo1(&PB0, 100, 0.8, 2.2);
Servo servo2(&PB1, 100, 0.8, 2.2);
UartNum<int, 2> uartNum(&uart2);

void posReceiveEvent(UartNum<int, 2>* uartNum)
{
	if (uartNum->getLength() == 2)
	{
		int posX = uartNum->getNum()[0];
		int posY = uartNum->getNum()[1];
		uart1.printf("%d\t%d\r\n", posX, posY);

		//unsigned char* posChar = (unsigned char*)uartNum->getNum();
		//for (int i = 0; i < 8; i++)
		//{
		//	uart1.printf("%u ", posChar[i]);
		//}
		//uart1.printf("\r\n");
	}
}

void setup()
{
	ebox_init();
	uart1.begin(115200);
	servo1.begin();
	servo2.begin();
	uartNum.begin(115200);
	uartNum.attach(posReceiveEvent);
}
int main(void)
{
	setup();
	//float pct = 0, increase = 1;
	while (1)
	{
		//pct += increase;
		//if (pct >= 100 || pct <= 0)
		//{
		//	increase = -increase;
		//}
		//uart1.begin(115200);
		//servo1.setPct(pct);
		//servo2.setPct(pct);
		////servo1.setPct(50);
		////servo2.setPct(50);

		//uart1.printf("%f\r\n", servo1.getPct());
		//delay_ms(10);
	}

}


