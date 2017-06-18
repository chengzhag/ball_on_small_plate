#include "uart_vcan.h"

UartVscan::UartVscan(Uart *uartX) :
	uart(uartX)
{

}

void UartVscan::begin(uint32_t baud_rate, uint8_t _use_dma /*= 1*/)
{
	uart->begin(baud_rate, _use_dma);
}

void UartVscan::begin(uint32_t baud_rate, uint8_t data_bit, uint8_t parity, float stop_bit, uint8_t _use_dma)
{
	uart->begin(baud_rate, data_bit, parity, stop_bit, _use_dma);
}

void UartVscan::sendGreyImage(uint8_t* imgaddr, uint32_t width, uint32_t height)
{
	cmd = 1;
	uart->write(cmd);
	uart->write(~cmd);
	for (uint8_t i = 0; i < width; i++)
		for (uint8_t j = 0; j < height; j++)
			uart->write(imgaddr[i*width + j]);
	uart->write(~cmd);
	uart->write(cmd);
}

void UartVscan::sendLinearCCD(uint8_t *ccdaddr, uint8_t *ccdaddr2, int ccdsize)
{
	cmd = 2;
	uart->write(cmd);
	uart->write(~cmd);
	for (int i = 0; i < ccdsize; i++)
		uart->write(ccdaddr[i]);
	for (int i = 0; i < ccdsize; i++)
		uart->write(ccdaddr2[i]);
	uart->write(~cmd);
	uart->write(cmd);
}

void UartVscan::sendLinearCCD(uint8_t *ccdaddr, int ccdsize)
{
	cmd = 2;
	uart->write(cmd);
	uart->write(~cmd);
	for (int i = 0; i < ccdsize; i++)
		uart->write(ccdaddr[i]);
	uart->write(~cmd);
	uart->write(cmd);
}
