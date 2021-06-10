#ifndef INCLUDE_PL_PLATFORM_H
#define INCLUDE_PL_PLATFORM_H 1

#include "epdpsu.h"
#include "epdc.h"
#include "dispinfo.h"
//#include "i2c-eeprom.h"

#define PL_GPIO_NONE ((unsigned)-1)

struct pl_gpio{
    int (*config)(unsigned gpio, uint16_t flags);
};

struct pl_hwinfo;

/* Common platform data */
struct pl_platform {
	struct pl_gpio gpio;
	struct pl_epdpsu psu;
//	struct pl_wireless wls;					// added for wireless circuitries - SJE, 8.16.2018
	struct pl_epdc epdc;
	struct pl_i2c *i2c;
	const struct pl_system_gpio *sys_gpio;
	const struct pl_hwinfo *hwinfo;
	const struct pl_dispinfo *dispinfo;
};


#endif /* INCLUDE_PL_PLATFORM_H */