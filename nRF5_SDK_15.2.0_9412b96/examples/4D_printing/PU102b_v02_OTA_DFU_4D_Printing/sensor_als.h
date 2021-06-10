#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

/*ALS Stuff*/
                              //Reset Value
#define ALS_CONTR 0x80        //0x00
#define ALS_MEAS_RATE 0x85    //0x03
#define PART_ID 0x86          //0xA0
#define MANUFAC_ID 0x87       //0x05
#define ALS_DATA_CH1_0 0x88   
#define ALS_DATA_CH1_1 0x89
#define ALS_DATA_CH0_0 0x8A
#define ALS_DATA_CH0_1 0x8B
#define ALS_STATUS 0x8C
 


enum i2c_light_sensor_type {
	LTR_329ALS
};



struct i2c_als {
        struct pl_i2c *i2c;
	uint8_t i2c_addr;
	enum i2c_light_sensor_type type;
};


extern int ALS_init(const struct i2c_als *als);
extern int ALS_get_status(const struct i2c_als *als);
extern int read_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t* data, uint8_t count);
extern uint16_t ALS_get_data(const struct i2c_als *als);
#endif /* PL_ENDIAN_H */