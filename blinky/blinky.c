#include <stdint.h>

#define GPIO_BASE       0x50000000
#define GPIO_DIR        (*(volatile uint32_t *)(GPIO_BASE + 0x518))
#define GPIO_OUT        (*(volatile uint32_t *)(GPIO_BASE + 0x504))
#define LED_PIN         13

void delay(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < (ms * 1000); i++);
}

int main(void)
{
    GPIO_DIR |= (1 << LED_PIN);

    while (1)
    {
        GPIO_OUT ^= (1 << LED_PIN);
        delay(500);
    }
}