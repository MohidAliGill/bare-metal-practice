#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define F_CPU               16000000UL
#define BAUD_RATE           9600
#define UBRR_VALUE          ((F_CPU / 16 / BAUD_RATE) - 1)

// MFRC522 Registers
#define COMMAND_REG         0x01
#define SOFT_RESET_CMD      0x0F
#define TX_CONTROL_REG      0x14
#define FIFO_DATA_REG       0x09
#define FIFO_LEVEL_REG      0x0A
#define BIT_FRAMING_REG     0x0D
#define COMM_IRQ_REG        0x04
#define ERROR_REG           0x06
#define VERSION_REG         0x37
#define COMMAND_TRANSCEIVE  0x0C
#define COMMAND_IDLE        0x00
#define PICC_REQIDL         0x26
#define PICC_ANTICOLL       0x93
#define MAX_LEN             16

// SPI Pins
#define SS_PIN              PB2
#define MOSI_PIN            PB3
#define MISO_PIN            PB4
#define SCK_PIN             PB5

// ---------- UART ----------
static void uart_init() {
    UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
    UBRR0L = (unsigned char)(UBRR_VALUE);
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

static void uart_send(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

static void uart_send_str(const char *str) {
    while (*str) {
        if (*str == '\n') uart_send('\r');
        uart_send(*str++);
    }
}

// ---------- SPI ----------
static void spi_init() {
    DDRB |= (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << SS_PIN);
    DDRB &= ~(1 << MISO_PIN);
    PORTB |= (1 << SS_PIN);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

static uint8_t spi_transfer(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

// ---------- MFRC522 ----------
static void mfrc522_write_register(uint8_t reg, uint8_t value) {
    PORTB &= ~(1 << SS_PIN);
    spi_transfer((reg << 1) & 0x7E);
    spi_transfer(value);
    PORTB |= (1 << SS_PIN);
}

static uint8_t mfrc522_read_register(uint8_t reg) {
    PORTB &= ~(1 << SS_PIN);
    spi_transfer((reg << 1) | 0x80);
    uint8_t value = spi_transfer(0x00);
    PORTB |= (1 << SS_PIN);
    return value;
}

static void mfrc522_reset() {
    mfrc522_write_register(COMMAND_REG, SOFT_RESET_CMD);
    _delay_ms(50);
}

static void mfrc522_antenna_on() {
    uint8_t val = mfrc522_read_register(TX_CONTROL_REG);
    if (!(val & 0x03)) {
        mfrc522_write_register(TX_CONTROL_REG, val | 0x03);
    }
}

static uint8_t mfrc522_request(uint8_t req_mode, uint8_t *tag_type) {
    mfrc522_write_register(COMMAND_REG, COMMAND_IDLE);
    mfrc522_write_register(FIFO_LEVEL_REG, 0x80);

    mfrc522_write_register(FIFO_DATA_REG, req_mode);
    mfrc522_write_register(BIT_FRAMING_REG, 0x07);
    mfrc522_write_register(COMMAND_REG, COMMAND_TRANSCEIVE);

    uint8_t i = 255;
    while (i--) {
        uint8_t irq = mfrc522_read_register(COMM_IRQ_REG);
        if (irq & 0x30) break;
        _delay_ms(1);
    }

    uint8_t err = mfrc522_read_register(ERROR_REG);
    if (err & 0x1B) return 0;

    uint8_t fifo_level = mfrc522_read_register(FIFO_LEVEL_REG);
    if (fifo_level < 2) return 0;

    tag_type[0] = mfrc522_read_register(FIFO_DATA_REG);
    tag_type[1] = mfrc522_read_register(FIFO_DATA_REG);

    return 1;
}

static uint8_t mfrc522_anticoll(uint8_t *uid) {
    mfrc522_write_register(COMMAND_REG, COMMAND_IDLE);
    mfrc522_write_register(FIFO_LEVEL_REG, 0x80);

    mfrc522_write_register(FIFO_DATA_REG, PICC_ANTICOLL);
    mfrc522_write_register(FIFO_DATA_REG, 0x20); // NVB (Number of Valid Bits)
    mfrc522_write_register(COMMAND_REG, COMMAND_TRANSCEIVE);
    mfrc522_write_register(BIT_FRAMING_REG, 0x00);

    uint8_t i = 255;
    while (i--) {
        uint8_t irq = mfrc522_read_register(COMM_IRQ_REG);
        if (irq & 0x30) break;
        _delay_ms(1);
    }

    uint8_t err = mfrc522_read_register(ERROR_REG);
    if (err & 0x1B) return 0;

    for (uint8_t j = 0; j < 5; j++) {
        uid[j] = mfrc522_read_register(FIFO_DATA_REG);
    }

    return 1;
}

// ---------- MAIN ----------
int main(void) {
    uart_init();
    spi_init();

    _delay_ms(1000);
    uart_send_str("Initializing MFRC522...\n");

    mfrc522_reset();
    mfrc522_antenna_on();

    uint8_t version = mfrc522_read_register(VERSION_REG);
    char buffer[32];
    sprintf(buffer, "VersionReg = 0x%02X\n", version);
    uart_send_str(buffer);

    uint8_t tag_type[2];
    uint8_t uid[5];

    while (1) {
        if (mfrc522_request(PICC_REQIDL, tag_type)) {
            uart_send_str("Card Detected!\n");
            if (mfrc522_anticoll(uid)) {
                uart_send_str("Card UID: ");
                for (uint8_t i = 0; i < 5; i++) {
                    sprintf(buffer, "%02X ", uid[i]);
                    uart_send_str(buffer);
                }
                uart_send_str("\n");
            }
            _delay_ms(1000);
        }
    }
}