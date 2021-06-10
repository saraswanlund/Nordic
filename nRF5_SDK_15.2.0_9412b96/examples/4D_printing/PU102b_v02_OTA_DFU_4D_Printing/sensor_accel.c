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

#include "sensor_accel.h"
#include "sensor_als.h"
#include "pl_endian.h"
#include "pl_i2c.h"



int accel_init(const struct i2c_accel *accel){   
    uint8_t i2c_addr;
    uint8_t command[2];

    i2c_addr = accel->i2c_addr;

    // enable accelerometer 
    command[0] = CTRL_REG1; 
    command[1] = LP_HZ1_6 | X_AXIS_EN | Y_AXIS_EN |Z_AXIS_EN; // Enable X,Y,Z axis, power on mode, 
    if(write_i2c_data_8(i2c_addr,command,2,0))  
    {
        return -1;
    }

    command[1] = 0x00;

    command[0] = CTRL_REG2; 
    command[1] = HP_FILT_AOI1 | HP_FILT_AOI2 | HP_FILT_NORMAL; //Remove any DC from readings
    if(write_i2c_data_8(i2c_addr,command,2,0))  
    {
        return -1;
    }
  
    command[0] = CTRL_REG3; 
    command[1] = 0x00; 
    command[1] = I1_IA1; //Put activity interrupt on pin INT1
    if(write_i2c_data_8(i2c_addr,command,2,0))  
    {
        return -1;
    }

    //other setups
     command[0] = CTRL_REG4;
     command[1] = 0x00; 
     command[1] = HIGH_RES_EN;//High res and BDU enabled

     if(write_i2c_data_8(i2c_addr,command,2,0))  
     {
        return -1;
     }


    command[0] = CTRL_REG5; 
    command[1] = 0x00; 
    command[1] = LATCH_INTERRUPT_INT1 | LATCH_INTERRUPT_INT2;
    if(write_i2c_data_8(i2c_addr,command,2,0))  
    {
        return -1;
    }

    command[0] = CTRL_REG6; 
    command[1] = 0x00;
    command[1] = I2_IA2; //Enable Interrupt function 2 on pin 2
    if(write_i2c_data_8(i2c_addr,command,2,0))  
    {
        return -1;
    }

    command[0] = REFERENCE; 
    command[1] = 0x00; 
    if(write_i2c_data_8(i2c_addr,command,2,0))  
    {
        return -1;
    }

    command[0] = INT1_CFG; 
    command[1] = D6_DETECTION_XY_AXISES ;  //Enable position detection interrupt
    if(write_i2c_data_8(i2c_addr,command,2,0))  
    {
        return -1;
    }


    command[0] = INT1_THS; 
    command[1] = 31; 
    if(write_i2c_data_8(i2c_addr,command,2,0))  
    {
        return -1;
    }

    command[0] = INT1_DUR; 
    command[1] = 0x00; 
    if(write_i2c_data_8(i2c_addr,command,2,0))  
    {
        return -1;
    }

    command[0] = INT2_CFG; 
    command[1] = INTERRUPT_ANY_MOVEMENT; 
    if(write_i2c_data_8(i2c_addr,command,2,0))  
    {
        return -1;
    }

    command[0] = INT2_THS; 
    command[1] = 100; 
    if(write_i2c_data_8(i2c_addr,command,2,0))  
    {
        return -1;
    }

    command[0] = INT2_DUR; 
    command[1] = 0x00; 
    if(write_i2c_data_8(i2c_addr,command,2,0))  
    {
        return -1;
    }

     //enable secondary tempperature sensor
     command[0] = TEMP_CFG_REG;
     command[1] = 0x40;
     if(write_i2c_data_8(i2c_addr,command,2,0))  
     {
        return -1;
     }

     return 0;
     
}

// gets the data from the register
int accel_get_data(const struct i2c_accel *accel, uint16_t *data){

    uint8_t i2c_addr;
    uint8_t temp;
    int i;
    uint8_t register_addr;
    uint8_t raw[6];
    int16_t data_x;
    int16_t data_y;
    int16_t data_z;
    int x;
    int y;
    int z;
    

    i2c_addr = accel->i2c_addr;
    register_addr = OUT_X_L;

    for (i = 0; i < 6; i++){
        read_reg(i2c_addr, register_addr, &temp, 1);
        raw[i] = temp;
        register_addr++;
    }

    data_x = (raw[1]<<8) | raw[0];
    data_y = (raw[3]<<8) | raw[2];
    data_z = (raw[5]<<8) | raw[4];

    data[0] = data_x;
    data[1] = data_y;
    data[2] = data_z;

    printf(" x: %d , y: %d , z:%d \n",data_x,data_y,data_z);
     
    return 0;

}

uint8_t accel_get_status_reg(const struct i2c_accel *accel, uint8_t register_addr)
{
  uint8_t i2c_addr = accel->i2c_addr;
  i2c_addr = 0x18;
  uint8_t temp;
  read_reg(i2c_addr,register_addr,&temp,1);
  return temp;
}
