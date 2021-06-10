#include <string.h>

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "waveshare.h"

static uint8_t  lut_vcomDC[] = {
    0x00, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x60, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
};
static uint8_t  lut_ww[] = {
    0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
    0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static uint8_t  lut_bw[] = {
    0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x0F, 0x0F, 0x00, 0x00, 0x03,
    0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
    0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static uint8_t  lut_wb[] = {
    0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static uint8_t  lut_bb[] = {
    0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#define SPI_WS_INSTANCE  2 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_WS_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

#define TEST_STRING "Nordic"
static uint8_t       m_tx_buf[] = TEST_STRING;           /**< TX buffer. */
static uint8_t       m_rx_buf[sizeof(TEST_STRING) + 1];    /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);        /**< Transfer length. */

//#define ws_cmd_powersetting     0x01
//#define ws_cmd_softstart        0x06
//#define ws_cmd_04               0x04
//#define ws_cmd_panelsetting     0x00
//#define ws_cmd_PLLsetting       0x30
//#define ws_cmd_resolution       0x61
//#define ws_cmd_vcomDCsetting    0x82
//#define ws_cmd_SetFrame1        0x10
//#define ws_cmd_SetFrame2        0x13
//#define ws_cmd_VCOM             0x50
//#define ws_cmd_lut_vcomDC       0x20
//#define ws_cmd_lut_ww           0x21
//#define ws_cmd_lut_bw           0x22
//#define ws_cmd_lut_wb           0x23
//#define ws_cmd_lut_bb           0x24
//#define ws_cmd_DisplayFrame     0x12

static uint8_t      ws_cmd_powersetting[] = {0x01};
static uint8_t      ws_cmd_softstart[] = {0x06};
static uint8_t      ws_cmd_04[] = {0x04};
static uint8_t      ws_cmd_panelsetting[] = {0x00};
static uint8_t      ws_cmd_PLLsetting[] = {0x30};
static uint8_t      ws_cmd_resolution[] = {0x61};
static uint8_t      ws_cmd_vcomDCsetting[] = {0x82};
static uint8_t      ws_cmd_SetFrame1[] = {0x10};
static uint8_t      ws_cmd_SetFrame2[] = {0x13};
static uint8_t      ws_cmd_VCOM[] = {0x50};
static uint8_t      ws_cmd_lut_vcomDC[] = {0x20};
static uint8_t      ws_cmd_lut_ww[] = {0x21};
static uint8_t      ws_cmd_lut_bw[] = {0x22};
static uint8_t      ws_cmd_lut_wb[] = {0x23};
static uint8_t      ws_cmd_lut_bb[] = {0x24};
static uint8_t      ws_cmd_DisplayFrame[] = {0x12};

static uint8_t      ws_data_powersetting[] = {0x03, 0x00, 0x2b, 0x2b, 0x03};
static uint8_t      ws_data_softstart[] = {0x17, 0x17, 0x17};
static uint8_t      ws_data_panelsetting[] = {0xbf, 0x0d};
static uint8_t      ws_data_PLLsetting[] = {0x3a};
static uint8_t      ws_data_resolution[] = {EPD_WIDTH, ((EPD_HEIGHT >> 8) & 0xFF), (EPD_HEIGHT & 0xFF)};
static uint8_t      ws_data_vcomDCsetting[] = {0x28};
static uint8_t      ws_data_VCOM[] = {0x97};

void WS_SendCommand(uint8_t tx_data[]);
void WS_SendData(uint8_t tx_data[], uint16_t size);

/**
 * @brief SPI user event handler.
 * @param event
 */
void spi_ws_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context)
{
/*    spi_xfer_done = true;
    NRF_LOG_INFO("Transfer completed.");
    if (m_rx_buf[0] != 0)
    {
        NRF_LOG_INFO(" Received:");
        NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
    }
*/
    // Reset rx buffer and transfer done flag
    memset(m_rx_buf, 0, m_length);

    switch(p_event -> type)
    {
        case NRF_DRV_SPI_EVENT_DONE:
            spi_xfer_done = true;
    }
}

void WS_SPI_init(void){
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = WS_CS;
    spi_config.miso_pin = NULL;
    spi_config.mosi_pin = WS_MOSI;
    spi_config.sck_pin  = WS_SCK;
    spi_config.frequency= NRF_DRV_SPI_FREQ_4M;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_ws_event_handler, NULL));

    // declare the pins 
    nrf_gpio_pin_dir_set(WS_DC, NRF_GPIO_PIN_DIR_OUTPUT);       // DC pin
    nrf_gpio_pin_dir_set(WS_RST, NRF_GPIO_PIN_DIR_OUTPUT);      // RST pin
    nrf_gpio_pin_dir_set(WS_BUSY, NRF_GPIO_PIN_DIR_INPUT);      // BUSY pin

    uint8_t i = 0;

    WS_Reset();

    NRF_LOG_RAW_INFO("CMD Power Setting");
    WS_SendCommand(ws_cmd_powersetting);
    WS_SendData(ws_data_powersetting, 5);
//    WS_SendData(0x03);
//    WS_SendData(0x00);
//    WS_SendData(0x2b);
//    WS_SendData(0x2b);
//    WS_SendData(0x03);
//    for(i = 0; i < 5; i++){
//        WS_SendData(ws_data_powersetting[i]);
//    }

    WS_SendCommand(ws_cmd_softstart);
    WS_SendData(ws_data_softstart, 3);
//    WS_SendData(0x17);
//    WS_SendData(0x17);
//    WS_SendData(0x17);
//    for(i = 0; i < 3; i++){
//        WS_SendData(ws_data_softstart[i]);
//    }

    WS_SendCommand(ws_cmd_04);

    WS_SendCommand(ws_cmd_panelsetting);
    WS_SendData(ws_data_panelsetting, 2);
//    WS_SendData(0xbf);
//    WS_SendData(0x0d);
//    for(i = 0; i < 2; i++){
//        WS_SendData(ws_data_panelsetting[i]);
//    }

    WS_SendCommand(ws_cmd_PLLsetting);
    WS_SendData(ws_data_PLLsetting, 1);
//    WS_SendData(0x3a);
//    for(i = 0; i < 1; i++){
//        WS_SendData(ws_data_PLLsetting[i]);
//    }

    WS_SendCommand(ws_cmd_resolution);
    WS_SendData(ws_data_resolution, 3);
//    WS_SendData(EPD_WIDTH);
//    WS_SendData(((EPD_HEIGHT >> 8) & 0xFF));
//    WS_SendData((EPD_HEIGHT & 0xFF));
//    for(i = 0; i < 3; i++){
//        WS_SendData(ws_data_resolution[i]);
//    }

    WS_SendCommand(ws_cmd_vcomDCsetting);
    WS_SendData(ws_data_vcomDCsetting, 1);
//    WS_SendData(0x28);
//    for(i = 0; i < 1; i++){
//        WS_SendData(ws_data_vcomDCsetting[i]);
//    }
}

void WS_SendCommand(uint8_t tx_data[]){
    nrf_gpio_pin_clear(WS_DC);
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_data, 1, NULL, NULL));
    while(!spi_xfer_done){
        __WFE(); // wait until transfer is done
    }
    nrf_delay_us(250);
}

void WS_SendData(uint8_t tx_data[], uint16_t size){
    nrf_gpio_pin_set(WS_DC);
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_data, size, NULL, NULL));
    while(!spi_xfer_done){
        __WFE(); // wait until transfer is done
    }
    nrf_delay_us(250);
}

void WS_SendCommand2(uint8_t tx_data[]){
//    spi_xfer_done = false;
//    while(spi_xfer_done){
        nrf_gpio_pin_clear(WS_DC);
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_data, 1, NULL, NULL));
//    }
//    nrf_delay_ms(5);
}

void WS_SendData2(uint8_t tx_data[]){
//    spi_xfer_done = false;
//    while(spi_xfer_done){
        nrf_gpio_pin_set(WS_DC);
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_data, 1, NULL, NULL));
//    }
//    nrf_delay_ms(5);
}

void WS_SetFrameMemory(unsigned char * image_buffer){
    unsigned char Width, Height, data;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;
    
    WS_SendCommand(ws_cmd_SetFrame1);

    for (int j = 0; j < Height; j++) {
        for (int i = 0; i < Width; i++) {
            WS_SendData(0x00, 1);
        }
    }
    nrf_delay_ms(10);

    WS_SendCommand(ws_cmd_SetFrame2);
    for (int j = 0; j < Height; j++) {
        for (int i = 0; i < Width; i++) {
            WS_SendData(image_buffer[i + j * Width], 1);
//            data = pgm_read_byte(&image_buffer[i + j * Width]);
//            WS_SendData(data);
//            Serial.print(data);
        }
    }
    nrf_delay_ms(10);

    WS_SetFullReg();
    WS_DisplayFrame();    
}

void WS_SetFullReg(void){
    WS_SendCommand(ws_cmd_VCOM);       //VCOM AND DATA INTERVAL SETTING
    WS_SendData(ws_data_VCOM, 1);                 //WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

    unsigned int count;
    WS_SendCommand(ws_cmd_lut_vcomDC);
    WS_SendData(lut_vcomDC, 44);
//    for(count=0; count<44; count++) {
//        WS_SendData(lut_vcomDC[count], 1);
//    }

    WS_SendCommand(ws_cmd_lut_ww);
    WS_SendData(lut_ww, 42);
//    for(count=0; count<42; count++) {
//        WS_SendData(lut_ww[count], 1);
//    }

    WS_SendCommand(ws_cmd_lut_bw);
    WS_SendData(lut_bw, 42);
//    for(count=0; count<42; count++) {
//        WS_SendData(lut_bw[count], 1);
//    }

    WS_SendCommand(ws_cmd_lut_wb);
    WS_SendData(lut_wb, 42);
//    for(count=0; count<42; count++) {
//        WS_SendData(lut_wb[count], 1);
//    }

    WS_SendCommand(ws_cmd_lut_bb);
    WS_SendData(lut_bb, 42);
//    for(count=0; count<42; count++) {
//        WS_SendData(lut_bb[count], 1);
//    }
        
    nrf_delay_ms(1000);
}

void WS_DisplayFrame(void){
    WS_SendCommand(ws_cmd_DisplayFrame);
    nrf_delay_ms(100);
    WS_WaitUntilIdle();
}

void WS_ClearFrameMemory(void) {
    unsigned char Width, Height;
//    Width = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8 ) : (EPD_WIDTH / 8 + 1);
//    Height = EPD_HEIGHT;
    Width = 0x0D;      // 13
    Height = 0xD4;     // 212
    uint8_t white[] = {0x00};
    uint8_t black[] = {0xFF};
//    uint8_t data_00[2756];
//    memset(data_00, 0x00, sizeof(data_00));
//    uint8_t data_FF[2756];
//    memset(data_FF, 0xFF, sizeof(data_00));

   WS_SendCommand(ws_cmd_SetFrame1);
//   WS_SendData(data_00, 50);
    for (int j = 0; j < Height; j++) {
        for (int i = 0; i < Width; i++) {
            WS_SendData(white, 1);
        }
    }

//    nrf_delay_ms(1000);

    WS_SendCommand(ws_cmd_SetFrame2);
//    WS_SendData(data_FF, 50);
    for (int j = 0; j < Height; j++) {
        for (int i = 0; i < Width; i++) {
            WS_SendData(black, 1);
        }
    }

    nrf_delay_ms(1000);

    WS_SetFullReg();
    WS_DisplayFrame();
}

void WS_WaitUntilIdle(void){
    while(nrf_gpio_pin_read(WS_BUSY)){
        nrf_delay_ms(100);
    }
}

void WS_Reset(void){
    // reset
    nrf_gpio_pin_clear(WS_RST);
    nrf_delay_ms(200);
    nrf_gpio_pin_set(WS_RST);
    nrf_delay_ms(200);
}

// Reset rx buffer and transfer done flag
/*memset(m_rx_buf, 0, m_length);
spi_xfer_done = false;

APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, m_rx_buf, m_length));

while (!spi_xfer_done)
{
    __WFE();
}

NRF_LOG_FLUSH();

bsp_board_led_invert(BSP_BOARD_LED_0);
nrf_delay_ms(200);
*/