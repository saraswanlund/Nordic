#include "nrf_drv_twi.h"
#include "nrf_gpio.h"

#define TWI_SDA 10
#define TWI_SCL 9

static void twi_master_init(nrf_drv_twi_t  m_twi_master);
