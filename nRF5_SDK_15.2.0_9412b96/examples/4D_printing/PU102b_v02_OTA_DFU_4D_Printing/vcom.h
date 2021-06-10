#ifndef VCOM_H_
#define VCOM_H_

#include "hwinfo.h"
#include <stdint.h>

struct vcom_cal {
	int32_t swing;
	int32_t swing_ideal;
	int32_t dac_offset;
	int32_t dac_dx;
	int32_t dac_dy;
	int32_t dac_step_mv;
};

/** Initialise a vcom_cal structure */
extern void vcom_init(struct vcom_cal *v, const struct pl_hw_vcom_info *c);

/** Get the DAC register value for a given VCOM input voltage */
extern int vcom_calculate(const struct vcom_cal *v, int input_mv);

#endif /* VCOM_H_ */
