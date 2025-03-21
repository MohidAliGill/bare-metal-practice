#include <avr/io.h>
#include <util/delay.h>

#define F_CPU           16000000UL
#define PWM_TOP         255     // 8-bit resolution
#define PWM_PRESCALER   8
#define FADE_DELAY_MS   20

// Initialize Timer1 in Fast PWM mode (Mode 14)
static void init_pwm()
{
    DDRB |= (1 << PB1); // Pin 9  as Output pin
    
    // Fast PWM Mode 14: WGM13:WGM10 = 1110
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Fast PWM - Prescaler = 8

    ICR1 = PWM_TOP;

    OCR1A = PWM_TOP / 2;
}

// Set PWM duty cycle on OC1A (PB1)
void set_pwm_duty(uint8_t duty)
{
    OCR1A = duty;
}

int main(void)
{
    init_pwm();

    while (1)
    {
        for (uint8_t i = 0; i <= PWM_TOP; i++)
        {
            set_pwm_duty(i);
            _delay_ms(20);
        }

        for (uint8_t i = PWM_TOP; i > 0; i--)
        {
            set_pwm_duty(i);
            _delay_ms(20);
        }
    }

    return 0;
}