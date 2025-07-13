#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "hardware/adc.h"
#include "semphr.h"

const uint led_pin_blue = 12;
SemaphoreHandle_t xADCMutex;

void vTaskControleLED(void *pvParameters) 
{
    for (;;) 
    {
        gpio_put(led_pin_blue, 1);
        vTaskDelay(50);
        gpio_put(led_pin_blue, 0);
        vTaskDelay(950);
        printf("Blink\n");
    }
}

void vTaskTemperaturaInterna(void *pvParameters) 
{
    for (;;) 
    {
        if (xSemaphoreTake(xADCMutex, portMAX_DELAY) == pdTRUE) {
            adc_select_input(4);
            uint16_t raw_value = adc_read();
            xSemaphoreGive(xADCMutex);
            
            float voltage = raw_value * 3.3f / (1 << 12);
            float temp_celsius = 27.0f - (voltage - 0.706f) / 0.001721f;
            printf("Temperatura Interna: %.2f °C\n", temp_celsius);
        }
        vTaskDelay(2000);
    }
}

void vTaskJoystick(void *pvParameters)
{
    for(;;)
    {
        if (xSemaphoreTake(xADCMutex, portMAX_DELAY) == pdTRUE) {
            adc_select_input(0);
            uint adc_y_raw = adc_read();
            adc_select_input(1);
            uint adc_x_raw = adc_read();


            if (adc_y_raw > 2500) 
            {
                printf("Cima\n");
            } 
            else if (1500 > adc_y_raw) 
            {
                printf("Baixo\n");
            }
            else if (1500 > adc_x_raw)
            {
                printf("Esquerda\n");
            }
            else if (adc_x_raw > 2600)
            {
                printf("Direita\n");
            }
            
            xSemaphoreGive(xADCMutex);
        }
        vTaskDelay(500);
    }
}

void setup() 
{
    stdio_init_all();
    
    // Cria o mutex para controle do ADC
    xADCMutex = xSemaphoreCreateMutex();
    if (xADCMutex != NULL) 
    {
        // Configuração do LED
        gpio_init(led_pin_blue);
        gpio_set_dir(led_pin_blue, GPIO_OUT);

        // Configuração do ADC
        adc_init();
        adc_set_temp_sensor_enabled(true);  // Ativa o sensor de temperatura antes de outros canais
        
        // Joystick
        adc_gpio_init(26); // Canal 0 (Y)
        adc_gpio_init(27); // Canal 1 (X)
    }
}

int main() 
{
    setup();

    xTaskCreate(vTaskControleLED, "Task LED", 256, NULL, 1, NULL);
    xTaskCreate(vTaskTemperaturaInterna, "Task Temp", 256, NULL, 1, NULL);
    xTaskCreate(vTaskJoystick, "Task Joy", 1024, NULL, 1, NULL);

    vTaskStartScheduler();

    for (;;) {}
}