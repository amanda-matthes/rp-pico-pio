#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "test.pio.h" // this is generated from test.pio and defines test_program and test_program_get_default_config - use pico_generate_pio_header in CMakeLists.txt

int main()
{
    // LED blink for sanity checks
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    uint32_t delay_ms = 1000;

    stdio_init_all();

    // pick a PIO block and a state machine within that PIO (Pico 2 has 3 PIO blocks with 4 state machines each)
    PIO test_pio = pio0;         // wee will use PIO block 0
    uint test_state_machine = 1; // we will use state machine
    uint test_pin = 4;           // we will use GPIO 4 for the output
    uint test_frequency_Hz = 9;

    // add program to pio instruction memory (on the Pico 2, that can be 32 instructions per PIO)
    uint test_memory_offset = pio_add_program(test_pio, &test_program);
    pio_sm_config pio_state_machine_config = test_program_get_default_config(test_memory_offset);

    // set the test pin as the output of the PIO state machine
    pio_gpio_init(test_pio, test_pin);
    pio_sm_set_consecutive_pindirs(test_pio, test_state_machine, test_pin, 1, true);
    sm_config_set_set_pins(&pio_state_machine_config, test_pin, 1);

    // initialise the state machine
    pio_sm_init(test_pio, test_state_machine, test_memory_offset, &pio_state_machine_config);
    pio_sm_set_enabled(test_pio, test_state_machine, true);

    // set the frequency
    test_pio->txf[test_state_machine] = (SYS_CLK_HZ / (2 * test_frequency_Hz)) - 3;

    // loop to keep going and for LED sanity check
    while (true)
    {
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(delay_ms);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(delay_ms);
    }
}
