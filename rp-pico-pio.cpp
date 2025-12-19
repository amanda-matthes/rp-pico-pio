#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "blink.pio.h"

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq)
{
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);

    printf("Blinking pin %d at %d Hz\n", pin, freq);

    // PIO counter program takes 3 more cycles in total than we pass as
    // input (wait for n + 1; mov; jmp)
    pio->txf[sm] = (125000000 / (2 * freq)) - 3;
}

int main()
{
    stdio_init_all();

    PIO pio = pio0;
    uint offset = pio_add_program(pio, &blink_program);
    printf("Loaded program at %d\n", offset);

    uint pin2 = 2;
    uint pin3 = 3;

    blink_pin_forever(pio, 0, offset, pin2, 3);
    blink_pin_forever(pio, 1, offset, pin3, 3);

    // For more pio examples see https://github.com/raspberrypi/pico-examples/tree/master/pio
    while (true)
    {
    }
}
