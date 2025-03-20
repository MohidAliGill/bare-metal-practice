#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU       16000000UL 
#define BAUD        9600      
#define UBRR_VALUE  ((F_CPU / 16 / BAUD) - 1) 
#define BUFFER_SIZE 64

volatile char uart_buffer[BUFFER_SIZE];
volatile uint8_t head = 0;
volatile uint8_t tail = 0;

// ISR for buffer data received
ISR(USART_RX_vect)
{
    char received = UDR0;
    
    uint8_t next = (head + 1) % BUFFER_SIZE;
    if (next != tail)
    {
        uart_buffer[head] = received;
        head = next;
    }
}

// Initialising USART
static void init_usart()
{
    // Setting baud rate
    UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
    UBRR0L = (unsigned char)(UBRR_VALUE);

    // Enabling RX, TX and RX complete interrupt
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

    // Setting frame format
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Enabling global interrupts
    sei();
}

// Send characher over USART
static void usart_send(unsigned char data)
{
    // Wait until transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

// Send a null terminated string over USART
static void usart_send_str(char *str)
{
    while (*str)
    {
        usart_send(*str++);
    }
}

// Received buffered data over USART
static char usart_receive_buffered()
{
    if (head == tail)
    {
        return 0;
    }

    char data = uart_buffer[tail];
    tail = (tail + 1) % BUFFER_SIZE;
    return data;
}

int main(void)
{
    init_usart();

    usart_send_str("UART Buffer Ready!!\n");

    while (1)
    {
        char received = usart_receive_buffered();
        if (received)
        {
            usart_send(received);
        }
    }

    return 0;
}