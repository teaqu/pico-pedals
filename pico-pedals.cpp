#include <stdio.h>
#include <algorithm>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "tusb.h"
#include "bsp/board_api.h"
#include "hardware/uart.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

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

    adc_init();
    adc_gpio_init(27);
    adc_gpio_init(28);

    // UART
    stdio_init_all();
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // test button
    const uint BUTTON_PIN = 2;
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

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
        if (trig_value > 0)
        {
            printf("Pedal out: %u\n", trig_value);
        }
        if (trig_value2 > 0) {
            printf("Pedal 2 out: %u\n", trig_value2);
        }
        tud_task();

        bool button_pressed = !gpio_get(BUTTON_PIN);

        // Determine hat value
        uint8_t hat = GAMEPAD_HAT_CENTERED;
        if (button_pressed)
        {
            hat = GAMEPAD_HAT_UP;
        }

        if (tud_hid_ready())
        {
            hid_gamepad_report_t report = {0};
            report.ry = trig_value;
            report.rx = trig_value2;
            report.hat = hat;

            // Send the report over USB HID
            tud_hid_report(0, &report, sizeof(report));
        }
    }
}
