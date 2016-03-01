#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "spi_slave_stream.h"
#include "spi_slave.h"
#include "nrf_delay.h"

static void blink_led(int count)
{
	for (int i = 0; i < count; i++) {
		nrf_gpio_pin_set(0);
		nrf_delay_us(50000);
		nrf_gpio_pin_clear(0);
		nrf_delay_us(50000);
	}
	nrf_delay_us(100000);
}

void ble_radio_ntf_handler(bool radio_state)
{
	if(radio_state==true)
	{
		nrf_gpio_pin_set(SPIS_PTS_PIN);
	}
	else
	{
		nrf_gpio_pin_clear(SPIS_PTS_PIN);
	}
}

/**@brief Function to set the tx buffer for next transaction.
 *
 * This will fill the tx buffer with data that will be sent to the master on the next transaction.
 *
 * @param[in] data buffer
 * @param[in] data buffer
 */
void spi_slave_send_data(uint8_t *buf, uint16_t size)
{
	while (NRF_SPIS1->EVENTS_END != 0) { }
	nrf_gpio_pin_set(SPIS_PTS_PIN);
	transmitting = true;
	m_tx_buf[0] = ((size & 0xFF00) >> 8);
	m_tx_buf[1] = (size & 0xFF);
	memcpy(m_tx_buf+2, buf, size);
	//alert the particle board we have data to send
	nrf_gpio_pin_set(SPIS_SA_PIN);
}

/**@brief Function for SPI slave event callback.
 *
 * This will get called upon receiving an SPI transaction complete event.
 *
 * @param[in] event SPI slave driver event.
 */
static void spi_slave_event_handle(spi_slave_evt_t event)
{
    uint32_t err_code;

    if (event.evt_type == SPI_SLAVE_XFER_DONE)
    {
    	if (transmitting) {
    		nrf_gpio_pin_clear(SPIS_SA_PIN);
    		nrf_gpio_pin_clear(SPIS_PTS_PIN);
    		transmitting = false;
    	} else {
			if (event.rx_amount == 255) {
				memcpy(buf+currentRxBufferSize, m_rx_buf, 254);
				currentRxBufferSize+=254;
			} else {
				memcpy(buf+currentRxBufferSize, m_rx_buf, event.rx_amount);
				currentRxBufferSize += event.rx_amount;
				rx_callback(buf, currentRxBufferSize);
				currentRxBufferSize = 0;

			}
    	}
    	//Set buffers.
		err_code = spi_slave_buffers_set(m_tx_buf, m_rx_buf, SPI_SLAVE_HW_TX_BUF_SIZE, SPI_SLAVE_HW_RX_BUF_SIZE);
		APP_ERROR_CHECK(err_code);
    } else if (event.evt_type == SPI_SLAVE_BUFFERS_SET_DONE) {

    } else if (event.evt_type == SPI_SLAVE_EVT_TYPE_MAX) {

    }
}

uint32_t spi_slave_stream_init(void (*a)(uint8_t *m_tx_buf, uint16_t size))
{
    uint32_t           err_code;
    spi_slave_config_t spi_slave_config;

    nrf_gpio_cfg_output(SPIS_SA_PIN);
    nrf_gpio_pin_clear(SPIS_SA_PIN);

    nrf_gpio_cfg_output(SPIS_PTS_PIN);
    nrf_gpio_pin_clear(SPIS_PTS_PIN);

    currentRxBufferSize = 0;

    // Enable Radio Notification, allows us to alert the master when we are busy
	err_code = ble_radio_notification_init(NRF_APP_PRIORITY_LOW,NRF_RADIO_NOTIFICATION_DISTANCE_800US,ble_radio_ntf_handler);
	APP_ERROR_CHECK(err_code);

    rx_callback = a;
    transmitting = false;

    err_code = spi_slave_evt_handler_register(spi_slave_event_handle);
    APP_ERROR_CHECK(err_code);

    spi_slave_config.pin_miso         = SPIS_MISO_PIN;
    spi_slave_config.pin_mosi         = SPIS_MOSI_PIN;
    spi_slave_config.pin_sck          = SPIS_SCK_PIN;
    spi_slave_config.pin_csn          = SPIS_CSN_PIN;
    spi_slave_config.mode             = SPI_MODE_0;
    spi_slave_config.bit_order        = SPIM_LSB_FIRST;
    spi_slave_config.def_tx_character = DEF_CHARACTER;
    spi_slave_config.orc_tx_character = ORC_CHARACTER;

    err_code = spi_slave_init(&spi_slave_config);
    APP_ERROR_CHECK(err_code);

    //Set buffers.
    err_code = spi_slave_buffers_set(m_tx_buf, m_rx_buf, sizeof(m_tx_buf), sizeof(m_rx_buf));
    APP_ERROR_CHECK(err_code);


    return NRF_SUCCESS;
}

