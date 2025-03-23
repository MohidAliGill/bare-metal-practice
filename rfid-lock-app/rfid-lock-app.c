#include <util/delay.h>
#include <stdio.h>
#include "rfid-lock-app.h"

const uint8_t KNOWN_UID[] = {0x19, 0x03, 0x56, 0x73};

// ---------- UART ----------
void uart_init() {
    UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
    UBRR0L = (unsigned char)(UBRR_VALUE);
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_send_str(const char *str) {
    while (*str) {
        if (*str == '\n') uart_send('\r');
        uart_send(*str++);
    }
}

// ---------- SPI ----------
void spi_init() {
    DDRB |= (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << SS_PIN);
    DDRB &= ~(1 << MISO_PIN);
    PORTB |= (1 << SS_PIN);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

uint8_t spi_transfer(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

// ---------- GPIO PIN ----------
void led_init(void)
{
    DDRD |= ((1 << RED_LED_PIN) | (1 << GREEN_LED_PIN));
    PORTD &= ~((1 << RED_LED_PIN) | (1 << GREEN_LED_PIN)); // Turn LEDs Off Initially
}

void lock(void)
{
    PORTD |= (1 << RED_LED_PIN);
    PORTD &= ~(1 << GREEN_LED_PIN);
}

void unlock(void)
{
    PORTD |= (1 << GREEN_LED_PIN);
    PORTD &= ~(1 << RED_LED_PIN);
}

// ---------- MFRC522 ----------
void mfrc522_write_register(uint8_t reg, uint8_t value) {
    PORTB &= ~(1 << SS_PIN);
    spi_transfer((reg << 1) & 0x7E);
    spi_transfer(value);
    PORTB |= (1 << SS_PIN);
}

uint8_t mfrc522_read_register(uint8_t reg) {
    PORTB &= ~(1 << SS_PIN);
    spi_transfer((reg << 1) | 0x80);
    uint8_t value = spi_transfer(0x00);
    PORTB |= (1 << SS_PIN);
    return value;
}

void mfrc522_reset() {
    mfrc522_write_register(COMMAND_REG, 0x0F);
    _delay_ms(50);
}

void mfrc522_antenna_on() {
    uint8_t val = mfrc522_read_register(TX_CONTROL_REG);
    if (!(val & 0x03)) {
        mfrc522_write_register(TX_CONTROL_REG, val | 0x03);
    }
}

void calculate_crc(uint8_t *data, uint8_t len, uint8_t *result) {
    mfrc522_write_register(COMMAND_REG, COMMAND_IDLE);
    mfrc522_write_register(DIV_IRQ_REG, 0x04);
    mfrc522_write_register(FIFO_LEVEL_REG, 0x80);
    for (uint8_t i = 0; i < len; i++)
        mfrc522_write_register(FIFO_DATA_REG, data[i]);
    mfrc522_write_register(COMMAND_REG, COMMAND_CALC_CRC);
    for (uint8_t i = 0; i < 0xFF; i++) {
        if (mfrc522_read_register(DIV_IRQ_REG) & 0x04) break;
        _delay_ms(1);
    }
    result[0] = mfrc522_read_register(CRC_RESULT_L_REG);
    result[1] = mfrc522_read_register(CRC_RESULT_H_REG);
}

uint8_t mfrc522_request(uint8_t req_mode, uint8_t *tag_type) {
    mfrc522_write_register(COMMAND_REG, COMMAND_IDLE);
    mfrc522_write_register(FIFO_LEVEL_REG, 0x80);
    mfrc522_write_register(BIT_FRAMING_REG, 0x07);

    mfrc522_write_register(FIFO_DATA_REG, req_mode);
    mfrc522_write_register(COMMAND_REG, COMMAND_TRANSCEIVE);

    for (uint8_t i = 0; i < 200; i++) {
        if (mfrc522_read_register(COMM_IRQ_REG) & 0x30) break;
        _delay_ms(1);
    }

    if (mfrc522_read_register(ERROR_REG) & 0x1B) return 0;

    tag_type[0] = mfrc522_read_register(FIFO_DATA_REG);
    tag_type[1] = mfrc522_read_register(FIFO_DATA_REG);
    return 1;
}

uint8_t mfrc522_anticoll(uint8_t *uid) {
    mfrc522_write_register(COMMAND_REG, COMMAND_IDLE);
    mfrc522_write_register(FIFO_LEVEL_REG, 0x80);
    mfrc522_write_register(BIT_FRAMING_REG, 0x00);

    mfrc522_write_register(FIFO_DATA_REG, PICC_ANTICOLL);
    mfrc522_write_register(FIFO_DATA_REG, 0x20);
    mfrc522_write_register(COMMAND_REG, COMMAND_TRANSCEIVE);

    for (uint8_t i = 0; i < 200; i++) {
        if (mfrc522_read_register(COMM_IRQ_REG) & 0x30) break;
        _delay_ms(1);
    }

    if (mfrc522_read_register(ERROR_REG) & 0x1B) return 0;

    for (uint8_t i = 0; i < 5; i++)
        uid[i] = mfrc522_read_register(FIFO_DATA_REG);

    return 1;
}

uint8_t compare_uid(uint8_t *uid)
{
    for (uint8_t i = 0; i < UID_LENGTH; i++)
    {
        if (uid[i] != KNOWN_UID[i]) return 0;
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
    char hex[6];
    sprintf(hex, "0x%02X\n", version);
    uart_send_str("VersionReg = ");
    uart_send_str(hex);

    uint8_t tag_type[2];
    uint8_t uid[5];

    while (1) {
        if (mfrc522_request(PICC_REQIDL, tag_type)) {
            uart_send_str("Card Detected!\n");

            if (mfrc522_anticoll(uid))
            {
                uart_send_str("Card UID: ");
                for (uint8_t i = 0; i < UID_LENGTH; i++)
                {
                    char byte_hex[4];
                    sprintf(byte_hex, "%02X", uid[i]);
                    uart_send_str(byte_hex);
                }
                uart_send_str("\n");

                if (compare_uid(uid))
                {
                    uart_send_str("Access Granted!\n");
                    unlock();
                    _delay_ms(5000);
                    lock();
                }
                else
                {
                    uart_send_str("Access Denied!\n");
                }
            }
        }
        _delay_ms(1000);
    }
}