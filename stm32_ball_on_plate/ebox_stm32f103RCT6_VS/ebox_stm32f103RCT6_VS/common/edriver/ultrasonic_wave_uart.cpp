#include "ultrasonic_wave_uart.h"

UltrasonicWaveUart::UltrasonicWaveUart(Uart *uartX) :
	uart(uartX),
	dis(0),
	isHigh(true),
	isReady(false)
{

}

void UltrasonicWaveUart::begin()
{
	uart->begin(9600);
	uart->attach(this, &UltrasonicWaveUart::rxEvent, RxIrq);
}

void UltrasonicWaveUart::rxEvent()
{
	if (isHigh)
	{
		isHigh = false;
		uint8_t c = uart->read();
		dis = c * 256;
	}
	else
	{
		isHigh = true;
		uint8_t c = uart->read();
		dis = dis + c;
		isReady = true;
		dataRecieveEvent.call(dis);
	}
}

uint16_t UltrasonicWaveUart::read()
{
	while (!isReady)
	{
	}
	isReady = false;
	return dis;
}

void UltrasonicWaveUart::attach(void(*dataRecieveEvent)(uint16_t))
{
	this->dataRecieveEvent.attach(dataRecieveEvent);
}

void UltrasonicWaveUart::trig()
{
	uart->write(0x55);
}
