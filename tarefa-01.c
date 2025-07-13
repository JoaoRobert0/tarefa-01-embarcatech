#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "hardware/adc.h"

const uint led_pin_blue = 12;

void vTaskControleLED(void *pvParameters) 
{
    for (;;) 
    {
        gpio_put(led_pin_blue, 1);
        vTaskDelay(50);  // LED ligado por 50ms
        gpio_put(led_pin_blue, 0);
        vTaskDelay(950); // LED desligado por 950ms
        printf("Blinking\n");
    }
}

void vTaskTemperaturaInterna(void *pvParameters) 
{
    for (;;) 
    {
        // Lê o valor bruto do ADC (12 bits, 0-4095)
        uint16_t raw_value = adc_read();

        // Converte para tensão (referência: 3.3V)
        float voltage = raw_value * 3.3f / (1 << 12);

        // Fórmula do datasheet do RP2040
        float temp_celsius = 27.0f - (voltage - 0.706f) / 0.001721f;

        // Exibe no terminal
        printf("Temperatura Interna: %.2f °C\n", temp_celsius);

        // Delay de 2 segundos
        vTaskDelay(2000);
    }
}

void setup() 
{
    stdio_init_all();

    gpio_init(led_pin_blue);
    gpio_set_dir(led_pin_blue, GPIO_OUT);

    adc_init();
    adc_set_temp_sensor_enabled(true);  // Ativa o sensor de temperatura
    adc_select_input(4);                // Canal ADC4 é o sensor interno
}

int main() 
{
    setup();

    xTaskCreate(vTaskControleLED, "Task 1", 128, NULL, 1, NULL);
    xTaskCreate(vTaskTemperaturaInterna, "Task 2", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    for (;;) 
    {

    }
}