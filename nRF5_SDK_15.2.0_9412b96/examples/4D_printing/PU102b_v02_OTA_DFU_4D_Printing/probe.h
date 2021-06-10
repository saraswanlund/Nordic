#ifndef INCLUDE_PROBE_H
#define INCLUDE_PROBE_H 1

//#include <FatFs/ff.h>

struct pl_platform;
struct s1d13541;
struct pl_i2c;
struct pl_dispinfo;
struct i2c_eeprom;
struct pl_wflib_eeprom_ctx;
struct vcom_cal;
struct pl_epdpsu_gpio;
struct tps65185_info;

extern int probe_hwinfo(struct pl_platform *plat,
			const struct i2c_eeprom *hw_eeprom,
			struct pl_hwinfo *hwinfo_eeprom,
			const struct pl_hwinfo *hwinfo_default);
extern int probe_i2c(struct pl_platform *plat, struct s1d13541 *s1d13541,
		     struct pl_i2c *host_i2c, struct pl_i2c *disp_i2c);
extern int probe_dispinfo(struct pl_dispinfo *dispinfo, struct pl_wflib *wflib, const struct i2c_eeprom *e, struct pl_wflib_eeprom_ctx *e_ctx);
extern int probe_hvpmic(struct pl_platform *plat, struct vcom_cal *vcom_cal,
			struct pl_epdpsu_gpio *epdpsu_gpio,
			struct tps65185_info *pmic_info);
extern int probe_epdc(struct pl_platform *plat, struct s1d13541 *s1d13541);

#endif /* INCLUDE_PROBE_H */
