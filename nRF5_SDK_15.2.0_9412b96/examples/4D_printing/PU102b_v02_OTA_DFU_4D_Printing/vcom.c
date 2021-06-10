//#include "assert.h"
#include "vcom.h"
#include <stdlib.h>

//#define LOG_TAG "vcom"
//#include "utils.h"

#define DIV_ROUND_CLOSEST(x, divisor)(				\
	{							\
		(((x) + ((divisor) / 2)) / (divisor));		\
	}							\
)

void vcom_init(struct vcom_cal *v, const struct pl_hw_vcom_info *c)
{
//	assert(v != NULL);
//	assert(c != NULL);

	v->dac_dx = c->dac_x2 - c->dac_x1;
	v->dac_dy = c->dac_y2 - c->dac_y1;
	v->dac_offset = c->dac_y1 -
		DIV_ROUND_CLOSEST((c->dac_x1 * v->dac_dy),  v->dac_dx);
	v->swing = c->vgpos_mv - c->vgneg_mv;
	v->swing_ideal = c->swing_ideal;
	v->dac_step_mv = DIV_ROUND_CLOSEST(v->dac_dy, v->dac_dx);
}

int vcom_calculate(const struct vcom_cal *v, int input_mv)
{
	int32_t scaled_mv;
	int dac_value;

//	assert(v != NULL);

	scaled_mv = DIV_ROUND_CLOSEST(input_mv * v->swing, v->swing_ideal);
	dac_value = DIV_ROUND_CLOSEST((scaled_mv - v->dac_offset) * v->dac_dx,
				      v->dac_dy);

//	LOG("input: %d, scaled: %ld, DAC reg: 0x%02X", input_mv, scaled_mv, dac_value);

	return dac_value;
}
