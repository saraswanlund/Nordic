#include <string.h>

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_error.h"

#include "pl_spi.h"

#define SPI_PL_INSTANCE  1 /**< SPI instance index. */

static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_PL_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

static uint8_t       m_tx_buf[] = {0x00, 0x10, 0x00, 0x02};           /**< TX buffer. */
static uint8_t       m_rx_buf[20] = {0xFF};                           /**< RX buffer. */
static const uint8_t m_length = sizeof(m_rx_buf);                     /**< Transfer length. */

/**
 * @brief SPI user event handler.
 * @param event
 */
void spi_pl_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context)
{
    uint8_t count = sizeof(m_rx_buf);
    switch(p_event -> type)
    {
        case NRF_DRV_SPI_EVENT_DONE:
            spi_xfer_done = true;
            printf("Transfer CMP\n");
            break;
    }
    //memset(m_rx_buf, 0, m_length);
}

void PL_SPI_init(void){
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = NRF_DRV_SPI_PIN_NOT_USED;
    spi_config.miso_pin = PL_MISO;
    spi_config.mosi_pin = PL_MOSI;
    spi_config.sck_pin  = PL_SCK;
    spi_config.orc       = 0xFF;
    spi_config.frequency = NRF_SPI_FREQ_8M;
    spi_config.mode      = NRF_DRV_SPI_MODE_0;
    spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;
    spi_config.irq_priority = 0;
    //APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_pl_event_handler, NULL));
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));
//    memset(m_rx_buf, 0, m_length);
//    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 4, m_rx_buf, 2));
//    printf("RX_Buf: %02x %02x", m_rx_buf[0], m_rx_buf[1]);
}

void spi_write_bytes(uint8_t *data, uint16_t size){
    spi_xfer_done = false;
    uint32_t err = nrf_drv_spi_transfer(&spi, data, size, NULL, NULL);
    //printf("Err: %d\n",err);
    APP_ERROR_CHECK(err);
    

//    while(!spi_xfer_done || nrf_drv_spi_end_event_get (&spi)){
//         //__WFE(); // wait until transfer is done changed from __WFE() 1/12
//    }
}

void spi_read_bytes(uint8_t *data, uint8_t size){
    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, NULL, NULL, data, size));
    

//    while(!spi_xfer_done){
//         //__WFE(); // wait until transfer is done __WFE() 1/12
//    }
}