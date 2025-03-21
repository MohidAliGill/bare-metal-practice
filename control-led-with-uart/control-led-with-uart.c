#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#define F_CPU           16000000UL
#define BAUD            9600
#define UBRR_VALUE      ((F_CPU / 16 / BAUD) - 1)

#define BUFFER_SIZE     64
#define CMD_BUFFER_SIZE 32

volatile char uart_buffer[BUFFER_SIZE];
volatile uint8_t head = 0;
volatile uint8_t tail = 0;

static char command_buffer[CMD_BUFFER_SIZE];
static uint8_t cmd_index = 0;

#define LED_DDR         DDRB
#define LED_PORT        PORTB
#define LED_PIN         PB5

// ISR for buffer data received
ISR(USART_RX_vect)
{
    char received = UDR0;
    
    uint8_t next = (head + 1) % BUFFER_SIZE;
    if (next != tail)  // Prevent buffer overflow
    {
        uart_buffer[head] = received;
        head = next;
    }
}

// Initialise the USART
static void init_usart()
{
    UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
    UBRR0L = (unsigned char)(UBRR_VALUE);

    // Enable RX, TX, and RX Complete Interrupt
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    sei();  // Enable global interrupts
}

// Initialise the GPIO
static void init_gpio()
{
    LED_DDR |= (1 << LED_PIN);
}

// Send character over USART
static void usart_send(unsigned char data)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

// Send a null-terminated string over USART (fixing newlines)
static void usart_send_str(const char *str)
{
    while (*str)
    {
        if (*str == '\n')
        {
            usart_send('\r'); 
        }
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

// Toggles LED on
static void toggle_led_on()
{
    LED_PORT |= (1 << LED_PIN);
    usart_send_str("\nLED TURNED ON!!\n");
}

// Toggles LED off
static void toggle_led_off()
{
    LED_PORT &= ~(1 << LED_PIN);
    usart_send_str("\nLED TURNED OFF!!\n");
}

// Prints the active status of LED
static void print_led_status()
{
    if (LED_PORT & (1 << LED_PIN))
    {
        usart_send_str("\nLED STATUS: ON\n");
    } 
    else
    {
        usart_send_str("\nLED STATUS: OFF\n");
    }
}

// Process the command string and call the respective function
static void process_command()
{
    command_buffer[cmd_index] = '\0';

    usart_send_str("\nProcessing Command: ");
    usart_send_str(command_buffer);

    if (strcmp(command_buffer, "LED ON") == 0)
    {
        toggle_led_on();
    } 
    else if (strcmp(command_buffer, "LED OFF") == 0)
    {
        toggle_led_off();
    }
    else if (strcmp(command_buffer, "STATUS") == 0)
    {
        print_led_status();
    } 
    else 
    {
        usart_send_str("\nInvalid Command\n");
    }

    cmd_index = 0;
}

int main(void)
{
    init_usart();
    init_gpio();

    usart_send_str("\nUART Ready. Type commands:\n- LED ON\n- LED OFF\n- STATUS\n");

    while (1)
    {
        char received = usart_receive_buffered();

        if (received)
        {
            // Echo received character (for visibility)
            usart_send(received);

            if (received == '\b' || received == 0x7F)
            {
                if (cmd_index > 0)
                {
                    cmd_index--;

                    usart_send('\b');
                    usart_send(' ');
                    usart_send('\b');
                }
            }
            else if (received == '\n' || received == '\r')  // Process when Enter is pressed
            {
                if (cmd_index > 0)  // Ignore empty inputs
                {
                    process_command();
                }
            } 
            else if (cmd_index < CMD_BUFFER_SIZE - 1)  // Prevent overflow
            {
                command_buffer[cmd_index++] = received;
            }
        }
    }

    return 0;
}