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

/* Constants */
constexpr int kRelayGpioPin = 6;
constexpr int kAnalogInputGpioPin = 26;

constexpr int kGreenLedGpioPin = 17;
constexpr int kYellowLedGpioPin = 18;
constexpr int kRedLedGpioPin = 16;

constexpr unsigned kSensorReadingCount = 200;
constexpr unsigned kSensorReadingInterval = 10; // milliseconds

constexpr unsigned kLedBlinkInterval = 300; // milliseconds

constexpr unsigned kWaterLevelLow = 35;
constexpr unsigned kWaterLevelHigh = 65;
constexpr unsigned kWaterLevelElevated = 40;

constexpr unsigned kElevatedLevelCounterThreshold = 300; // approx. 15 minutes

constexpr unsigned kSleepTimeBetweenCheckingSensor = 1000; // milliseconds

void relay_task(int switch_on_off)
{
    gpio_init(kRelayGpioPin);
    gpio_set_dir(kRelayGpioPin, GPIO_OUT);
    if (switch_on_off == 0)
    {
        gpio_put(kRelayGpioPin, 0);
    }
    else
    {
        gpio_put(kRelayGpioPin, 1);
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
    led_task(kGreenLedGpioPin, 1);
    vTaskDelay(kLedBlinkInterval);
    led_task(kGreenLedGpioPin, 0);
    led_task(kYellowLedGpioPin, 1);
    vTaskDelay(kLedBlinkInterval);
    led_task(kYellowLedGpioPin, 0);
    led_task(kRedLedGpioPin, 1);
    vTaskDelay(kLedBlinkInterval);
    led_task(kRedLedGpioPin, 0);
    vTaskDelay(kLedBlinkInterval);
    led_task(kGreenLedGpioPin, 1);
}

unsigned read_adc_task(int gpio)
{
    adc_init();
    adc_gpio_init(gpio);
    adc_select_input(0);

    unsigned readings[kSensorReadingCount];
    for (unsigned i = 0; i < kSensorReadingCount; i++) {
        readings[i] = adc_read();
        vTaskDelay(kSensorReadingInterval); // Delay between each reading
    }

    std::sort(readings, readings + kSensorReadingCount); // Sort the readings in ascending order

    return readings[kSensorReadingCount/2]; // Return the median value (middle value)
}

void vTaskCode( void * pvParameters )
{
    init_led_task();
    unsigned long counterOkReadings{0};
    unsigned long counterErrorReadings{0};
    unsigned counterRelayOn{0};
    unsigned elevatedLevelCounter{0};
    bool currentlyPumping{false};

    while (true)
    {
        auto waterLevel = read_adc_task(kAnalogInputGpioPin);

        if(waterLevel > 0)
        {
            led_task(kYellowLedGpioPin, 1);
            vTaskDelay(kLedBlinkInterval);
            led_task(kYellowLedGpioPin, 0);
            counterOkReadings++;
        }
        else
        {
            led_task(kRedLedGpioPin, 1);
            vTaskDelay(kLedBlinkInterval);
            led_task(kRedLedGpioPin, 0);
            counterErrorReadings++;
        }
        if (waterLevel <= kWaterLevelLow)
        {
            relay_task(0);
            currentlyPumping = false;
        }
        else if (currentlyPumping == false && waterLevel > kWaterLevelHigh)
        {
            relay_task(1);
            currentlyPumping = true;
            counterRelayOn++;
            elevatedLevelCounter = 0;
        }
        else if (currentlyPumping == false && waterLevel > kWaterLevelElevated)
        {
            elevatedLevelCounter++;
            if (elevatedLevelCounter >= kElevatedLevelCounterThreshold)
            {
                relay_task(1);
                currentlyPumping = true;
                counterRelayOn++;
                elevatedLevelCounter = 0;
            }
        }
        else if (currentlyPumping == false)
        {
            if (elevatedLevelCounter > 0)
            {
                elevatedLevelCounter--;
            }
        }
        vTaskDelay(kSleepTimeBetweenCheckingSensor);
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