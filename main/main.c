#include <stdio.h>
#include <delay.h>

#include "lcd.h"
#include "lcd_init.h"
#include "Drivers/LED/led.h"

#include "esp_task_wdt.h"

#include "celeste.h"

extern uint16_t display_mem[128][128];

void app_main(void)
{
    esp_task_wdt_deinit();

    printf("hello\n");
    LedGpioConfing();

    LCD_Init();
    display_all();

    Celeste_P8_set_rndseed(666);

    Celeste_P8_init();

    while (1)
    {
        Celeste_P8_update();
        Celeste_P8_draw();
        display_all();
        delay_ms(33);
    }
}
