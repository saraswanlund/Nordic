#include "TWI.h"



static void twi_master_init(nrf_drv_twi_t  m_twi_master)
{
    ret_code_t err_code;
    const nrf_drv_twi_config_t config =
    {
       .scl                = TWI_SCL,
       .sda                = TWI_SDA,
       .frequency          = NRF_DRV_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi_master, &config, NULL, NULL);

    if (NRF_SUCCESS == err_code)
    {
        nrf_drv_twi_enable(&m_twi_master);
    }

    APP_ERROR_CHECK(err_code);
}