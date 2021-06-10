#include "epson-epdc.h"
#include "epson-i2c.h"
#include "platform.h"
#include "hwinfo.h"
#include "dispinfo.h"
#include "wflib.h"
#include <string.h>
#include <stdio.h>
#include "probe.h"
//#include "config.h"
//#include "i2c-eeprom.h"
#include "vcom.h"
#include "pmic-tps65185.h"
//#include "assert.h"

//#define LOG_TAG "probe"
#//include "utils.h"

/* ToDo: add to generic HV-PMIC interface */
#define I2C_PMIC_ADDR_TPS65185 0x68
#define I2C_PMIC_ADDR_MAX17135 0x48

int probe_i2c(struct pl_platform *plat, struct s1d13541 *s1d13541,
	      struct pl_i2c *host_i2c, struct pl_i2c *disp_i2c)
{
	static const char px[] = "I2C: ";
	int stat;

	switch (plat->hwinfo->board.i2c_mode) {
	case I2C_MODE_HOST: /* MSP430, I2C already initialised */                       // Switch case will go through this case
//		LOG("%sHost", px);
		stat = 0;
		plat->i2c = host_i2c;
		break;  
	case I2C_MODE_DISP:                                                             // not using I2C Mode, we are in SPI mode for Display
		/* This must be the Epson S1D13541...
		 *
		 * Ideally we should check the display info, but this requires
		 * I2C so it's not practical.  Alternatively, we could check
		 * the io_config and board_type info to have more clues.  It
		 * may also be possible to probe for the presence of the
		 * S1D13541 via SPI by reading the product code register for
		 * example.  */
//		LOG("%sS1D13541", px);
		stat = epson_i2c_init(s1d13541, disp_i2c, EPSON_EPDC_S1D13541);
		plat->i2c = disp_i2c;
		break;
//	case I2C_MODE_S1D13524:
//		LOG("%sS1D13524", px);
//		stat = epson_i2c_init(s1d13541, disp_i2c, EPSON_EPDC_S1D13524);
//		plat->i2c = disp_i2c;
//		break;
//	case I2C_MODE_SC18IS6XX:
//		LOG("%snot supported", px);
//		stat = -1;
//		break;
//	case I2C_MODE_NONE:
	default:  
            stat = epson_i2c_init(s1d13541, disp_i2c, EPSON_EPDC_S1D13541);
            break;
//		assert_fail("Invalid I2C mode");
	}
        
	return stat;
}

int probe_dispinfo(struct pl_dispinfo *dispinfo, struct pl_wflib *wflib,
		   const struct i2c_eeprom *e,
		   struct pl_wflib_eeprom_ctx *e_ctx)
{					
	return (pl_wflib_init_eeprom(wflib, e_ctx, e, dispinfo));           // use internally stored data, SJE - 7.16.2018

}

int probe_hvpmic(struct pl_platform *plat, struct vcom_cal *vcom_cal,
		 struct pl_epdpsu_gpio *epdpsu_gpio,
		 struct tps65185_info *pmic_info)
{
	static const char px_psu[] = "EPD PSU: ";
	static const char px_pmic[] = "HV-PMIC: ";
	const struct pl_hwinfo *hwinfo = plat->hwinfo;
	int stat;

	/* ToDo: use hwinfo->board.io_config instead */
	if (!strcmp(hwinfo->board.board_type, "Raven")) {
		/* Warning: This must not call the epdc functions yet... */
		printf("%sEPDC\n", px_psu);
		stat = pl_epdpsu_epdc_init(&plat->psu, &plat->epdc);
	} else {
		printf("%sGPIO\n", px_psu);
		stat = pl_epdpsu_gpio_init(&plat->psu, epdpsu_gpio);
	}

	if (stat) {
		printf("Failed to initialise EPD PSU\n");
		return -1;
	}

	vcom_init(vcom_cal, &hwinfo->vcom);

	switch (hwinfo->board.hv_pmic) {
	case HV_PMIC_NONE:
		printf("%sNone\n", px_pmic);
		stat = 0;
		break;
	case HV_PMIC_TPS65185:
		printf("%sTPS65185\n", px_pmic);
		stat = tps65185_init(pmic_info, plat->i2c,
				     I2C_PMIC_ADDR_TPS65185, vcom_cal);
		if (!stat) /* ToDo: generalise set_vcom with HV-PMIC API */
			stat = tps65185_set_vcom_voltage(
				pmic_info, plat->dispinfo->info.vcom);
		break;
	default:
		printf("Invalid HV-PMIC id\n");
                break;
	}

	return stat;
}

int probe_epdc(struct pl_platform *plat, struct s1d13541 *s1d13541)
{
	static const char px[] = "EPDC: ";
	const struct pl_hwinfo *hwinfo = plat->hwinfo;
	struct pl_epdc *epdc = &plat->epdc;
	int stat;

	switch (hwinfo->board.epdc_ref) {
	case EPDC_S1D13524:
		printf("%sS1D13524\n", px);
//		stat = epson_epdc_init(epdc, plat->dispinfo,
//				       EPSON_EPDC_S1D13524, s1d13541);
		break;
	case EPDC_S1D13541:
		printf("%sS1D13541\n", px);
		stat = epson_epdc_init(epdc, plat->dispinfo,
				       EPSON_EPDC_S1D13541, s1d13541);
		break;
	default:
//		assert_fail("Invalid EPDC identifier");
                break;
	}

	return stat;
}
