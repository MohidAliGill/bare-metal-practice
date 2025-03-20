#include <stdint.h>

#define GPIO_BASE       0x50000000
#define GPIO_DIR        (*(volatile uint32_t *)(GPIO_BASE + 0x518))
#define GPIO_OUT        (*(volatile uint32_t *)(GPIO_BASE + 0x504))
#define LED_PIN         13

#define SYSTICK_BASE    0xE000E010
#define SYSTICK_CTRL    (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_LOAD    (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_VAL     (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))

volatile uint32_t sysTickCounter = 0;

void SysTick_Handler(void)
{
    sysTickCounter++;
}

void systick_init(void)
{
    SYSTICK_LOAD = 64000 - 1;
    SYSTICK_VAL = 0;
    SYSTICK_CTRL = 7;
}

void delay_ms(uint32_t ms)
{
    sysTickCounter = 0;
    while (sysTickCounter < ms);
}

int main(void)
{
    systick_init();
    GPIO_DIR |= (1 << LED_PIN);

    while (1)
    {
        GPIO_OUT ^= (1 << LED_PIN);
        delay_ms(500);
    }
}