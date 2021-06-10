
#include "battery_moniter.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "main.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "pl_i2c.h"



void batt_mon_init(void)
{
  uint8_t data_buffer[2];
  int error;

  //issue unseal command
  for(int i = 0; i!=2; i++)
  {
    data_buffer[0] = 0x00;
    error = write_i2c_data_8(0x55 ,data_buffer,1,1);
    data_buffer[1] = 0x80;
    data_buffer[0] = 0x00;
    error = write_i2c_data_8(0x55 ,data_buffer,1,0);
  }

  //issue hard reset
  data_buffer[0] = 0x00;
  data_buffer[1] = 0x41;
  error = write_i2c_data_8(0x55 ,data_buffer,2,0);

}

uint16_t batt_mon_read_voltage(void)
{
    uint8_t data_buffer[2];
    uint16_t Voltage;
    int error;


    data_buffer[1] = 0x05;
    data_buffer[0] = 0x00;
    error = write_i2c_data_8(0x55 ,data_buffer,1,1);
    data_buffer[1] = 0x05;
    data_buffer[0] = 0x00;
    error = write_i2c_data_8(0x55 ,data_buffer,1,0);
    error = read_i2c_data_8(0x55, data_buffer,2,0);

    Voltage = (data_buffer[1] << 8) | data_buffer[0];

    printf("Voltage Reading %d\n",Voltage);

    return Voltage;
}
/*
uint16_t batt_mon_read_temp(void)
{
}
uint8_t batt_mon_read_SOC(void)
{
}
uint16_t batt_mon_read_rem_cap(void)
{
}
uint16_t batt_mon_read_control_status(void)
{
}
*/