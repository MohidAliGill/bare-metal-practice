#include <avr/io.h>
#include <util/delay.h>

#define F_CPU       16000000UL
#define BAUD_RATE   9600
#define UBRR_VALUE  ((F_CPU / 16 / BAUD_RATE) - 1)

// SPI Pins
#define SS_PIN      PB2
#define MOSI_PIN    PB3
#define MISO_PIN    PB4
#define SCK_PIN     PB5

// MFRC522 Registers
#define VERSION_REG 0x37

// ---------- UART ----------
static void uart_init()
{
    UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
    UBRR0L = (unsigned char)(UBRR_VALUE);

    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

static void uart_send(char data)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

static void uart_send_str(const char *str)
{
    while (*str)
    {
        if (*str == '\n') uart_send('\r');
        uart_send(*str++);
    }
}

// ---------- SPI ----------
static void spi_init()
{
    // MOSI, SCK, SS = output
    DDRB |= (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << SS_PIN);
    DDRB &= ~(1 << MISO_PIN);  // Just for clarity (input)

    // Enable SPI | Master | Clock rate = F_CPU/16
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

static uint8_t spi_transfer(uint8_t data)
{
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

// ---------- MFRC522 ----------
static uint8_t mfrc522_read_register(uint8_t reg)
{
    uint8_t read_cmd = (reg << 1) | 0x80;

    PORTB &= ~(1 << SS_PIN);  // Select MFRC522

    spi_transfer(read_cmd);
    uint8_t value = spi_transfer(0x00);

    PORTB |= (1 << SS_PIN);   // Deselect

    return value;
}

// ---------- MAIN ----------
int main(void)
{
    uart_init();
    spi_init();

    _delay_ms(1000);
    uart_send_str("Reading MFRC522 Version Register...\n");

    uint8_t version = mfrc522_read_register(VERSION_REG);

    char hex[5];
    hex[0] = "0123456789ABCDEF"[version >> 4];
    hex[1] = "0123456789ABCDEF"[version & 0x0F];
    hex[2] = '\n';
    hex[3] = '\0';

    uart_send_str("VersionReg = 0x");
    uart_send_str(hex);

    while (1);
}