#include "lcd_init.h"
#include "delay.h"

spi_device_handle_t spi_port;

// This function is called (in irq context!) just before a transmission starts. It will
// set the D/C line to the value indicated in the user field.
void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
	int dc = (int)t->user;

	gpio_set_level(LCD_DC_PIN, dc);
}

void LCD_Peripherals_Init(void)
{
	esp_err_t ret;

	spi_bus_config_t buscfg = {
		.miso_io_num = NULL,
		.mosi_io_num = LCD_MOSI_PIN,
		.sclk_io_num = LCD_SCL_PIN,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = 128 * 128 * 16 // 最大传输大小
	};
	spi_device_interface_config_t devcfg = {

		.clock_speed_hz = 80 * 1000 * 1000,		 // Clock out at 80 MHz
		.mode = 3,								 // SPI mode 3
		.spics_io_num = LCD_CS_PIN,				 // CS pin
		.queue_size = 7,						 // 事务队列尺寸 7个
		.pre_cb = lcd_spi_pre_transfer_callback, // 数据传输前回调，用作D/C（数据命令）线分别处理
	};
	// 初始化SPI总线
	ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
	ESP_ERROR_CHECK(ret);

	// 添加SPI总线驱动
	ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi_port);
	ESP_ERROR_CHECK(ret);

	gpio_reset_pin(LCD_RES_PIN);						   // 初始化引脚
	gpio_set_direction(LCD_RES_PIN, GPIO_MODE_DEF_OUTPUT); // 配置引脚为输出模式

	gpio_reset_pin(LCD_DC_PIN);							  // 初始化引脚
	gpio_set_direction(LCD_DC_PIN, GPIO_MODE_DEF_OUTPUT); // 配置引脚为输出模式

	gpio_reset_pin(LCD_BLK_PIN);						   // 初始化引脚
	gpio_set_direction(LCD_BLK_PIN, GPIO_MODE_DEF_OUTPUT); // 配置引脚为输出模式
}

/******************************************************************************
	  函数说明：LCD串行数据写入函数
	  入口数据：dat  要写入的串行数据
	  返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat)
{
	esp_err_t ret;
	spi_transaction_t t = {0};
	t.length = 1 * 8;								 // 要写入的数据长度 Len 是字节数，len, transaction length is in bits.
	t.tx_buffer = &dat;								 // 数据指针
	t.user = (void *)1;								 // 设置D/C 线，在SPI传输前回调中根据此值处理DC信号线
	ret = spi_device_polling_transmit(spi_port, &t); // 开始传输
	assert(ret == ESP_OK);							 // 一般不会有问题
}

/******************************************************************************
	  函数说明：LCD写入数据
	  入口数据：dat 写入的数据
	  返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}

/******************************************************************************
	  函数说明：LCD写入数据
	  入口数据：dat 写入的数据
	  返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat >> 8);
	LCD_Writ_Bus(dat);
}

/******************************************************************************
	  函数说明：LCD写入命令
	  入口数据：dat 写入的命令
	  返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	esp_err_t ret;
	spi_transaction_t t = {0};
	t.length = 8;									 // 要传输的位数 一个字节 8位
	t.tx_buffer = &dat;								 // 将命令填充进去
	t.user = (void *)0;								 // 设置D/C 线，在SPI传输前回调中根据此值处理DC信号线
	ret = spi_device_polling_transmit(spi_port, &t); // 开始传输
	assert(ret == ESP_OK);							 // 一般不会有问题
}

/******************************************************************************
	  函数说明：设置起始和结束地址
	  入口数据：x1,x2 设置列的起始和结束地址
				y1,y2 设置行的起始和结束地址
	  返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
	y1 += 2;
	y2 += 2;
	if (USE_HORIZONTAL == 0)
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1 + 2);
		LCD_WR_DATA(x2 + 2);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1 + 1);
		LCD_WR_DATA(y2 + 1);
		LCD_WR_REG(0x2c); // 储存器写
	}
	else if (USE_HORIZONTAL == 1)
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1 + 2);
		LCD_WR_DATA(x2 + 2);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1 + 1);
		LCD_WR_DATA(y2 + 1);
		LCD_WR_REG(0x2c); // 储存器写
	}
	else if (USE_HORIZONTAL == 2)
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1 + 1);
		LCD_WR_DATA(x2 + 1);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1 + 2);
		LCD_WR_DATA(y2 + 2);
		LCD_WR_REG(0x2c); // 储存器写
	}
	else
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1 + 1);
		LCD_WR_DATA(x2 + 1);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1 + 2);
		LCD_WR_DATA(y2 + 2);
		LCD_WR_REG(0x2c); // 储存器写
	}
}

void send_init_command()
{
	LCD_RES_Clr(); // 复位
	delay_ms(100);
	LCD_RES_Set();
	delay_ms(100);

	LCD_BLK_Set(); // 打开背光
	delay_ms(100);

	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11); // Sleep out
	delay_ms(120);	  // Delay 120ms
	//------------------------------------ST7735S Frame Rate-----------------------------------------//
	LCD_WR_REG(0xB1);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_REG(0xB3);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	//------------------------------------End ST7735S Frame Rate---------------------------------//
	LCD_WR_REG(0xB4); // Dot inversion
	LCD_WR_DATA8(0x03);
	//------------------------------------ST7735S Power Sequence---------------------------------//
	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x04);
	LCD_WR_REG(0xC1);
	LCD_WR_DATA8(0XC0);
	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0x2A);
	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0xEE);
	//---------------------------------End ST7735S Power Sequence-------------------------------------//
	LCD_WR_REG(0xC5); // VCOM
	LCD_WR_DATA8(0x1A);
	LCD_WR_REG(0x36); // MX, MY, RGB mode
	if (USE_HORIZONTAL == 0)
		LCD_WR_DATA8(0x00);
	else if (USE_HORIZONTAL == 1)
		LCD_WR_DATA8(0xC0);
	else if (USE_HORIZONTAL == 2)
		LCD_WR_DATA8(0x70);
	else
		LCD_WR_DATA8(0xA0);
	//------------------------------------ST7735S Gamma Sequence---------------------------------//
	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x22);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x2E);
	LCD_WR_DATA8(0x30);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x2A);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x2E);
	LCD_WR_DATA8(0x3A);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x13);
	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x16);
	LCD_WR_DATA8(0x06);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x2D);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x23);
	LCD_WR_DATA8(0x27);
	LCD_WR_DATA8(0x27);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x2D);
	LCD_WR_DATA8(0x3B);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x13);
	//------------------------------------End ST7735S Gamma Sequence-----------------------------//
	LCD_WR_REG(0x3A); // 65k mode
	LCD_WR_DATA8(0x05);
	LCD_WR_REG(0x29); // Display on
}

void send_init_command2()
{
	LCD_RES_Clr(); // 复位
	delay_ms(100);
	LCD_RES_Set();
	delay_ms(100);

	// LCD Init For 1.44Inch LCD Panel with ST7735R.
	LCD_WR_REG(0x11); // Sleep exit
	delay_ms(120);
	LCD_WR_REG(0x11); // Sleep exit
	delay_ms(120);

	LCD_WR_REG(0xB1);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);

	LCD_WR_REG(0xB3);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);

	LCD_WR_REG(0xB4); // Dot inversion
	LCD_WR_DATA8(0x03);

	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x04);

	LCD_WR_REG(0xC1);
	LCD_WR_DATA8(0xC5);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x0d);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0x2A);

	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0xEE);

	LCD_WR_REG(0xC5); // VCOM
	LCD_WR_DATA8(0x06);  // 1D  .06

	LCD_WR_REG(0x36); // MX, MY, RGB mode
	LCD_WR_DATA8(0xc8);  // c0

	LCD_WR_REG(0x3A); // MX, MY, RGB mode
	LCD_WR_DATA8(0x55);  // c0

	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0x0b);
	LCD_WR_DATA8(0x17);
	LCD_WR_DATA8(0x0a);
	LCD_WR_DATA8(0x0d);
	LCD_WR_DATA8(0x1a);
	LCD_WR_DATA8(0x19);
	LCD_WR_DATA8(0x16);
	LCD_WR_DATA8(0x1d);
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x37);
	LCD_WR_DATA8(0x3c);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x10);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0x0c);
	LCD_WR_DATA8(0x19);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x0d);
	LCD_WR_DATA8(0x1b);
	LCD_WR_DATA8(0x19);
	LCD_WR_DATA8(0x15);
	LCD_WR_DATA8(0x1d);
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x39);
	LCD_WR_DATA8(0x3E);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x10);

	delay_ms(120);
	LCD_WR_REG(0x29); // Display on
}

void LCD_Init(void)
{
	LCD_Peripherals_Init(); // 初始化GPIO/SPI
	send_init_command();
}
