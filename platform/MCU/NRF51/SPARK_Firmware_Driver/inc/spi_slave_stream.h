
#ifndef SPI_SLAVE_EXAMPLE_H__
#define SPI_SLAVE_EXAMPLE_H__

#include <stdint.h>
#include <stdbool.h>
#include "spi_slave.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "ble_radio_notification.h"

#define SPI_SLAVE_HW_TX_BUF_SIZE 255u
#define SPI_SLAVE_HW_RX_BUF_SIZE SPI_SLAVE_HW_TX_BUF_SIZE

#define DEF_CHARACTER 0xAAu             /**< SPI default character. Character clocked out in case of an ignored transaction. */
#define ORC_CHARACTER 0x55u             /**< SPI over-read character. Character clocked out after an over-read of the transmit buffer. */

#define SPIS_PTS_PIN 14
#define SPIS_SA_PIN 12
#define SPIS_MISO_PIN 10
#define SPIS_MOSI_PIN 11
#define SPIS_SCK_PIN 9
#define SPIS_CSN_PIN 8

volatile bool busy;
volatile bool transmitting;
volatile bool moreData;
volatile bool spiStreamSemaphoreReleased;
uint8_t m_tx_buf[SPI_SLAVE_HW_TX_BUF_SIZE];   /**< SPI TX buffer. */
uint8_t m_rx_buf[SPI_SLAVE_HW_RX_BUF_SIZE];   /**< SPI RX buffer. */

uint8_t buf[768];
int currentRxBufferSize;

void (*rx_callback)(uint8_t *m_tx_buf, uint16_t size);

uint32_t spi_slave_stream_init(void (*a)(uint8_t *m_tx_buf, uint16_t size));
void spi_slave_send_data(uint8_t *buf, uint16_t size);

bool spi_slave_ready();


#endif
