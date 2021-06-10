#include "epdc.h"
#include "epson-epdc.h"
#include "epson-s1d135xx.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "platform.h"

static int epson_epdc_clear_init(struct pl_epdc *epdc)
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_clear_init(p);
}

static int epson_epdc_update(struct pl_epdc *epdc, int wfid,
			     const struct pl_area *area)
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_update(p, wfid, area);
}

static int epson_epdc_wait_update_end(struct pl_epdc *epdc)
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_wait_update_end(p);
}

static int epson_epdc_set_power(struct pl_epdc *epdc,
				enum pl_epdc_power_state state)
{
	struct s1d13541 *p = epdc->data;

	if (s1d135xx_set_power_state(p, state))
		return -1;

	epdc->power_state = state;

	return 0;
}

static int epson_epdc_set_epd_power(struct pl_epdc *epdc, int on)
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_set_epd_power(p, on);
}

int epson_epdc_init(struct pl_epdc *epdc, const struct pl_dispinfo *dispinfo,
		    enum epson_epdc_ref ref, struct s1d13541 *s1d13541)
{
	int stat;

//	assert(epdc != NULL);
//	assert(dispinfo != NULL);
//	assert(s1d13541 != NULL);
//	assert(s1d13541->data != NULL);

	if (s1d13541->data->hrdy != PL_GPIO_NONE)
		printf("Using HRDY GPIO\n");

	if (s1d13541->data->hdc != PL_GPIO_NONE)
		printf("Using HDC GPIO\n");

	s1d13541->flags.needs_update = 0;

	epdc->clear_init = epson_epdc_clear_init;
	epdc->update = epson_epdc_update;
	epdc->wait_update_end = epson_epdc_wait_update_end;
	epdc->set_power = epson_epdc_set_power;
	epdc->set_epd_power = epson_epdc_set_epd_power;
	epdc->data = s1d13541;
	epdc->dispinfo = dispinfo;

	switch (ref) {
            case EPSON_EPDC_S1D13541:
                    stat = epson_epdc_init_s1d13541(epdc);
                    break;
            default:
                    printf("Invalid Epson ref\n");
	}

	if (stat)
		return -1;

        // skip the step of loading wflib to EEPROM / SD card
	printf("Loading wflib\n");

	if (epdc->load_wflib(epdc))
		return -1;

	s1d13541->xres = epdc->xres;
	s1d13541->yres = epdc->yres;

	printf("Ready %dx%d\n", epdc->xres, epdc->yres);

	return 0;
}

int epson_epdc_early_init(struct s1d13541 *p, enum epson_epdc_ref ref)
{
	int stat;

	switch (ref) {
	case EPSON_EPDC_S1D13541:
		stat = epson_epdc_early_init_s1d13541(p);
		break;
//	default:
//		assert_fail("Invalid Epson ref");
	}

	return stat;
}
