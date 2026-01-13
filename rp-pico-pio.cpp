#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "test.pio.h" // this is generated from test.pio and defines test_program and test_program_get_default_config - use pico_generate_pio_header in CMakeLists.txt

#include "pin_toggle.pio.h"
#include "pin_toggle_basic.pio.h"

/**
 * PIO example: test
 */
void pio_test()
{
    // pick a PIO block and a state machine within that PIO (Pico 2 has 3 PIO blocks with 4 state machines each)
    PIO pio_block = pio0;         // PIO block 0
    uint state_machine_index = 1; // state machine 1
    uint pin = 2;                 // GPIO pin 2

    // add program to pio instruction memory
    uint memory_offset = pio_add_program(pio_block, &test_program);
    pio_sm_config state_machine_config = test_program_get_default_config(memory_offset);

    // set the output pin of the PIO state machine
    pio_gpio_init(pio_block, pin);
    pio_sm_set_consecutive_pindirs(pio_block, state_machine_index, pin, 1, true);
    sm_config_set_set_pins(&state_machine_config, pin, 1);

    // slow down clock to adjust frequency
    uint32_t required_state_machine_clock_Hz = 1000000; // 1 MHz
    float clock_divider = SYS_CLK_HZ / required_state_machine_clock_Hz;
    sm_config_set_clkdiv(&state_machine_config, clock_divider);

    // initialise the state machine
    pio_sm_init(pio_block, state_machine_index, memory_offset, &state_machine_config);
    pio_sm_set_enabled(pio_block, state_machine_index, true);

    // add something to the FIFO of the state machine
    pio_block->txf[state_machine_index] = 0xAFAFAFAF;
}

/**
 * PIO example: Toggle GPIO pin by slowing down the state machine clock
 */
void pio_pin_toggle_basic()
{
    PIO pio_block = pio0;         // use PIO block 0
    uint state_machine_index = 1; // use state machine 1
    uint pin = 2;                 // use GPIO pin 2

    uint pin_toggle_frequency_Hz = 500000; // 500 kHz

    // add program to pio instruction memory
    uint memory_offset = pio_add_program(pio_block, &pin_toggle_basic_program);
    pio_sm_config state_machine_config = pin_toggle_basic_program_get_default_config(memory_offset);

    // set the output pin of the PIO state machine
    pio_gpio_init(pio_block, pin);
    pio_sm_set_consecutive_pindirs(pio_block, state_machine_index, pin, 1, true);
    sm_config_set_set_pins(&state_machine_config, pin, 1);

    // slow down clock to adjust frequency
    uint32_t required_state_machine_clock_Hz = 6 * pin_toggle_frequency_Hz; // 3 cycles high, 3 cycles low
    float clock_divider = SYS_CLK_HZ / required_state_machine_clock_Hz;
    sm_config_set_clkdiv(&state_machine_config, clock_divider);

    // initialise the state machine
    pio_sm_init(pio_block, state_machine_index, memory_offset, &state_machine_config);
    pio_sm_set_enabled(pio_block, state_machine_index, true);
}

/**
 * PIO example: Toggle GPIO pin 2 at 1 MHz using state machine 1 in PIO block 0
 */
void pio_pin_toggle()
{
    PIO pin_toggle_pio = pio0;         // use PIO block 0
    uint pin_toggle_state_machine = 1; // use state machine 1
    uint pin_toggle_pin = 2;           // use GPIO pin 2
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
    // for this example we need to provide the delay between each toggle
    /**
     * T_system = 1 / f_system // system clock period
     * T_target = 1 / f_target // target period
     *
     * toggle_delay = T_toggle / 2
     *
     * cycles_between_toggles = toggle_delay / T_system
     *                        = (T_target / 2) / T_system
     *                        = f_system / (2 * f_target)
     *
     * this PIO example has 3 cycles of overhead per toggle
     * so we subtract 3 from the result
     */
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
    pio_pin_toggle_basic();
    // pio_pin_toggle();

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
