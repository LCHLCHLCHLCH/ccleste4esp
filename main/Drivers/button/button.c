#include "button.h"

void button_gpio_init()
{
    gpio_config_t io_conf;                                                                                    // 定义GPIO初始化结构体
    io_conf.mode = GPIO_MODE_INPUT;                                                                           // 输入模式
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;                                                                  // 使能上拉
    io_conf.pin_bit_mask = (1ULL << 1) | (1ULL << 2) | (1ULL << 3) | (1ULL << 4) | (1ULL << 5) | (1ULL << 6); // 设置引脚为GPIO0
    gpio_config(&io_conf);                                                                                    // 将以上配置写入寄存器
}