#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "test.pio.h" // this is generated from test.pio and defines test_program and test_program_get_default_config - use pico_generate_pio_header in CMakeLists.txt

#include "pin_toggle.pio.h"

/**
 * PIO example: test
 */
void pio_test()
{
    // pick a PIO block and a state machine within that PIO (Pico 2 has 3 PIO blocks with 4 state machines each)
    PIO test_pio = pio0;
    uint test_state_machine = 1;
    uint test_pin = 2;

    // add program to pio instruction memory (on the Pico 2, that can be 32 instructions per PIO)
    uint test_memory_offset = pio_add_program(test_pio, &test_program);
    pio_sm_config test_state_machine_config = test_program_get_default_config(test_memory_offset);

    // slow down clock
    sm_config_set_clkdiv_int_frac8(&test_state_machine_config, 10, 0);

    // set the test pin as the output of the PIO state machine
    pio_gpio_init(test_pio, test_pin);
    pio_sm_set_consecutive_pindirs(test_pio, test_state_machine, test_pin, 1, true);
    sm_config_set_set_pins(&test_state_machine_config, test_pin, 1);

    // initialise the state machine
    pio_sm_init(test_pio, test_state_machine, test_memory_offset, &test_state_machine_config);
    pio_sm_set_enabled(test_pio, test_state_machine, true);

    // add something to the FIFO of the state machine
    test_pio->txf[test_state_machine] = 0xAFAFAFAF;
}

/**
 * PIO example: Toggle GPIO pin 2 at 1 MHz using state machine 1 in PIO block 0
 */
void pio_pin_toggle()
{
    PIO pin_toggle_pio = pio0;         // wee will use PIO block 0
    uint pin_toggle_state_machine = 1; // we will use state machine 1
    uint pin_toggle_pin = 2;           // we will toggle GPIO pin 2
    uint pin_toggle_frequency_Hz = 1000000;

    // add program to pio instruction memory
    uint pin_toggle_memory_offset = pio_add_program(pin_toggle_pio, &pin_toggle_program);
    pio_sm_config pin_toggle_state_machine_config = pin_toggle_program_get_default_config(pin_toggle_memory_offset);

    // set the output pin of the PIO state machine
    pio_gpio_init(pin_toggle_pio, pin_toggle_pin);
    pio_sm_set_consecutive_pindirs(pin_toggle_pio, pin_toggle_state_machine, pin_toggle_pin, 1, true);
    sm_config_set_set_pins(&pin_toggle_state_machine_config, pin_toggle_pin, 1);

    // initialise the state machine
    pio_sm_init(pin_toggle_pio, pin_toggle_state_machine, pin_toggle_memory_offset, &pin_toggle_state_machine_config);
    pio_sm_set_enabled(pin_toggle_pio, pin_toggle_state_machine, true);

    // set the frequency by filling the FIFO of the state machine
    pin_toggle_pio->txf[pin_toggle_state_machine] = (SYS_CLK_HZ / (2 * pin_toggle_frequency_Hz)) - 3;
}

/**
 * PIO example: Move data from the TX FIFO to a GPIO pin
 */
void pio_move_data_TX_to_GPIO()
{
    // TODO
}

int main()
{
    // LED blink for sanity checks
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    uint32_t delay_ms = 1000;

    // PIO example - uncomment ONE SINGLE example at a time (they may conflict on using the same PIO block / state machine / GPIO pins)
    // pio_test();
    pio_pin_toggle();

    // loop to keep going and for LED sanity check
    // (some examples may not reach this)
    while (true)
    {
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(delay_ms);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(delay_ms);
    }
}
