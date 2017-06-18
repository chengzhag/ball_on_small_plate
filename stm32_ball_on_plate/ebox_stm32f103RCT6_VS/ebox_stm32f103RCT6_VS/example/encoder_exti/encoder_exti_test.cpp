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
#include "encoder_exti.h"



EncoderExti encoder1(&PA8, &PA7);

void setup()
{
    ebox_init();
    uart1.begin(115200);
}

int main(void)
{
    setup();
	long long oldPos = 0, nowPos = 0;

    while(1)
    {
		nowPos = encoder1.getPosition();
        uart1.printf("%ld\r\n", nowPos/*- oldPos*/);
		oldPos = nowPos;
        delay_ms(100);
    }
}


