#include <stdio.h>
#include <math.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "main.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "sensor_als.h"
#include "pl_endian.h"
#include "pl_i2c.h"


int ALS_init(const struct i2c_als *als){
    
    uint8_t i2c_addr;
    uint8_t command[2];

    i2c_addr = als->i2c_addr;

    // enable Ambient Light Sensor, ALS 
    command[0] = ALS_CONTR;                    // control register
    command[1] = 0x01;                         // power mode bit
    if(write_i2c_data_8(i2c_addr, command, 2, 0)) // 0x01 command  
    {
        return -1;
    }

    //ALS set Measurement Rate
     command[0] = ALS_MEAS_RATE;                // measure rate register
     command[1] = 0x12;
     if(write_i2c_data_8(i2c_addr, command, 2, 0)) // measure rate 200 ms  
     { 
        return -1;
     }

     return 0;
}


// gets the status of the data in the register
int ALS_get_status(const struct i2c_als *als){
    
    uint8_t register_addr;
    uint8_t i2c_addr;
    uint8_t data = 0x00;

    i2c_addr = als->i2c_addr;

    register_addr = ALS_STATUS; //status of data
    read_reg(i2c_addr, register_addr, &data, 1);
    printf("%02x\n", data); // if data is 0x04 then data in register is valid. ie. New data has arrived

    return data;
}

// gets data in register
uint16_t ALS_get_data(const struct i2c_als *als){
    
    uint8_t data0;
    uint8_t data1;
    uint8_t data2;
    uint8_t data3;
    uint16_t data_ch1;
    uint16_t data_ch0;


    read_reg(I2C_ALS_ADDR,ALS_DATA_CH1_0,&data0,1);

    read_reg(I2C_ALS_ADDR,ALS_DATA_CH1_1,&data1,1);

    read_reg(I2C_ALS_ADDR,ALS_DATA_CH0_0,&data2,1);

    read_reg(I2C_ALS_ADDR,ALS_DATA_CH0_1,&data3,1);




    data_ch0  = (data3<<8)|data2;
    data_ch1 = (data1<<8)|data0;

    printf("ch0: %04x ch1: %04x \n",data_ch0,data_ch1);

    return (data_ch0 + data_ch1)/2;
}



int read_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t* data, uint8_t count){
      
    uint8_t register_addr = reg_addr;

    if(write_i2c_data_8(i2c_addr, &register_addr,count,1))
      {
          return -1;
      }

    if(read_i2c_data_8(i2c_addr,data, count, 0))
      {
          return -1;
      }

    return 0;
} 

