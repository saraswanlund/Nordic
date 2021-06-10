/**
 * Copyright (c) 2015 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef EEPROM_I2C_H__
#define EEPROM_I2C_H__

#include "sdk_errors.h"

/* TWI instance ID. */
#define TWI_INSTANCE_ID     1

/* Common addresses definition for temperature sensor. */
#define HWINFO_EEPROM_ADDR          0x50

#define EEPROM_SCL         9    //!< SCL pin.
#define EEPROM_SDA         10   //!< SDA pin.

/** Transation flags */
enum pl_eeprom_flags {
	PL_EEPROM_NO_STOP  = 1,       /**< Do not generate stop bit */
	PL_EEPROM_NO_START = 0,       /**< Do not generate start bit */
};

enum i2c_eeprom_type {
	EEPROM_24LC014,
	EEPROM_24AA256,
	EEPROM_AT214CM02,
        EEPROM_CAT24M01
};

struct i2c_eeprom {
        struct pl_i2c *i2c;
	uint8_t i2c_addr;
	enum i2c_eeprom_type type;
};

extern void eeprom_twi_init (void);
extern void read_eeprom_data(void);
extern void write_eeprom_data(void);

extern int eeprom_read(const struct i2c_eeprom *eeprom, uint32_t offset, uint16_t count, uint8_t *data);
extern int eeprom_write(const struct i2c_eeprom *eeprom, uint32_t offset, uint16_t count, uint8_t *data);

#endif // EEPROM_I2C_H__