#ifndef EPSON_S1D13541_H__
#define EPSON_S1D13541_H__

#include <stdint.h>
#include <stdlib.h>
#include "epdc.h"

struct pl_gpio;
struct pl_wflib;

#define S1D135XX_TEMP_MASK                   0x00FF

enum s1d135xx_reg {
	S1D135XX_REG_REV_CODE              = 0x0002,
	S1D135XX_REG_SOFTWARE_RESET        = 0x0008,
	S1D135XX_REG_SYSTEM_STATUS         = 0x000A,
	S1D135XX_REG_I2C_CLOCK             = 0x001A,
	S1D135XX_REG_PERIPH_CONFIG         = 0x0020,
	S1D135XX_REG_HOST_MEM_PORT         = 0x0154,
	S1D135XX_REG_I2C_TEMP_SENSOR_VALUE = 0x0216,
	S1D135XX_REG_I2C_STATUS            = 0x0218,
	S1D135XX_REG_PWR_CTRL              = 0x0230,
	S1D135XX_REG_SEQ_AUTOBOOT_CMD      = 0x02A8,
	S1D135XX_REG_DISPLAY_BUSY          = 0x0338,
	S1D135XX_REG_INT_RAW_STAT          = 0x033A,
};

enum s1d135xx_rot_mode {
	S1D135XX_ROT_MODE_0   = 0,
	S1D135XX_ROT_MODE_90  = 1,
	S1D135XX_ROT_MODE_180 = 2,
	S1D135XX_ROT_MODE_270 = 3,
};

struct s1d13541_data{
  unsigned reset;
  unsigned cs0;
  unsigned hirq;
  unsigned hrdy;
  unsigned hdc;
  unsigned clk_en;
  unsigned vcc_en;
 };

 struct s1d13541{
  const struct s1d13541_data *data;
  struct pl_gpio *gpio;
  uint16_t hrdy_mask;
  uint16_t hrdy_result;
  int measured_temp;
  unsigned xres;
  unsigned yres;
  struct {
    uint8_t needs_update:1;
  }flags;
 };


extern void s1d13541_hard_reset(const struct s1d13541_data *data);
extern int s1d13541_soft_reset(struct s1d13541 *p);
extern int s1d13541_check_prod_code(struct s1d13541 *p, uint16_t ref_code);
extern int s1d13541_load_init_code_withoutSD(struct s1d13541 *p);
extern int s1d13541_load_wflib(struct s1d13541 *p, struct pl_wflib *wflib,
			       uint32_t addr);
extern int s1d13541_init_gate_drv(struct s1d13541 *p);
extern int s1d13541_wait_dspe_trig(struct s1d13541 *p);
extern int s1d13541_clear_init(struct s1d13541 *p);
extern int s1d13541_fill(struct s1d13541 *p, uint16_t mode, unsigned bpp,
			 const struct pl_area *a, uint8_t grey);
extern int s1d13541_fill_pixel_xy(struct s1d13541 *p, uint8_t grey, uint16_t pixel_x, uint16_t pixel_y);
extern int s1d13541_fill_pixel(struct s1d13541 *p, unsigned int packet_num, uint8_t grey);
extern int s1d13541_update(struct s1d13541 *p, int wfid,
			   const struct pl_area *area);
extern int s1d13541_wait_update_end(struct s1d13541 *p);
extern int s1d13541_wait_idle(struct s1d13541 *p);
extern int s1d13541_set_power_state(struct s1d13541 *p,
				    enum pl_epdc_power_state state);
extern int s1d13541_set_epd_power(struct s1d13541 *p, int on);
extern void s1d13541_cmd(struct s1d13541 *p, uint16_t cmd,
			 const uint16_t *params, size_t n);
extern uint16_t s1d13541_read_reg(struct s1d13541 *p, uint16_t reg);
extern void s1d13541_write_reg(struct s1d13541 *p, uint16_t reg, uint16_t val);
extern int s1d13541_load_register_overrides(struct s1d13541 *p);

extern int epson_epdc_early_init_s1d13541(struct s1d13541 *p);
extern int epson_epdc_init_s1d13541(struct pl_epdc *epdc);

#endif // EPSON_S1D13541_H__
