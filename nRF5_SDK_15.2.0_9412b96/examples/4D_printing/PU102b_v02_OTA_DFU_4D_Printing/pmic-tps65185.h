#ifndef INCLUDE_PMIC_TPS65185_H
#define INCLUDE_PMIC_TPS65185_H 1

#include <stdint.h>

struct pl_i2c;
struct vcom_cal;

struct tps65185_info {
	struct pl_i2c *i2c;
	uint8_t i2c_addr;
	const struct vcom_cal *cal;
};

extern int tps65185_init(struct tps65185_info *pmic, struct pl_i2c *i2c,
			 uint8_t i2c_addr, const struct vcom_cal *cal);
extern int tps65185_set_vcom_voltage(struct tps65185_info *pmic, int mv);
extern int tps65185_set_vcom_register(struct tps65185_info *pmic, int value);

extern int tps65185_wait_pok(struct tps65185_info *pmic);
extern int tps65185_enable(struct tps65185_info *pmic);
extern int tps65185_disable(struct tps65185_info *pmic);

extern int tps65185_temperature_measure(struct tps65185_info *pmic,
					int16_t *measured);

#endif /* INCLUDE_PMIC_TPS65185_H */
