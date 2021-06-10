/*
 * i2c.c -- i2c interface abstraction layer
 *
 * Authors:
 *   
 *
 */
#include "app_error.h"
#include "nrf_drv_twi.h"
//#include "nrf_drv_twis.h"

#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "pl_i2c.h"
#include "pl_endian.h"

/* TWI instance ID. */
#define TWIM_INSTANCE_ID     0

/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done = false;

static bool m_error_flag;

/* TWI instance. */
static const nrf_drv_twi_t m_pl_twim = NRF_DRV_TWI_INSTANCE(TWIM_INSTANCE_ID);

int pl_i2c_reg_read_8(struct pl_i2c *i2c, uint8_t i2c_addr, uint8_t reg, uint8_t *data)
{
        ret_code_t err_code;
//        m_xfer_done = false;

	err_code = nrf_drv_twi_tx(&m_pl_twim, i2c_addr, &reg, 1, true);
        

//        while(!nrf_drv_twi_is_busy(&m_pl_twim)); 
//        while (m_xfer_done == false){
//            __WFE(); // wait until transfer is done
//        }

//        m_xfer_done = false;
	
        err_code = nrf_drv_twi_rx(&m_pl_twim, i2c_addr, data, 1);
        APP_ERROR_CHECK(err_code);
//         while(!nrf_drv_twi_is_busy(&m_pl_twim)); 
//        while (m_xfer_done == false){
//          __WFE(); // wait until transfer is done
//       }
        
        return 0;
}

int pl_i2c_reg_write_8(struct pl_i2c *i2c, uint8_t i2c_addr, uint8_t reg, uint8_t data)
{
        ret_code_t err_code;
	const uint8_t w_data[2] = { reg, data };
//        m_xfer_done = false;

	err_code=nrf_drv_twi_tx(&m_pl_twim, i2c_addr, w_data, sizeof(w_data), 0);
        APP_ERROR_CHECK(err_code);
//        while(!nrf_drv_twi_is_busy(&m_pl_twim)); 
//        while (m_xfer_done == false){
//            __WFE(); // wait until transfer is done
//        }

        return 0;
}

int pl_i2c_reg_read_16be(uint8_t i2c_addr, uint8_t reg, uint16_t *data)
{
	nrf_drv_twi_tx(&m_pl_twim, i2c_addr, &reg, 1, PL_I2C_NO_STOP);

	nrf_drv_twi_rx(&m_pl_twim, i2c_addr, (uint8_t *)data, 2);

#if CONFIG_LITTLE_ENDIAN
	swap16(data);
#endif

	return 0;
}

int pl_i2c_reg_write_16be(uint8_t i2c_addr, uint8_t reg, uint16_t data)
{
	const uint8_t w_data[3] = {
		reg, ((data >> 8) & 0xff), (data & 0xff) };

	nrf_drv_twi_tx(&m_pl_twim, i2c_addr, w_data, sizeof(w_data), 0);

	return 0;
}

/**
 * @brief TWI events handler.
 */
void pl_twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            m_xfer_done = true;
            //printf("TWI EVENT DONE\n");
            break;
        case NRF_DRV_TWI_EVT_ADDRESS_NACK:
            printf("TWI Address NACK Error\n");
            m_xfer_done = true;
            break;
        case NRF_DRV_TWI_EVT_DATA_NACK:
            printf("TWI Event Data NACK Error\n");
            m_xfer_done = true;
            break;
        default:
            break;
    }
}

/**
 * @brief TWI initialization.
 */

void pl_twi_init(struct pl_i2c *i2c)
{
    uint8_t pgstat;		///
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_pl_config = {
       .scl                = PL_I2C_SCL,
       .sda                = PL_I2C_SDA,
       .frequency          = NRF_DRV_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = true
    };

    err_code = nrf_drv_twi_init(&m_pl_twim, &twi_pl_config, NULL, NULL);  //err_code = nrf_drv_twi_init(&m_pl_twim, &twi_pl_config, pl_twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_pl_twim);

    // for debugging purpose, to see whether I2C is working
    //pl_i2c_reg_read_8(i2c, TPS65185_ADDR, PGSTAT_REG, &pgstat);       // reading should be 0x65
    //printf("PGSTAT: 0x%02X\n", pgstat);
}

int read_i2c_data_8(uint8_t i2c_addr, uint8_t *data, uint8_t count, uint8_t flags)
{
    ret_code_t err_code;
    m_xfer_done = false;

    err_code = nrf_drv_twi_rx(&m_pl_twim, i2c_addr, data, count);
    APP_ERROR_CHECK(err_code);
//
//    while(!m_xfer_done){
//        __WFE(); // wait until transfer is done
//    }

    return 0;
}

int write_i2c_data_8(uint8_t i2c_addr, uint8_t *data, uint8_t count, uint8_t flags)
{
    ret_code_t err_code;
    m_xfer_done = false;

    err_code = nrf_drv_twi_tx(&m_pl_twim, i2c_addr, data, count, flags);
    APP_ERROR_CHECK(err_code);
    
//    while(!m_xfer_done){
//        __WFE(); // wait until transfer is done
//    }

    return 0;
}