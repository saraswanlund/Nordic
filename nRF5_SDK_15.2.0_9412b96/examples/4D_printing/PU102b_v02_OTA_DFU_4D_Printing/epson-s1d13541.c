/*
 * epson-s1d13541.c -- Epson EPDC S1D13541
 *
 * Authors:
 *   Guillaume Tucker <guillaume.tucker@plasticlogic.com>
 *
 */

#include "epson-s1d135xx.h"
#include "epdc.h"
#include <stdlib.h>
//#include "assert.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_delay.h"
#include "nrf_gpio.h"

//#define LOG_TAG "s1d13541"
//#include "utils.h"

#define S1D13541_PROD_CODE              0x0053
#define S1D13541_STATUS_HRDY            (1 << 13)
#define S1D13541_INTERNAL_CLOCK_ENABLE  (1 << 7)
#define S1D13541_I2C_CLOCK_DIV          7 /* 100 kHz */
#define S1D13541_PROT_KEY_1             0x5678 /* ToDo: add to s1d135xx_data */
#define S1D13541_PROT_KEY_2             0x1234
#define S1D13541_TEMP_SENSOR_CONTROL    (1 << 14)
#define S1D13541_TEMP_SENSOR_EXTERNAL   (1 << 6)
#define S1D13541_AUTO_TEMP_JUDGE_EN     (1 << 2)
#define S1D13541_GENERIC_TEMP_EN        (1 << 15)
#define S1D13541_GENERIC_TEMP_JUDGE_EN  (1 << 14)
#define S1D13541_GENERIC_TEMP_MASK      0x01FF
#define S1D13541_INT_RAW_WF_UPDATE      (1 << 14)
#define S1D13541_INT_RAW_OUT_OF_RANGE   (1 << 10)
#define S1D13541_LD_IMG_1BPP            (0 << 4)
#define S1D13541_LD_IMG_2BPP            (1 << 4)
#define S1D13541_LD_IMG_4BPP            (2 << 4)
#define S1D13541_LD_IMG_8BPP            (3 << 4)
#define S1D13541_WF_ADDR                0x00080000L

enum s1d13541_reg {
	S1D13541_REG_CLOCK_CONFIG          = 0x0010,
	S1D13541_REG_PROT_KEY_1            = 0x042C,
	S1D13541_REG_PROT_KEY_2            = 0x042E,
	S1D13541_REG_FRAME_DATA_LENGTH     = 0x0400,
	S1D13541_REG_LINE_DATA_LENGTH      = 0x0406,
	S1D13541_REG_WF_DECODER_BYPASS     = 0x0420,
	S1D13541_REG_TEMP_SENSOR_VALUE     = 0x0576,
	S1D13541_REG_GENERIC_TEMP_CONFIG   = 0x057E,
};

enum s1d13541_cmd {
	S1D13541_CMD_RD_TEMP               = 0x12,
};

static const struct pl_wfid s1d13541_wf_table[] = {
	{ wf_refresh,      1 },
	{ wf_delta,        3 },
	{ wf_delta_mono,   2 },
	{ wf_refresh_mono, 4 },
	{ wf_init,         0 },
	{ NULL,           -1 }
};

/* -- private functions -- */

static int s1d13541_init_clocks(struct s1d13541 *p);
static void update_temp(struct s1d13541 *p, uint16_t reg);
static int update_temp_manual(struct s1d13541 *p, int manual_temp);
static int update_temp_auto(struct s1d13541 *p, uint16_t temp_reg);

/* -- pl_epdc interface -- */

static int s1d13541_load_wflib(struct pl_epdc *epdc)
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_load_wflib(p, &epdc->wflib, S1D13541_WF_ADDR);
}

static int s1d13541_set_temp_mode(struct pl_epdc *epdc,
				  enum pl_epdc_temp_mode mode)
{
	struct s1d13541 *p = epdc->data;
	uint16_t reg;

	if (mode == epdc->temp_mode)
		return 0;

	reg = s1d135xx_read_reg(p, S1D135XX_REG_PERIPH_CONFIG);
	/* ToDo: when do we set this bit back? */
	reg &= S1D13541_TEMP_SENSOR_CONTROL;

	switch (mode) {
	case PL_EPDC_TEMP_MANUAL:
		break;
	case PL_EPDC_TEMP_EXTERNAL:
		reg &= ~S1D13541_TEMP_SENSOR_EXTERNAL;
		break;
	case PL_EPDC_TEMP_INTERNAL:
		reg |= S1D13541_TEMP_SENSOR_EXTERNAL;
		break;
	default:
		//assert_fail("Invalid temperature mode");
                break;
	}

	s1d135xx_write_reg(p, S1D135XX_REG_PERIPH_CONFIG, reg);

	/* Configure the controller to automatically update the waveform table
	 * after each temperature measurement.  */
	reg = s1d135xx_read_reg(p, S1D13541_REG_WF_DECODER_BYPASS);
	reg |= S1D13541_AUTO_TEMP_JUDGE_EN;
	s1d135xx_write_reg(p, reg, S1D13541_REG_WF_DECODER_BYPASS);

	epdc->temp_mode = mode;

	return 0;
}

static int s1d13541_update_temp(struct pl_epdc *epdc)
{
	struct s1d13541 *p = epdc->data;
	int stat;

	switch (epdc->temp_mode) {
	case PL_EPDC_TEMP_MANUAL:
		stat = update_temp_manual(p, epdc->manual_temp);
		break;
	case PL_EPDC_TEMP_EXTERNAL:
		stat = update_temp_auto(p, S1D135XX_REG_I2C_TEMP_SENSOR_VALUE);
		break;
	case PL_EPDC_TEMP_INTERNAL:
		stat = update_temp_auto(p, S1D13541_REG_TEMP_SENSOR_VALUE);
		break;
	}

	if (stat)
		return -1;

	if (p->flags.needs_update) {
#if VERBOSE_TEMPERATURE
		LOG("Updating waveform table");
#endif

		if (s1d13541_load_wflib(epdc))
			return -1;
	}

	return 0;
}

static int s1d13541_fill(struct pl_epdc *epdc, const struct pl_area *area,
			 uint8_t grey)
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_fill(p, S1D13541_LD_IMG_8BPP, 8, area, grey);
}

static int s1d13541_fill_pixel_xy(struct pl_epdc *epdc, uint8_t grey, uint16_t pixel_x, uint16_t pixel_y)
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_fill_pixel_xy(p, grey, pixel_x, pixel_y);
}

static int s1d13541_fill_pixel(struct pl_epdc *epdc, uint32_t location, uint8_t grey)
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_fill_pixel(p, location, grey);
}

// function for displaying a horizontal line pattern - 10.9.17
static int s1d13541_horiz_line_pattern(struct pl_epdc *epdc, uint16_t line)
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_horiz_line_pattern(p, epdc->yres, epdc->xres, line, S1D13541_LD_IMG_8BPP);
}

// function for displaying a horizontal line pattern - 10.9.17
static int s1d13541_horiz_line_180_pattern(struct pl_epdc *epdc, uint16_t line)
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_horiz_line_180_pattern(p, epdc->yres, epdc->xres, line, S1D13541_LD_IMG_8BPP);
}

// function for displaying a vertical line pattern - 10.9.17
static int s1d13541_vert_line_pattern(struct pl_epdc *epdc, uint16_t line)
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_vert_line_pattern(p, epdc->yres, epdc->xres, line, S1D13541_LD_IMG_8BPP);
}

static int s1d13541_pattern_check(struct pl_epdc *epdc, uint16_t size)
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_pattern_check(p, epdc->yres, epdc->xres, size, S1D13541_LD_IMG_8BPP);
}

static int s1d13541_load_app_image(struct pl_epdc *epdc,
			       const struct pl_area *area, uint8_t data[])
{
	struct s1d13541 *p = epdc->data;

	return s1d135xx_load_app_image(p, S1D13541_LD_IMG_8BPP, 8, area, data);
}

//static int s1d13541_load_image(struct pl_epdc *epdc, const char *path,
//			       const struct pl_area *area, int left, int top)
//{
//	struct s1d13541 *p = epdc->data;
//
//	return s1d135xx_load_image(p, path, S1D13541_LD_IMG_8BPP, 8, area,
//				   left, top);
//}
//
//static int s1d13541_load_180_image(struct pl_epdc *epdc, const char *path,
//			       const struct pl_area *area, int left, int top)
//{
//	struct s1d13541 *p = epdc->data;
//
//	return s1d135xx_load_180_image(p, path, S1D13541_LD_IMG_8BPP, 8, area,
//				   left, top);
//}


/* -- initialisation -- */

int epson_epdc_early_init_s1d13541(struct s1d13541 *p)
{
	p->hrdy_mask = S1D13541_STATUS_HRDY;
	p->hrdy_result = S1D13541_STATUS_HRDY;
	p->measured_temp = -127;
	s1d135xx_hard_reset(p->gpio, p->data);

	if (s1d135xx_soft_reset(p))
		return -1;

	if (s1d135xx_check_prod_code(p, S1D13541_PROD_CODE))
		return -1;

	return s1d13541_init_clocks(p);
}

int epson_epdc_init_s1d13541(struct pl_epdc *epdc)
{
	struct s1d13541 *p = epdc->data;

	if (epson_epdc_early_init_s1d13541(p))
		return -1;

	// original code of using SD card to read ecode.bin file
//	if (s1d135xx_load_init_code(p)) {
//		LOG("Failed to load init code");
//		return -1;
//	}

	// reading ecode.bin data internally stored - SJE, 7.14.2018
	if (s1d135xx_load_init_code_withoutSD(p)) {
		printf("Failed to load init code without SD\n");
		return -1;
	}

//	if (s1d135xx_load_register_overrides(p)) {
//		LOG("Failed to load register overrides");
//		return -1;
//	}

	s1d135xx_write_reg(p, S1D13541_REG_PROT_KEY_1, S1D13541_PROT_KEY_1);
	s1d135xx_write_reg(p, S1D13541_REG_PROT_KEY_2, S1D13541_PROT_KEY_2);

	if (s1d135xx_wait_idle(p))
		return -1;

	if (epdc->set_power(epdc, PL_EPDC_RUN))
		return -1;

	if (s1d135xx_init_gate_drv(p))
		return -1;

	if (s1d135xx_wait_dspe_trig(p))
		return -1;

	epdc->load_wflib = s1d13541_load_wflib;
	epdc->set_temp_mode = s1d13541_set_temp_mode;
	epdc->update_temp = s1d13541_update_temp;
	epdc->fill = s1d13541_fill;
	epdc->fill_pixel = s1d13541_fill_pixel;								// function for filling pixel by pixel
	epdc->fill_pixel_xy = s1d13541_fill_pixel_xy;								// function for filling pixel by pixel
	epdc->horiz_line_pattern = s1d13541_horiz_line_pattern;				// function for displaying horizontal line pattern
	epdc->horiz_line_180_pattern = s1d13541_horiz_line_180_pattern;
	epdc->vert_line_pattern = s1d13541_vert_line_pattern;				// function for displaying vertical line pattern
	epdc->pattern_check = s1d13541_pattern_check;
        epdc->load_app_image = s1d13541_load_app_image;
//	epdc->load_image = s1d13541_load_image;
//	epdc->load_180_image = s1d13541_load_180_image;						// function for uploading image in 180 degrees
	epdc->wf_table = s1d13541_wf_table;
	epdc->xres = s1d135xx_read_reg(p, S1D13541_REG_LINE_DATA_LENGTH);
	epdc->yres = s1d135xx_read_reg(p, S1D13541_REG_FRAME_DATA_LENGTH);

	return epdc->set_temp_mode(epdc, PL_EPDC_TEMP_INTERNAL);
}

/* ----------------------------------------------------------------------------
 * private functions
 */

static int s1d13541_init_clocks(struct s1d13541 *p)
{
	s1d135xx_write_reg(p, S1D135XX_REG_I2C_CLOCK, S1D13541_I2C_CLOCK_DIV);
	s1d135xx_write_reg(p, S1D13541_REG_CLOCK_CONFIG,
			   S1D13541_INTERNAL_CLOCK_ENABLE);

	return s1d135xx_wait_idle(p);
}

static void update_temp(struct s1d13541 *p, uint16_t reg)
{
	uint16_t regval;

	regval = s1d135xx_read_reg(p, S1D135XX_REG_INT_RAW_STAT);
	p->flags.needs_update = (regval & S1D13541_INT_RAW_WF_UPDATE) ? 1 : 0;
        
	s1d135xx_write_reg(p, S1D135XX_REG_INT_RAW_STAT,
			   (S1D13541_INT_RAW_WF_UPDATE |
			    S1D13541_INT_RAW_OUT_OF_RANGE));
        printf("Update Temperature Write Reg\n");
	regval = s1d135xx_read_reg(p, reg) & S1D135XX_TEMP_MASK;

#if VERBOSE_TEMPERATURE
	if (regval != p->measured_temp)
		LOG("Temperature: %d", regval);
#endif

	p->measured_temp = regval;
}

static int update_temp_manual(struct s1d13541 *p, int manual_temp)
{
	uint16_t regval;

	regval = (S1D13541_GENERIC_TEMP_EN |
		  S1D13541_GENERIC_TEMP_JUDGE_EN |
		  (manual_temp & S1D13541_GENERIC_TEMP_MASK));
	s1d135xx_write_reg(p, S1D13541_REG_GENERIC_TEMP_CONFIG, regval);

	if (s1d135xx_wait_idle(p))
		return -1;

	update_temp(p, S1D13541_REG_GENERIC_TEMP_CONFIG);

	return 0;
}

static int update_temp_auto(struct s1d13541 *p, uint16_t temp_reg)
{
	if (s1d135xx_set_power_state(p, PL_EPDC_STANDBY))
		return -1;

	s1d135xx_cmd(p, S1D13541_CMD_RD_TEMP, NULL, 0);

	if (s1d135xx_wait_idle(p))
		return -1;

	if (s1d135xx_set_power_state(p, PL_EPDC_RUN))
		return -1;

	update_temp(p, temp_reg);

	return 0;
}