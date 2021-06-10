#ifndef INCLUDE_PL_WFLIB_H
#define INCLUDE_PL_WFLIB_H 1


#include <stdint.h>
#include <stdlib.h>

struct pl_wflib;
struct i2c_eeprom;
struct pl_dispinfo;

/** Function type to write data to the output (i.e. the EPDC) */
typedef int (*pl_wflib_wr_t)(void *ctx, const uint8_t *data, size_t n);

/** Generic interface to load a waveform library */
struct pl_wflib {
	int (*xfer)(struct pl_wflib *wflib, pl_wflib_wr_t wr, void *ctx);
	uint32_t size;
	void *priv;
};

/** Initialise a wflib interface for a FatFS file */
//extern int pl_wflib_init_fatfs(struct pl_wflib *wflib, FIL *f,
//			       const char *path);

/** Structure to use to load LZSS-compressed waveform library from EEPROM */
struct pl_wflib_eeprom_ctx {
	const struct i2c_eeprom *eeprom;
	const struct pl_dispinfo *dispinfo;
	uint16_t offset;
};

/** Initialise a wflib interface for an I2C EEPROM + LZSS */
extern int pl_wflib_init_eeprom(struct pl_wflib *wflib,
				struct pl_wflib_eeprom_ctx *p,
				const struct i2c_eeprom *eeprom,
				const struct pl_dispinfo *dispinfo);

#endif /* INCLUDE_PL_WFLIB_H */