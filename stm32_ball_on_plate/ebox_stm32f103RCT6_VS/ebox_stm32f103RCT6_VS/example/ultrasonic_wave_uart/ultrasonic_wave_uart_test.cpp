/**
  ******************************************************************************
  * @file   : *.cpp
  * @author : shentq
  * @version: V1.2
  * @date   : 2016/08/14

  * @brief   ebox application example .
  *
  * Copyright 2016 shentq. All Rights Reserved.         
  ******************************************************************************
 */

#include "ebox.h"
#include "ultrasonic_wave_uart.h"

UltrasonicWaveUart sonic(&uart2);

void setup()
{
    ebox_init();
    uart1.begin(115200);
	sonic.begin();
    uart1.printf("ok\n");
}
int main(void)
{
	setup();
	uint64_t time = 0;
    while(1)
    {	
		time = millis();
		sonic.trig();
		//delay_ms(100);
		uart1.printf("%d\t", sonic.read());
		uart1.printf("fps: %f\r\n", 1000.0f / (millis() - time));
    }

}


