#include "oled_i2c.h"
#include "font.h"

void OLEDI2C::write_byte(uint8_t reg_address, uint8_t data)
{
	i2c->take_i2c_right(speed);
	i2c->write_byte(adress, reg_address, data);
	i2c->release_i2c_right();
}

void OLEDI2C::write_cmd(unsigned char cmd) //Ð´ÃüÁî
{
	write_byte(0x00, cmd);
}

void OLEDI2C::write_data(unsigned char dat) //Ð´Êý¾Ý
{
	write_byte(0x40, dat);
}

OLEDI2C::OLEDI2C(I2c* i2c, uint8_t adress /*= 0x78*/) :
	i2c(i2c),
	speed(400000),
	adress(adress)
{

}

void OLEDI2C::begin(uint32_t speed /*= 400000*/)
{
	this->speed = speed;
	i2c->take_i2c_right(speed);
	i2c->begin(speed);
	i2c->release_i2c_right();
	delay_ms(100);
	write_cmd(0xAE); //display off
	write_cmd(0x20);	//Set Memory Addressing Mode	
	write_cmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	write_cmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	write_cmd(0xc8);	//Set COM Output Scan Direction
	write_cmd(0x00); //---set low column address
	write_cmd(0x10); //---set high column address
	write_cmd(0x40); //--set start line address
	write_cmd(0x81); //--set contrast control register
	write_cmd(0xff); //ÁÁ¶Èµ÷½Ú 0x00~0xff
	write_cmd(0xa1); //--set segment re-map 0 to 127
	write_cmd(0xa6); //--set normal display
	write_cmd(0xa8); //--set multiplex ratio(1 to 64)
	write_cmd(0x3F); //
	write_cmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	write_cmd(0xd3); //-set display offset
	write_cmd(0x00); //-not offset
	write_cmd(0xd5); //--set display clock divide ratio/oscillator frequency
	write_cmd(0xf0); //--set divide ratio
	write_cmd(0xd9); //--set pre-charge period
	write_cmd(0x22); //
	write_cmd(0xda); //--set com pins hardware configuration
	write_cmd(0x12);
	write_cmd(0xdb); //--set vcomh
	write_cmd(0x20); //0x20,0.77xVcc
	write_cmd(0x8d); //--set DC-DC enable
	write_cmd(0x14); //
	write_cmd(0xaf); //--turn on oled panel
	clear();
}

void OLEDI2C::set_xy(unsigned char x, unsigned char y)
{
	write_cmd(0xb0 + y);
	write_cmd(((x & 0xf0) >> 4) | 0x10);
	write_cmd((x & 0x0f) | 0x01);
}

void OLEDI2C::fill(unsigned char fill_Data)
{
	unsigned char m, n;
	for (m = 0; m < 8; m++)
	{
		write_cmd(0xb0 + m);		//page0-page1
		write_cmd(0x00);		//low column start address
		write_cmd(0x10);		//high column start address
		for (n = 0; n < 128; n++)
		{
			write_data(fill_Data);
		}
	}
}

void OLEDI2C::clear(void)
{
	fill(0x00);
}

void OLEDI2C::display_on(void)
{
	write_cmd(0X8D);  //ÉèÖÃµçºÉ±Ã
	write_cmd(0X14);  //¿ªÆôµçºÉ±Ã
	write_cmd(0XAF);  //OLED»½ÐÑ
}

void OLEDI2C::display_off(void)
{
	write_cmd(0X8D);  //ÉèÖÃµçºÉ±Ã
	write_cmd(0X10);  //¹Ø±ÕµçºÉ±Ã
	write_cmd(0XAE);  //OLEDÐÝÃß
}

void OLEDI2C::show_char(unsigned char x, unsigned char y, char ch, unsigned char TextSize /*= 1*/)
{
	unsigned char c = 0, i = 0;
	switch (TextSize)
	{
	case 1:
	{
		c = ch - 32;
		if (x > 126)
		{
			x = 0;
			y++;
		}
		set_xy(x, y);
		for (i = 0; i < 6; i++)
			write_data(font6x8[c][i]);
		x += 6;
	}break;
	case 2:
	{
		c = ch - 32;
		if (x > 120)
		{
			x = 0;
			y++;
		}
		set_xy(x, y);
		for (i = 0; i < 8; i++)
			write_data(font8x16[c * 16 + i]);
		set_xy(x, y + 1);
		for (i = 0; i < 8; i++)
			write_data(font8x16[c * 16 + i + 8]);
		x += 8;
	}break;
	}
}

void OLEDI2C::show_string(unsigned char x, unsigned char y, char ch[], unsigned char TextSize /*= 1*/)
{
	unsigned char j = 0;
	while (ch[j] != '\0')
	{
		show_char(x, y, ch[j], TextSize);
		x += 8;
		if (x > 120)
		{
			x = 0;
			y += 2;
		}
		j++;
	}
}

void OLEDI2C::printf(uint8_t x, uint8_t y, unsigned char TextSize, const char *fmt, ...)
{
	int     size1 = 0;
	size_t  size2 = 256;

	if (printf_buf != NULL)
		ebox_free(printf_buf);
	va_list va_params;
	va_start(va_params, fmt);

	do {
		printf_buf = (char *)ebox_malloc(size2);
		if (printf_buf == NULL)
			return;
		size1 = _vsnprintf(printf_buf, size2, fmt, va_params);
		if (size1 == -1 || size1 > size2)
		{
			size2 += 128;
			size1 = -1;
			ebox_free(printf_buf);
		}
	} while (size1 == -1);

	vsprintf(printf_buf, fmt, va_params);
	va_end(va_params);
	show_string(x, y, printf_buf, TextSize);
}

void OLEDI2C::show_chinese(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm = 0;
	unsigned int  adder = 32 * N;
	set_xy(x, y);
	for (wm = 0; wm < 16; wm++)
	{
		write_data(font16x16[adder]);
		adder += 1;
	}
	set_xy(x, y + 1);
	for (wm = 0; wm < 16; wm++)
	{
		write_data(font16x16[adder]);
		adder += 1;
	}
}

void OLEDI2C::draw_bmp(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	if (y1 % 8 == 0)
		y = y1 / 8;
	else
		y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		set_xy(x0, y);
		for (x = x0; x < x1; x++)
		{
			write_data(BMP[j++]);
		}
	}
}
