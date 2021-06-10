#include "pl_i2c.h"
#include <stdlib.h>
//#include "assert.h"
#include "vcom.h"
#include "pmic-tps65185.h"

//#define LOG_TAG "tps65185"
//#include "utils.h"

/* Set to 1 to dump registers */
#define DO_REG_DUMP 0

#define HVPMIC_DAC_MAX          ((1 << 9)-1)
#define HVPMIC_DAC_MIN          0
#define HVPMIC_TEMP_DEFAULT     20
#define HVPMIC_VERSION          0x65

#if 0
#define MV_DIV	33		// Each DAC step is 33mV
#endif

enum tps65185_register {
	HVPMIC_REG_TMST_VALUE = 0x00,
	HVPMIC_REG_ENABLE     = 0x01,
	HVPMIC_REG_VADJ       = 0x02,
	HVPMIC_REG_VCOM1      = 0x03,
	HVPMIC_REG_VCOM2      = 0x04,
	HVPMIC_REG_INT_EN1    = 0x05,
	HVPMIC_REG_INT_EN2    = 0x06,
	HVPMIC_REG_INT1       = 0x07,
	HVPMIC_REG_INT2       = 0x08,
	HVPMIC_REG_UPSEQ0     = 0x09,
	HVPMIC_REG_UPSEQ1     = 0x0A,
	HVPMIC_REG_DWNSEQ0    = 0x0B,
	HVPMIC_REG_DWNSEQ1    = 0x0C,
	HVPMIC_REG_TMST1      = 0x0D,
	HVPMIC_REG_TMST2      = 0x0E,
	HVPMIC_REG_PG_STAT    = 0x0F,
	HVPMIC_REG_REV_ID     = 0x10,
	HVPMIC_REG_MAX
};

union tps65185_version {
	struct {
		char version:4;
		char minor:2;
		char major:2;
	} v;
	uint8_t byte;
};

struct pmic_data {
	uint8_t reg;
	uint8_t data;
};

static const struct pmic_data init_data[] = {
	{ HVPMIC_REG_ENABLE,     0x00 },
	{ HVPMIC_REG_VADJ,       0x03 },
	{ HVPMIC_REG_VCOM1,      0x00 },
	{ HVPMIC_REG_VCOM2,      0x00 },
	{ HVPMIC_REG_INT_EN1,    0x00 },
	{ HVPMIC_REG_INT_EN2,    0x00 },
	{ HVPMIC_REG_UPSEQ0,     0x78 },
	{ HVPMIC_REG_UPSEQ1,     0xFF }, // changed from 0x00 to 0xFF to maximize the delay from 3ms to 12ms - 11.8.2018, refer to TPS65185 UPSEQ1 Register
	{ HVPMIC_REG_DWNSEQ0,    0x00 },
	{ HVPMIC_REG_DWNSEQ1,    0x00 },
	{ HVPMIC_REG_TMST1,      0x00 },
	{ HVPMIC_REG_TMST2,      0x78 }
};

int tps65185_init(struct tps65185_info *p, struct pl_i2c *i2c,
		  uint8_t i2c_addr, const struct vcom_cal *cal)
{
	union tps65185_version ver;
	int i;
	uint8_t pgstat;
	uint8_t int1, int2;

	p->i2c = i2c;
	p->i2c_addr = i2c_addr;
	p->cal = cal; /* Cal may be NULL if not being used */

	if (pl_i2c_reg_read_8(i2c, i2c_addr, HVPMIC_REG_REV_ID, &ver.byte))
		return -1;

	printf("Version: %d.%d.%d\n", ver.v.major, ver.v.minor, ver.v.version);

	if (ver.byte != HVPMIC_VERSION) {
		printf("Wrong version: 0x%02X instead of 0x%02X\n", ver.byte, HVPMIC_VERSION);
		return -1;
	}

	for (i = 0; i < ARRAY_SIZE(init_data); i++) {
		if (pl_i2c_reg_write_8(i2c, i2c_addr, init_data[i].reg, init_data[i].data))
			return -1;

                //pl_i2c_reg_read_8(i2c, i2c_addr, init_data[i].reg, &pgstat);    // debugging purpose
                //printf("TPS65185: %d\n", pgstat);
	}

	return 0;
}

/* program the internal VCOM Dac to give us the required voltage */
int tps65185_set_vcom_voltage(struct tps65185_info *p, int mv)
{
	int dac_value;
	uint8_t v1;
	uint8_t v2;

//	assert(p != NULL);

	dac_value = vcom_calculate(p->cal, mv);

	if (dac_value < HVPMIC_DAC_MIN)
		dac_value = HVPMIC_DAC_MIN;
	else if (dac_value > HVPMIC_DAC_MAX)
		dac_value = HVPMIC_DAC_MAX;

	v1 = dac_value & 0x00FF;
	v2 = ((dac_value >> 8) & 0x0001);

	if (pl_i2c_reg_write_8(p->i2c, p->i2c_addr, HVPMIC_REG_VCOM1, v1))
	    return -1;

	return pl_i2c_reg_write_8(p->i2c, p->i2c_addr, HVPMIC_REG_VCOM2, v2);
}

#if 1 /* ToDo: use or remove */
/* use i2c to determine when power up has completed */
int tps65185_wait_pok(struct tps65185_info *p)
{
	uint8_t pgstat;
	uint8_t int1, int2;

//	assert(p != NULL);

	if (pl_i2c_reg_read_8(p->i2c, p->i2c_addr, HVPMIC_REG_PG_STAT,
			   &pgstat))
		return -1;

	if (pl_i2c_reg_read_8(p->i2c, p->i2c_addr, HVPMIC_REG_INT1, &int1) ||
	    pl_i2c_reg_read_8(p->i2c, p->i2c_addr, HVPMIC_REG_INT2, &int2))
		return -1;

//	abort_msg("TPS65185 POK feature not tested", ABORT_UNDEFINED);

	return 0;
}
#endif

/* use the i2c interface to power up the PMIC */
int tps65185_enable(struct tps65185_info *p)
{
	return -1;
}

/* use the i2c interface to power down the PMIC */
int tps65185_disable(struct tps65185_info *p)
{
	return -1;
}

int tps65185_temperature_measure(struct tps65185_info *p, int16_t *measured)
{
	int8_t temp;
	uint8_t progress;

	/* Trigger conversion */
	if (pl_i2c_reg_write_8(p->i2c, p->i2c_addr,HVPMIC_REG_TMST1, 0x80))
		return -1;

	/* wait for it to complete */
	do {
		if (pl_i2c_reg_read_8(p->i2c, p->i2c_addr, HVPMIC_REG_TMST1,
				      &progress))
			return -1;
	} while ((progress & 0x20));

	/* read the temperature */
	if (pl_i2c_reg_read_8(p->i2c, p->i2c_addr, HVPMIC_REG_TMST_VALUE,
			      (uint8_t *)&temp)) {
		temp = HVPMIC_TEMP_DEFAULT;
//		LOG("Warning: using default temperature %d", temp);
	}

	*measured = temp;

	return 0;
}
