#ifndef INCLUDE_PL_I2C_H
#define INCLUDE_PL_I2C_H 1

#include <stdint.h>
#include "nrf_drv_twi.h"

/**
   @file pl/i2c.h

   Abstract interface to use an I2C bus master.
*/
#define PL_I2C_SCL            9
#define PL_I2C_SDA            10

#define TPS65185_ADDR          (0x68)
#define PGSTAT_REG             (0x10)

/** Transation flags */
enum pl_i2c_flags {
	PL_I2C_NO_STOP  = 1 << 0,       /**< Do not generate stop bit */
	PL_I2C_NO_START = 1 << 1,       /**< Do not generate start bit */
};

/** Interface to be populated with concrete implementations */
struct pl_i2c {
	/**
	   read some data on the I2C bus
	   @param[in] i2c this pl_i2c instance
	   @param[in] addr 8-bit I2C address
	   @param[out] data buffer to receive the data being read
	   @param[in] count number of bytes to read
	   @param[in] flags flags bitmask using pl_i2c_flags
	   @return -1 if error, 0 otherwise
	 */
         int (*read)(struct pl_i2c *i2c, 
                          uint8_t               address,
                          uint8_t *             p_data,
                          uint8_t               length);
//	int (*read)(struct pl_i2c *i2c, uint8_t addr,
//		    uint8_t *data, uint8_t count, uint8_t flags);

	/**
	   write some data on the I2C bus
	   @param[in] i2c this pl_i2c instance
	   @param[in] addr 8-bit I2C address
	   @param[in] data buffer with data to be written
	   @param[in] count number of bytes to write
	   @param[in] flags flags bitmask using pl_i2c_flags
	   @return -1 if error, 0 otherwise
	 */
         int (*write)(struct pl_i2c *i2c, 
                          uint8_t               address,
                          uint8_t const *       p_data,
                          uint8_t               length,
                          bool                  no_stop);
//	int (*write)(struct pl_i2c *i2c, uint8_t addr,
//		     const uint8_t *data, uint8_t count, uint8_t flags);

	/**
	   free the resources associated with this instance
	   @param i2c this pl_i2c instance
	 */
	void (*free)(struct pl_i2c *i2c);

	/**
	   private data specific to this instance
	 */
	void *priv;
};

/** Read an 8-bit register on the I2C bus
    @param[in] i2c pl_i2c instance
    @param[in] addr I2C address
    @param[in] reg register address
    @param[out] data pointer to store the 8-bit register value
    @return -1 if error, 0 otherwise
 */
extern int pl_i2c_reg_read_8(struct pl_i2c *i2c, uint8_t addr,
			     uint8_t reg, uint8_t *data);

/** Write an 8-bit register on the I2C bus
    @param[in] i2c pl_i2c instance
    @param[in] addr I2C address
    @param[in] reg register address
    @param[in] data 8-bit register value
    @return -1 if error, 0 otherwise
 */
extern int pl_i2c_reg_write_8(struct pl_i2c *i2c, uint8_t addr,
			      uint8_t reg, uint8_t data);

/** Read a 16-bit big-endian register on the I2C bus
    @param[in] i2c pl_i2c instance
    @param[in] addr I2C address
    @param[in] reg register address
    @param[out] data pointer to store the 16-bit register value
    @return -1 if error, 0 otherwise
 */
extern int pl_i2c_reg_read_16be(uint8_t addr,
				uint8_t reg, uint16_t *data);

/** Write a 16-bit big-endian register on the I2C bus
    @param[in] i2c pl_i2c instance
    @param[in] addr I2C address
    @param[in] reg register address
    @param[in] data 16-bit register value
    @return -1 if error, 0 otherwise
 */
extern int pl_i2c_reg_write_16be(uint8_t addr,
				 uint8_t reg, uint16_t data);

extern void pl_twi_init(struct pl_i2c *i2c);

extern int read_i2c_data_8(uint8_t i2c_addr, uint8_t *data, uint8_t count, uint8_t flags);
extern int write_i2c_data_8(uint8_t i2c_addr, uint8_t *data, uint8_t count, uint8_t flags);
#endif /* INCLUDE_PL_I2C_H */