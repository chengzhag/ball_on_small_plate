#include "ebox.h"
#include "servo.h"
Servo servo(&PA10);
void setup()
{
    ebox_init();
    servo.begin();
	uart1.begin(115200);
}
int main(void)
{
    setup();
	servo.setPct(50);
    while(1)
    {
        uart1.printf("%f\n",servo.getPct());
        delay_ms(1000);
    }
}