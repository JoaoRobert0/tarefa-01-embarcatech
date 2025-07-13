#include "pico/stdlib.h"

#include "FreeRTOS.h"

#include "task.h"

#include <stdio.h>

const uint led_pin_blue = 12;

void vBlinkTask()
{

    for (;;)
    {

        gpio_put(led_pin_blue, 1);

        vTaskDelay(50);

        gpio_put(led_pin_blue, 0);

        vTaskDelay(950);

        printf("Blinking\n");
    }
}

void setup() {
    stdio_init_all();

    gpio_init(led_pin_blue);

    gpio_set_dir(led_pin_blue, GPIO_OUT);
}

void main()
{
    setup();

    xTaskCreate(vBlinkTask, "Blink Task", 128, NULL, 1, NULL);

    vTaskStartScheduler();
}