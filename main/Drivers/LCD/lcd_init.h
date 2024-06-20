#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "stdlib.h"
#include "string.h"


#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"


#define USE_HORIZONTAL 1  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#define LCD_W 128
#define LCD_H 128

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

#define LCD_SCL_PIN     14
#define LCD_MOSI_PIN    13
#define LCD_RES_PIN     12
#define LCD_DC_PIN      11
#define LCD_CS_PIN      10
#define LCD_BLK_PIN     9


//-----------------LCD端口定义---------------- 

#define LCD_RES_Clr()  gpio_set_level(LCD_RES_PIN,0)//RES
#define LCD_RES_Set()  gpio_set_level(LCD_RES_PIN,1)

#define LCD_DC_Clr()   gpio_set_level(LCD_DC_PIN,0)//DC
#define LCD_DC_Set()   gpio_set_level(LCD_DC_PIN,1)

#define LCD_BLK_Clr()  gpio_set_level(LCD_BLK_PIN,0)//BLK
#define LCD_BLK_Set()  gpio_set_level(LCD_BLK_PIN,1)


// void delay_us(int us);
void delay_ms(int ms);
void LCD_Peripherals_Init(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
#endif




