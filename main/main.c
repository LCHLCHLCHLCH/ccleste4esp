#include <stdio.h>
#include <delay.h>

#include "lcd.h"
#include "lcd_init.h"
#include "Drivers/LED/led.h"
#include "Drivers/button/button.h"
#include "esp32_frontend.h"

#include "esp_task_wdt.h"

void app_main(void)
{
    esp_task_wdt_deinit();

    printf("Celeste ESP32 starting\n");
    LedGpioConfing();
    button_gpio_init();
    LCD_Init();

    // 设置 PICO-8 回调
    Celeste_P8_set_call_func(pico8_emu);

    Celeste_P8_set_rndseed(666);
    Celeste_P8_init();

    while (1)
    {
        Celeste_P8_update();
        Celeste_P8_draw();
        celeste_render_finish();
        delay_ms(33);
    }
}
