#include "ebox.h"
#include "encoder_motor.h"

EncoderMotor motor(TIM4,&PC8,&PC7,&PA2,Encoder_Motor_Target_Speed,0.005);

void setup()
{
	ebox_init();
	uart1.begin(115200);
	motor.begin(1, 0, 0);
}
int main(void)
{
	setup();
	float speed = 0, increase = 0.1;
	while (1)
	{
		speed += increase;
		if (speed >= 50 || speed <= -50)
		{
			increase = -increase;
		}

		motor.setSpd(speed);
		motor.refresh();

		uart1.printf("%d\r\n", motor.getPosDiff());
		delay_ms(5);

	}

}


