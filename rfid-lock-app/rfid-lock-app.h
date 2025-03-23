#ifndef RFID_LOCK
#define RFID_LOCK

#include <avr/io.h>

// ----- LED Pins ------

#define GREEN_LED_PIN       PD6
#define RED_LED_PIN         PD7

// ----- MFRC522 Commands & Registers -----
#define COMMAND_REG         0x01
#define SOFT_RESET_CMD      0x0F
#define TX_CONTROL_REG      0x14
#define FIFO_DATA_REG       0x09
#define FIFO_LEVEL_REG      0x0A
#define BIT_FRAMING_REG     0x0D
#define COMM_IRQ_REG        0x04
#define ERROR_REG           0x06
#define CRC_RESULT_L_REG    0x22
#define CRC_RESULT_H_REG    0x21
#define VERSION_REG         0x37
#define COMMAND_TRANSCEIVE  0x0C
#define COMMAND_CALC_CRC    0x03
#define COMMAND_IDLE        0x00
#define PICC_REQIDL         0x26
#define PICC_ANTICOLL       0x93
#define DIV_IRQ_REG         0x05

// ----- SPI Pins -----
#define SS_PIN              PB2
#define MOSI_PIN            PB3
#define MISO_PIN            PB4
#define SCK_PIN             PB5

// ------ UID Data -------
#define UID_LENGTH          4

// ------ UART Data -------
#define F_CPU               16000000UL
#define BAUD_RATE           9600
#define UBRR_VALUE          ((F_CPU / 16 / BAUD_RATE) - 1)

// ----- Function Declarations -----
void uart_init(void);
void uart_send(char data);
void uart_send_str(const char *str);

void spi_init(void);
uint8_t spi_transfer(uint8_t data);

void led_init(void);
void lock(void);
void unlock(void);

void mfrc522_reset(void);
void mfrc522_antenna_on(void);
void mfrc522_write_register(uint8_t reg, uint8_t value);
uint8_t mfrc522_read_register(uint8_t reg);
uint8_t mfrc522_request(uint8_t req_mode, uint8_t *tag_type);
uint8_t mfrc522_anticoll(uint8_t *uid);

#endif