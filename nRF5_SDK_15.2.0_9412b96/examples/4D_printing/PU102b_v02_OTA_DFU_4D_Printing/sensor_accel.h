#ifndef SENSOR_ACCEL_H
#define SENSOR_ACCEL_H

#include <stdint.h>

/*ALS Stuff*/
                              //Reset Value
#define STATUS_REG_AUX 0x07
#define OUT_ADC1_L     0x08
#define OUT_ADC1_H     0x09
#define OUT_ADC2_L     0x0A
#define OUT_ADC2_H     0x0B
#define OUT_ADC3_L     0x0C
#define OUT_ADC3_H     0x0D
#define TEMP_CFG_REG   0x1F
#define CTRL_REG1      0x20  //Axis, Power, Poll Rate
#define CTRL_REG2      0x21  //Filter config
#define CTRL_REG3      0x22  //Interrupt PIN INT1 connection config
#define CTRL_REG4      0x23  //Data and Update, Scale, Resolution, Self Testt
#define CTRL_REG5      0x24  //Boot, FIFO buffer, Latch Interrupt Pint
#define CTRL_REG6      0x25  //Interrupt PIN INT2 connection Config
#define REFERENCE      0x26  //Configures the reference PIN all measurements can be referenced from
#define STATUS_REG     0x27 //Status Register
#define OUT_X_L        0x28 //X data LSB 2 compliment left justified
#define OUT_X_H        0x29 //X data MSB 2 compliment left justified
#define OUT_Y_L        0x2A //Y data LSB 2 compliment left justified
#define OUT_Y_H        0x2B //Y data MSB 2 compliment left justified
#define OUT_Z_L        0x2C //Z data LSB 2 compliment left justified
#define OUT_Z_H        0x2D //Z data MSB 2 compliment left justified
#define FIFO_CTRL_REG  0x2E
#define FIFO_SRC_REG   0x2F
#define INT1_CFG       0x30  //Configures movement that can interrupt on PIN INT1
#define INT1_SRC       0x31  //Tells what caused the interrupt event for PIN INT1
#define INT1_THS       0x32  //Configures acceleration threshold required for interrupt on PIN INT1
#define INT1_DUR       0x33  //Configures acceleration duration threshold required for interrupt on PIN INT2
#define INT2_CFG       0x34  //Configures movement that can interrupt on PIN INT2
#define INT2_SRC       0x35  //Tells what caused the interrupt event for PIN INT2
#define INT2_THS       0x36  //Configures acceleration threshold required for interrupt on PIN INT2
#define INT2_DUR       0x37  //Configures acceleration duration threshold required for interrupt on PIN INT2
#define CLICK_CFG      0x38  //For "Click" sensing
#define CLICK_SRC      0x39  //For "Click" sensing
#define CLICK_THS      0x3A  //For "Click" sensing
#define TIME_LIMIT     0x3B  //For "Click" sensing
#define TIME_LATENCY   0x3C  //For "Click" sensing
#define TIME_WINDOW    0x3D  //For "Click" sensing
#define ACT_THS        0x3E  //Sleep/wake activity threshhold
#define ACT_DUR        0x3F  //Sleep/wake activity duration threshold



#define HZ400 (1<<6) | (1<<5) | (1<<4)
#define HZ200 (1<<6) | (1<<5)
#define HZ100 (1<<6) | (1<<4)
#define LP_HZ1_6 (1<<7)
#define LOW_POWER_EN (1<<3)
#define Z_AXIS_EN (1<<2)
#define Y_AXIS_EN (1<<1)
#define X_AXIS_EN (1<<0)
#define ADC_EN (1<<7)
#define TEMP_EN (1<<6)
#define SDO_PULLUP_DISABLE (1<<7) | (1<<4)
#define HP_FILT_NORMAL (1<<7)
#define HP_FILT_AOI1   (1<<0)
#define HP_FILT_AOI2   (1<<1)
#define ZYX_NEW_DATA_INT (1<<4) //For CTRL register 3  for int pin config
#define ACTIVITY_1_INT_EN (1<<6) //For CTRL register 3 and 6 for int pin config
#define ACTIVITY_2_INT_EN (1<<5) //For CTRL register 3 and 6 for int pin config.
#define INT_PIN_ACTIVE_LOW (1<<1) //For CTRL register 3 and 6 for int pin config.
#define SCALE_4G  (1<<4)  //For CTRL REG 4
#define HIGH_RES_EN (1<<3) //FOR CTRL REG 4
#define SELF_TEST_OFF (0xF9 & 0)
#define SELF_TEST_0 (1<<1)
#define SELF_TEST_1 (1<<2)
#define INTERRUPT_ANY_MOVEMENT (1<<5) | (1<<3) | (1<<1)
#define LATCH_INTERRUPT_INT1 (1<<3)
#define LATCH_INTERRUPT_INT2 (1<<1)
#define D4D_INT1 (1<<2)
#define D6_DETECTION_XY_AXISES (1<<6) | (1<<7) | (1<<3) | (1<<1) | (1<<5)
#define I1_IA1 (1<<6)
#define I2_IA2 (1<<5)
 




enum i2c_accel_type {
	LIS3DH
};


struct i2c_accel {
        struct pl_i2c *i2c;
	uint8_t i2c_addr;
	enum i2c_accel_type type;
};

extern int accel_init(const struct i2c_accel *accel);
extern int accel_get_data(const struct i2c_accel *accel, uint16_t *data);
extern uint8_t accel_get_status_reg(const struct i2c_accel *accel, uint8_t register_addr);
#endif /* PL_ENDIAN_H */