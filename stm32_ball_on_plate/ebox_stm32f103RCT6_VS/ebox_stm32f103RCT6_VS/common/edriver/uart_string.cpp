#include "uart_string.h"

void UartString::rxEvent()
{
	if (bufferIndex < UART_STRING_BUFFER_SIZE - 1)
	{
		uint8_t c;
		c = uart->read();
		buffer[bufferIndex] = c;
		bufferIndex++;
		if (c == '\n' || bufferIndex == UART_STRING_BUFFER_SIZE - 1)
		{
			buffer[bufferIndex] = '\0';
			stringEvent.call(buffer);
			bufferIndex = 0;
		}
	}
}

UartString::UartString(Uart *uartX) :
	bufferIndex(0),
	uart(uartX)
{
	//buffer = new char[bufferSize];//³¢ÊÔ¶¯Ì¬·ÖÅäÄÚ´æ
	//this->bufferSize = bufferSize;
}

void UartString::begin(uint32_t baud_rate, uint8_t data_bit, uint8_t parity, float stop_bit, uint8_t _use_dma)
{
	uart->begin(baud_rate, data_bit, parity, stop_bit, _use_dma);
	uart->attach(this, &UartString::rxEvent, RxIrq);
}

void UartString::begin(uint32_t baud_rate, uint8_t _use_dma /*= 1*/)
{
	uart->begin(baud_rate, _use_dma);
	uart->attach(this, &UartString::rxEvent, RxIrq);
}

void UartString::printf(const char *fmt, ...)
{
	uart->printf(fmt);
}

void UartString::attach(void(*stringEvent)(char *str))
{
	this->stringEvent.attach(stringEvent);
}
