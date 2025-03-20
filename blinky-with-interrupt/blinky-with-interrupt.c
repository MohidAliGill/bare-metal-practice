#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU                   16000000UL
#define FREQUENCY               1
#define MAX_VALUE_16_BIT_TIMER  65535

#define TIMER1_PRELOAD          (MAX_VALUE_16_BIT_TIMER - (F_CPU / (1024 * FREQUENCY)))

// ISR to toggle the LED and reset timer
ISR (TIMER1_OVF_vect)
{
    PORTB ^= _BV(PORTB5);
    TCNT1 = TIMER1_PRELOAD;
}

// Initialise gpio
static void init_gpio()
{
    DDRB |= _BV(DDB5);
}

// Initialise timer
static void init_timer()
{
    uint16_t preload = TIMER1_PRELOAD;
    TCNT1 = preload;

    // Sets the prescaler to 1024
    // 16M/1024 = 15625Hz
    TCCR1B = (1 << CS10) | (1 << CS12);
    TCCR1A =  0x00;
    TIMSK1 = (1 << TOIE1);
}

int main(void)
{
    init_gpio();
    init_timer();

    sei();

    while (1);

    return 0;
}