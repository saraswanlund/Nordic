#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "eeprom-i2c.h"
#include "pl_endian.h"
#include "pl_i2c.h"

#define min(x,y)	( (x) < (y) ? (x) : (y) )

enum i2c_eeprom_flags {
	EEPROM_16BIT_ADDRESS = 0x01
};

struct eeprom_data {
	uint32_t size;               /* size in bytes (-1) */
	uint16_t page_size;           /* size of a page in bytes */
	uint8_t flags;               /* flags from i2c_eeprom_flags */
};

static struct eeprom_data device_data[] = {
	{ 0x007F, 16, 0 },                   /* 24lc014 (128 Bytes) */
	{ 0x7FFF, 64, EEPROM_16BIT_ADDRESS }, /* 24aa256 (32 KBytes) */
	{ 0x001F4000, 256, EEPROM_16BIT_ADDRESS } /* AT214CM02 (2 Mbits, 256 KBytes) */
};

void read_i2c_data_16be(uint8_t i2c_addr, uint8_t reg, uint16_t *data);
void write_i2c_data_16be(uint8_t i2c_addr, uint8_t reg, uint16_t data);

/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done = false;

/* Buffer for samples read from temperature sensor. */
static uint8_t m_sample;

int eeprom_read(const struct i2c_eeprom *eeprom, uint32_t offset, uint16_t count, uint8_t *data)
{
    const struct eeprom_data *device;
    struct pl_i2c *i2c;
    uint8_t addr[2];
    uint8_t *addr_p;
    uint8_t addr_n;
    uint8_t i2c_addr;

    i2c_addr = eeprom->i2c_addr;

    device = &device_data[eeprom->type];

    if ((offset + count) >= device->size)
        return -1;

    // if offset is on second page, directly reads from next page
    if (offset >= 65536){
        offset = offset - 65536;
        i2c_addr = 0x51;
    }

    addr[1] = offset & 0x00FF;

//    printf("i2c_addr: %x \t addr_p: %x \t addr_n: %x\n", i2c_addr, &addr_p, addr_n);
//    printf("i2c_addr: %x \t addr: %x \t offset: %x\n", i2c_addr, addr[1], offset);

    if (device->flags & EEPROM_16BIT_ADDRESS) {
        addr[0] = (offset >> 8) & 0x00FF;
        addr_p = addr;
        addr_n = 2;
    } else {
        addr_p = &addr[1];
        addr_n = 1;
    }

//    printf("i2c_addr: %x \t addr_p: %x \t addr_n: %x\n", i2c_addr, *addr_p, addr_n);

    if(write_i2c_data_8(i2c_addr, addr_p, addr_n, 0))
        return -1;

    while (count) {
        const uint8_t n = min(count, 255);

        if(read_i2c_data_8(i2c_addr, data, n, 0))
            return -1;
        
        //printf("%x\n", *data);
        count -= n;
        data += n;
    }

    return 0;    
}

int eeprom_write(const struct i2c_eeprom *eeprom, uint32_t offset, uint16_t count, uint8_t *data)
{
    struct eeprom_data *device;
    struct pl_i2c *i2c;
    uint8_t write_data[2+count];
    int ret;
    uint8_t eeprom_address = eeprom->i2c_addr;

    device = &device_data[eeprom->type];

    if (offset + count >= device->size)
        return -1;

    if(offset >= 65536){
        offset -= 65536;
        eeprom_address = 0x51;
    }

    write_data[0] = ((offset >> 8) & 0x00FF);     // address first two bytes
    write_data[1] = (offset & 0x00FF);            // address second two bytes
   
    // TODO: without the printf, EEPROM_WRITE fails...
    printf("EEPROM_WRITE: Offset %d \t Address %02x%02x \n", offset, write_data[0], write_data[1]);

    memcpy(&write_data[2], data, count);          // comebine the address with data

    if (device->flags & EEPROM_16BIT_ADDRESS)
        ret = write_i2c_data_8(eeprom_address, write_data, 2+count, PL_EEPROM_NO_START);

    if (ret)
        return -1;

    return 0;
}