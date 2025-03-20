#include <avr/io.h>
#include <util/delay.h>

#define F_CPU       16000000UL
#define BAUD        9600
#define UBRR_VALUE  ((F_CPU / 16 / BAUD) - 1)

// Initializes USART communication
static void init_usart()
{
    // Setting baud rate
    UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
    UBRR0L = (unsigned char)(UBRR_VALUE);

    // Enabling RX and TX
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    // Setting frame format
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Send a single charachter over USART
static void usart_send(unsigned char data)
{
    // Wait until transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

// Receives single charachter over USART
static unsigned char usart_receive()
{
    // Waits until data is received
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

// Send a null terminated string over USART
static void usart_send_str(char *str)
{
    while (*str)
    {
        usart_send(*str++);
    }
}

int main(void)
{
    init_usart();

    while (1)
    {
        usart_send_str("Hello UART\n");
        _delay_ms(1000);
    }

    return 0;
}