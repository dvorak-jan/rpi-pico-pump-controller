#include <stdio.h>
#include <iostream>
#include <algorithm> // for std::sort
#include <array>

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "GPIO.hpp"
#include "hardware/adc.h"

void relay_task(int switch_on_off)
{
    int gpio = 6;
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
    if (switch_on_off == 0)
    {
        gpio_put(gpio, 0);
    }
    else
    {
        gpio_put(gpio, 1);
    }
}

void led_task(int gpio, int switch_on_off)
{
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
    if (switch_on_off == 0)
    {
        gpio_put(gpio, 0);
    }
    else
    {
        gpio_put(gpio, 1);
    }
}

void init_led_task()
{
    led_task(17, 1);
    vTaskDelay(300);
    led_task(17, 0);
    led_task(18, 1);
    vTaskDelay(300);
    led_task(18, 0);
    led_task(16, 1);
    vTaskDelay(300);
    led_task(16, 0);
    vTaskDelay(300);
    led_task(17, 1);
}

unsigned read_adc_task(int gpio)
{
    adc_init();
    adc_gpio_init(gpio);
    adc_select_input(0);

    unsigned readings[200];
    for (int i = 0; i < 200; i++) {
        readings[i] = adc_read();
        vTaskDelay(10); // Delay for 10 milliseconds between each reading
    }

    std::sort(readings, readings + 200); // Sort the readings in ascending order

    return readings[100]; // Return the median value (middle value)
}

void vTaskCode( void * pvParameters )
{
    init_led_task();
    unsigned counterOkReadings{0};
    unsigned counterErrorReadings{0};
    unsigned counterRelayOn{0};
    unsigned elevatedLevelCounter{0};
    bool currentlyPumping{false};

    while (true)
    {
        auto waterLevel = read_adc_task(26);

        if(waterLevel > 0)
        {
            led_task(18, 1);
            vTaskDelay(300);
            led_task(18, 0);
            counterOkReadings++;
        }
        else
        {
            led_task(16, 1);
            vTaskDelay(300);
            led_task(16, 0);
            counterErrorReadings++;
        }
        if (waterLevel <= 35) // < 35
        {
            relay_task(0);
            currentlyPumping = false;
            elevatedLevelCounter = 0;
        }
        else if (currentlyPumping == false && waterLevel > 65) // 110
        {
            relay_task(1);
            currentlyPumping = true;
            counterRelayOn++;
            elevatedLevelCounter = 0;
        }
        else if (currentlyPumping == false && waterLevel > 40)
        {
            elevatedLevelCounter++;
            if (elevatedLevelCounter >= 900)
            {
                relay_task(1);
                currentlyPumping = true;
                counterRelayOn++;
                elevatedLevelCounter = 0;
            }
        }
        else
        {
            elevatedLevelCounter = 0;
        }
        vTaskDelay(1000);
        printf("Counter OK readings: %d\nCounter error readings: %d\ncounter relay on: %d\n", counterOkReadings, counterErrorReadings, counterRelayOn);
    }
}

int main() {

BaseType_t xReturned;
TaskHandle_t xHandle = NULL;



/* Create the task, storing the handle. */
    xReturned = xTaskCreate(
                    vTaskCode,       /* Function that implements the task. */
                    "Pump task",     /* Text name for the task. */
                    512,             /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &xHandle );   

  vTaskStartScheduler();
  while(1)
  {
      configASSERT(0);    /* We should never get here */
  }

}