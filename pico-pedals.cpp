#include <stdio.h>
#include <algorithm>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "tusb.h"
#include "bsp/board_api.h"
#include "pico/cyw43_arch.h"

int input_max = 1700;
int output_min = 0;
int output_max = 127;

int scale_value(int input_value, int input_min)
{
    if (input_value < input_min)
    {
        input_value = input_min;
    }

    // Perform the scaling
    int scaled_value = std::min(output_max, ((input_value - input_min) * (output_max - output_min)) / (input_max - input_min) + output_min);

    return scaled_value;
}

int main()
{
    board_init();

    stdio_init_all();

    adc_init();
    adc_gpio_init(27);
    adc_gpio_init(28);

    // test button
    const uint BUTTON_PIN = 2;
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Wifi module for led
    cyw43_arch_init();

    tud_init(BOARD_TUD_RHPORT);

    if (board_init_after_tusb)
    {
        board_init_after_tusb();
    }

    adc_select_input(1);
    int input_min = adc_read();

    adc_select_input(2);
    int input_min2 = adc_read();

    while (true)
    {
        adc_select_input(1);
        volatile uint16_t raw_value = adc_read();
        volatile uint8_t trig_value = scale_value(raw_value, input_min);

        adc_select_input(2);
        volatile uint16_t raw_value2 = adc_read();
        volatile uint8_t trig_value2 = scale_value(raw_value2, input_min2);

        // Print the raw ADC value and corresponding voltage
        // printf("Pedal Value (raw): %u\n", raw_value);
        printf("Pedal out: %u\n", trig_value);
        tud_task();

        bool button_pressed = !gpio_get(BUTTON_PIN);

        // Determine hat value
        uint8_t hat = GAMEPAD_HAT_CENTERED;
        if (button_pressed)
        {
            hat = GAMEPAD_HAT_UP;
        }

        // Show led if pedal pressed
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, raw_value > input_min);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, raw_value2 > input_min);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, hat);

        if (tud_hid_ready())
        {
            hid_gamepad_report_t report = {0};
            report.ry = raw_value;
            report.rx = raw_value2;
            report.hat = hat;

            // Send the report over USB HID
            tud_hid_report(0, &report, sizeof(report));
        }
    }
}
