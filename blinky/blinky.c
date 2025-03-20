#include <avr/io.h>
#include <util/delay.h>

#define DELAY_TIME      500

static void blink()
{
    PORTB |= (1 << PORTB5);
    _delay_ms(DELAY_TIME);

    PORTB = PORTB & ~(1 << PORTB5);
    _delay_ms(DELAY_TIME);
}

int main(void)
{
    DDRB |= (1 << DDB5);

    while (1)
    {
        blink();
    }
}