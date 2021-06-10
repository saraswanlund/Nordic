/*
  Plastic Logic EPD project on MSP430

  Copyright (C) 2014 Plastic Logic Limited

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * epson-s1d135xx.c -- Common Epson S1D135xx primitives
 *
 * Authors:
 *   
 *
 */

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_delay.h"
#include "nrf_gpio.h"

#include "epson-s1d135xx.h"
//#include <pl/gpio.h>
#include "platform.h"
#include "wflib.h"
#include "pl_endian.h"
//#include <pl/types.h>
#include <stdlib.h>
#include "pl_spi.h"
#include "assert.h"

/* until the i/o operations are abstracted */
//#include "pnm-utils.h"

//#define LOG_TAG "s1d135xx"
//#include "utils.h"

/* Set to 1 to enable verbose update and EPD power on/off log messages */
#define VERBOSE 1

#define DATA_BUFFER_LENGTH              512

#define S1D135XX_WF_MODE(_wf)           (((_wf) << 8) & 0x0F00)
#define S1D135XX_XMASK                  0x0FFF
#define S1D135XX_YMASK                  0x0FFF
#define S1D135XX_INIT_CODE_CHECKSUM_OK  (1 << 15)
#define S1D135XX_PWR_CTRL_UP            0x8001
#define S1D135XX_PWR_CTRL_DOWN          0x8002
#define S1D135XX_PWR_CTRL_BUSY          0x0080
#define S1D135XX_PWR_CTRL_CHECK_ON      0x2200

#define S1D13541_LD_IMG_1BPP            (0 << 4)
#define S1D13541_LD_IMG_2BPP            (1 << 4)
#define S1D13541_LD_IMG_4BPP            (2 << 4)
#define S1D13541_LD_IMG_8BPP            (3 << 4)

enum s1d135xx_cmd {
	S1D135XX_CMD_INIT_SET         = 0x00, /* to load init code */
	S1D135XX_CMD_RUN              = 0x02,
	S1D135XX_CMD_STBY             = 0x04,
	S1D135XX_CMD_SLEEP            = 0x05,
	S1D135XX_CMD_INIT_STBY        = 0x06, /* init then standby */
	S1D135XX_CMD_INIT_ROT_MODE    = 0x0B,
	S1D135XX_CMD_READ_REG         = 0x10,
	S1D135XX_CMD_WRITE_REG        = 0x11,
	S1D135XX_CMD_BST_RD_SDR       = 0x1C,
	S1D135XX_CMD_BST_WR_SDR       = 0x1D,
	S1D135XX_CMD_BST_END_SDR      = 0x1E,
	S1D135XX_CMD_LD_IMG           = 0x20,
	S1D135XX_CMD_LD_IMG_AREA      = 0x22,
	S1D135XX_CMD_LD_IMG_END       = 0x23,
	S1D135XX_CMD_WAIT_DSPE_TRG    = 0x28,
	S1D135XX_CMD_WAIT_DSPE_FREND  = 0x29,
	S1D135XX_CMD_UPD_INIT         = 0x32,
	S1D135XX_CMD_UPDATE_FULL      = 0x33,
	S1D135XX_CMD_UPDATE_FULL_AREA = 0x34,
	S1D135XX_CMD_EPD_GDRV_CLR     = 0x37,
};

static int get_hrdy(struct s1d13541 *p);
static int do_fill(struct s1d13541 *p, const struct pl_area *area,
		   unsigned bpp, uint8_t g);
static int do_fill_pixels(struct s1d13541 *p, uint8_t g);
static int wflib_wr(void *ctx, const uint8_t *data, size_t n);
//static int transfer_file(FIL *file);
//static int transfer_image(FIL *f, const struct pl_area *area, int left, int top, int width);
static void transfer_data(const uint8_t *data, size_t n);
static void ecode_transfer_data(void);
static void send_cmd_area(struct s1d13541 *p, uint16_t cmd, uint16_t mode,
			  const struct pl_area *area);
static void send_cmd_cs(struct s1d13541 *p, uint16_t cmd);
static void send_cmd(struct s1d13541 *p, uint16_t cmd);
static void send_params(const uint16_t *params, size_t n);
static void send_param(uint16_t param);
static void set_cs(struct s1d13541 *p, int state);
static void set_hdc(struct s1d13541 *p, int state);

uint16_t ecode[1174] = {0x0000, 0x00f1, 0x03e7, 0x0000, 0x6000, 0x0000, 0xe000, 0x0043,
					  0x0085, 0x009c, 0xe000, 0xe000, 0xe000, 0xe000, 0x213b, 0xe000,
					  0xe000, 0xe000, 0xe000, 0x2000, 0x4000, 0x013f, 0x019b, 0xe000,
					  0xe000, 0xe000, 0xe000, 0xe000, 0xe000, 0xe000, 0xe000, 0x81a2,
					  0x81cc, 0x01cf, 0xe000, 0x2288, 0xe000, 0xa2bf, 0x02c2, 0xe000,
					  0xe000, 0xe000, 0xe000, 0x02c4, 0x02d3, 0x02e4, 0x22e7, 0xe000,
					  0xe000, 0xe000, 0xe000, 0xe000, 0xe000, 0x02fe, 0x2300, 0xa378,
					  0x23bd, 0xa3c0, 0x03c3, 0xe000, 0xe000, 0xe000, 0xe000, 0xe000,
					  0x03c8, 0xe000, 0x03de, 0x708e, 0x0002, 0x0006, 0x0003, 0x0003,
					  0x003f, 0x708c, 0x0080, 0x3010, 0x9010, 0x0080, 0x0080, 0x7030,
					  0x0005, 0x7548, 0xff2f, 0x754a, 0x007e, 0x0020, 0x0080, 0x0080,
					  0x001b, 0x754e, 0x0008, 0x740c, 0x4000, 0xf027, 0x3230, 0x9230,
					  0x0001, 0x0003, 0x1230, 0x2000, 0x2000, 0x740c, 0x4000, 0x100a,
					  0x0000, 0x4000, 0x1560, 0x0000, 0x0004, 0x308c, 0xc560, 0x7562,
					  0x0ff0, 0x3560, 0x9560, 0x0001, 0x0001, 0x1560, 0x0000, 0x0004,
					  0x754e, 0x0000, 0x008e, 0x003d, 0xffff, 0x03d1, 0x008e, 0x0002,
					  0xffff, 0x4332, 0x3006, 0x9806, 0x0003, 0x0003, 0x708e, 0x0004,
					  0x0006, 0x0002, 0x0003, 0x0081, 0x0006, 0x0000, 0x0003, 0x007d,
					  0x100a, 0x0000, 0x409c, 0x740c, 0xc000, 0x0020, 0x0080, 0x0080,
					  0x0068, 0x1338, 0x0000, 0x0008, 0x1560, 0x0000, 0x0004, 0x7560,
					  0x0090, 0x7562, 0x0040, 0x3560, 0x9560, 0x0001, 0x0001, 0x1560,
					  0x0000, 0x0004, 0xf06f, 0x3230, 0x9230, 0x0002, 0x0003, 0x100a,
					  0x0000, 0x4000, 0x7030, 0x0000, 0x008e, 0x0005, 0xffff, 0x0093,
					  0x008e, 0x003f, 0xffff, 0x03e7, 0x008e, 0x0029, 0xffff, 0x02e3,
					  0x3010, 0x9010, 0x0080, 0x0080, 0x3006, 0x9806, 0x0002, 0x0003,
					  0x708e, 0x0005, 0x0006, 0x0000, 0x0003, 0x009b, 0x0006, 0x0002,
					  0x0003, 0x0093, 0x100a, 0x0000, 0x409c, 0xf050, 0x3006, 0x9006,
					  0x0000, 0x0003, 0x3010, 0x9810, 0x0000, 0x0080, 0x3800, 0x708e,
					  0x0006, 0x0006, 0x0002, 0x0003, 0x413a, 0x3010, 0x9010, 0x0001,
					  0x001f, 0x701a, 0x0001, 0x701c, 0x0f05, 0x7082, 0x0f05, 0x701e,
					  0x0f00, 0x7020, 0x0080, 0x7234, 0x000a, 0x7236, 0x0002, 0x7238,
					  0x0000, 0x732c, 0x0014, 0x7410, 0x0006, 0x7412, 0x0043, 0x7414,
					  0x001f, 0x7416, 0x001f, 0x7418, 0x0006, 0x741a, 0x0043, 0x741c,
					  0x0003, 0x741e, 0x0000, 0x7422, 0x9b8c, 0x7424, 0x0000, 0x7426,
					  0x0000, 0x7428, 0x0000, 0x7432, 0xb737, 0x7434, 0x7531, 0x7436,
					  0x0db9, 0x7464, 0x00e0, 0x7466, 0x0001, 0x7468, 0x0001, 0x7510,
					  0x557d, 0x7512, 0x0514, 0x7514, 0x0100, 0x7516, 0xfd02, 0x7518,
					  0xfffe, 0x751a, 0x0403, 0x751c, 0x3f3f, 0x751e, 0x0000, 0x7520,
					  0x0200, 0x7522, 0x5406, 0x7524, 0xff54, 0x7526, 0xff06, 0x7528,
					  0x0006, 0x752a, 0x0006, 0x752c, 0x01ff, 0x752e, 0x02ff, 0x7530,
					  0x6474, 0x7532, 0xffff, 0x7534, 0x0010, 0x7550, 0x0010, 0x7558,
					  0x0000, 0x755a, 0x020a, 0x755c, 0x2800, 0x7570, 0x0fff, 0x757e,
					  0x0000, 0x7164, 0x2600, 0x7166, 0x0007, 0x7420, 0x0006, 0x1420,
					  0x0000, 0x0080, 0x7502, 0x0008, 0x1500, 0x0000, 0x1000, 0x7020,
					  0x8080, 0xf14d, 0x7020, 0x0080, 0x7400, 0x0001, 0x7334, 0x000d,
					  0x1338, 0x0000, 0x0001, 0x7400, 0x00f0, 0x7402, 0x0002, 0x7404,
					  0x0203, 0x7406, 0x0190, 0x7408, 0x0002, 0x740a, 0x0706, 0x740c,
					  0xc000, 0x740e, 0x8006, 0x7b30, 0x0080, 0x3800, 0x3140, 0x8940,
					  0x0001, 0x0700, 0x708e, 0x0012, 0x0006, 0x0002, 0x0003, 0x419a,
					  0x753e, 0x0000, 0x7540, 0x0180, 0x7542, 0x0000, 0x7020, 0x8080,
					  0x0020, 0x0040, 0x0040, 0x0183, 0x1560, 0x0000, 0x0004, 0x7240,
					  0x1000, 0x3082, 0xc01c, 0x7560, 0x0070, 0x7562, 0x03e8, 0x008e,
					  0x0013, 0xffff, 0x0166, 0x7562, 0x2710, 0x353e, 0x953e, 0x00c0,
					  0x00c0, 0x3560, 0x9560, 0x0001, 0x0001, 0x1560, 0x0000, 0x0004,
					  0x008e, 0x0013, 0xffff, 0x4173, 0x7560, 0x00c0, 0x3576, 0x9576,
					  0x2000, 0x2000, 0x1576, 0x0100, 0x0100, 0x008e, 0x0013, 0xffff,
					  0x018c, 0x353e, 0x953e, 0x0000, 0x00c0, 0xf18c, 0x7210, 0x4800,
					  0x3214, 0x9214, 0x0001, 0x0001, 0x1212, 0x0000, 0x0001, 0x008e,
					  0x0006, 0xffff, 0x011f, 0x008e, 0x0013, 0xffff, 0x01a0, 0x7820,
					  0x0080, 0x0082, 0x8000, 0x8000, 0x01de, 0x3800, 0x708e, 0x0013,
					  0x7540, 0x007f, 0xf14d, 0x7d40, 0x00ff, 0x708c, 0x0005, 0x6144,
					  0x0001, 0x6146, 0x0002, 0x6148, 0x0003, 0x614a, 0x0004, 0x0146,
					  0x0008, 0x000f, 0xc1b6, 0x1338, 0x0000, 0x0008, 0x1140, 0x0000,
					  0x0800, 0x7142, 0x0002, 0x3140, 0x90c0, 0x0000, 0x0005, 0x308c,
					  0xc0c4, 0x30c0, 0xc140, 0x0148, 0x0000, 0xffff, 0x41ca, 0x014a,
					  0x0000, 0xffff, 0x41ca, 0x7942, 0x0002, 0x7942, 0x0001, 0x708c,
					  0x0001, 0xf1a4, 0x0146, 0x0008, 0x000f, 0x01d5, 0x7942, 0x0002,
					  0x142a, 0x0000, 0x0001, 0x7142, 0x0002, 0x0140, 0x0004, 0x0004,
					  0x0287, 0x7088, 0x0000, 0x0088, 0x0000, 0x0001, 0x0287, 0x0420,
					  0x0002, 0x0002, 0x4287, 0x0430, 0x0008, 0x0008, 0x0207, 0x701c,
					  0x0f05, 0x7082, 0x0f05, 0x7402, 0x0002, 0x7404, 0x0203, 0x7408,
					  0x0002, 0x740a, 0x0706, 0x7410, 0x0006, 0x7412, 0x0043, 0x7414,
					  0x001f, 0x7416, 0x001f, 0x7418, 0x0006, 0x741a, 0x0043, 0x741c,
					  0x0003, 0xf221, 0x701c, 0x0f07, 0x7082, 0x0f07, 0x7402, 0x0002,
					  0x7404, 0x0203, 0x7408, 0x0003, 0x740a, 0x0606, 0x7410, 0x0008,
					  0x7412, 0x0041, 0x7414, 0x0025, 0x7416, 0x0025, 0x7418, 0x0008,
					  0x741a, 0x0041, 0x741c, 0x0005, 0x3430, 0x90c0, 0x0000, 0xfff8,
					  0x30c0, 0xc0d2, 0x301c, 0xc0c0, 0x70c4, 0x0001, 0x30c0, 0x9082,
					  0x0000, 0xfff0, 0x00d2, 0x0000, 0xffff, 0x0236, 0x3082, 0xc0c4,
					  0xf22f, 0x70c4, 0x7fff, 0x30c0, 0xc01c, 0xc082, 0x0020, 0x0080,
					  0x0080, 0x0287, 0x3430, 0x90c0, 0x0000, 0xff1f, 0x70d4, 0x0303,
					  0x30c0, 0x70d4, 0x0304, 0xc0c4, 0x0430, 0x00a0, 0x00e0, 0x4256,
					  0x30c0, 0x9510, 0x557d, 0x00ff, 0x3550, 0x9550, 0x0010, 0x0010,
					  0xf287, 0x30c0, 0xc086, 0x3430, 0x90c0, 0x0000, 0xff1f, 0x70d4,
					  0x0204, 0x70c4, 0x0014, 0x30c0, 0x70c0, 0x001e, 0xc0c6, 0x70d4,
					  0x0304, 0x30c0, 0xc088, 0x3080, 0xc0c0, 0x70c4, 0x001d, 0x3088,
					  0xc0c8, 0x10ce, 0x0001, 0x0001, 0x70ca, 0x02a0, 0x10ce, 0x0002,
					  0x0002, 0x30c0, 0xc0c6, 0xc0c6, 0x3080, 0xc0c4, 0x70de, 0x0000,
					  0x70dc, 0x00ff, 0x3086, 0xc0c4, 0x30c0, 0xc510, 0x3550, 0x9550,
					  0x0000, 0x0010, 0x3800, 0x708e, 0x0020, 0x7142, 0x0002, 0x3140,
					  0x9140, 0x0000, 0x0085, 0x3140, 0x8140, 0x0001, 0x0038, 0x008e,
					  0x0020, 0xffff, 0x02a1, 0x614c, 0x0002, 0x614e, 0x0003, 0x6150,
					  0x0004, 0x6152, 0x0005, 0xf2b6, 0x714c, 0x0000, 0x714e, 0x0000,
					  0x0140, 0x0100, 0x0700, 0x02b2, 0x0140, 0x0300, 0x0700, 0x02b2,
					  0x3406, 0xc150, 0x3400, 0xc152, 0xf2b6, 0x3406, 0xc152, 0x3400,
					  0xc150, 0x0140, 0x0000, 0x4000, 0x02bd, 0x1140, 0x0000, 0x4000,
					  0x7942, 0x0001, 0x708e, 0x0022, 0xf28a, 0x7942, 0x0002, 0x0334,
					  0x000a, 0x000e, 0x02cd, 0x0334, 0x0004, 0x000e, 0x02d0, 0x3800,
					  0x1b38, 0x0000, 0x0008, 0x1b38, 0x0000, 0x0001, 0x708e, 0x0029,
					  0x1338, 0x0000, 0x0008, 0x0558, 0x0000, 0x0001, 0x02df, 0x1560,
					  0x0000, 0x0004, 0x032c, 0x8000, 0x8000, 0x0050, 0x3800, 0x2b36,
					  0xf000, 0xf000, 0xa0d6, 0x0c01, 0x0000, 0x000f, 0x00d6, 0x0000,
					  0xf000, 0x02fd, 0x70d8, 0x0000, 0x70de, 0x0000, 0x3336, 0xc0c0,
					  0x30d6, 0xc0dc, 0x70c2, 0x0000, 0x00da, 0x0001, 0x0003, 0x02f3,
					  0x3800, 0x7b34, 0x0005, 0x708e, 0x0033, 0x708a, 0x0007, 0x7380,
					  0x0000, 0x7382, 0x0000, 0x3406, 0xc384, 0x3400, 0xc386, 0x0330,
					  0x0080, 0x0080, 0x0325, 0xa0c0, 0x0001, 0x3000, 0x0030, 0x70d4,
					  0x0204, 0x30c0, 0x70c0, 0x1000, 0xc0d4, 0x70de, 0x0000, 0x3336,
					  0x90dc, 0x0000, 0x0fff, 0x00c0, 0x0000, 0xf000, 0x431c, 0xf328,
					  0x2336, 0xf000, 0xf000, 0x008e, 0x0037, 0xffff, 0x0332, 0x708c,
					  0x0080, 0x032c, 0x8000, 0x8000, 0x000c, 0x0558, 0x0000, 0x0001,
					  0x034c, 0x0336, 0x0000, 0xf000, 0x434c, 0x1560, 0x0000, 0x0004,
					  0x3082, 0xc01c, 0x7560, 0x0020, 0x7562, 0xbb80, 0x3560, 0x9560,
					  0x0001, 0x0001, 0x1560, 0x0000, 0x0004, 0x7562, 0x2ee0, 0x008e,
					  0x0037, 0xffff, 0x03c6, 0x3334, 0xc0c0, 0x0338, 0x0008, 0x0008,
					  0x0365, 0x7088, 0x0000, 0x0088, 0x0000, 0x0001, 0x0365, 0x740e,
					  0x0006, 0x7334, 0x000b, 0x1338, 0x0000, 0x0001, 0x740e, 0x8006,
					  0x70de, 0x0002, 0x308a, 0xc0dc, 0x00c0, 0x0000, 0x000e, 0x0370,
					  0x1338, 0x0000, 0x0008, 0xa0c0, 0x0001, 0x0000, 0x0f30, 0x308a,
					  0xc0c4, 0x30c0, 0xcb34, 0x708a, 0x0007, 0x6380, 0x0002, 0x6382,
					  0x0003, 0x6384, 0x0004, 0x6386, 0x0005, 0x0140, 0x0100, 0x0700,
					  0x4395, 0x6380, 0x0003, 0x3400, 0xc0c0, 0x60c6, 0x0004, 0x60c6,
					  0x0002, 0x30c0, 0xc382, 0x6384, 0x0005, 0x6386, 0x0004, 0xf3bc,
					  0x0140, 0x0200, 0x0700, 0x43aa, 0x3406, 0xc0c0, 0x60c6, 0x0004,
					  0x60c6, 0x0002, 0x30c0, 0xc380, 0x3400, 0xc0c0, 0x60c6, 0x0005,
					  0x60c6, 0x0003, 0x30c0, 0xc382, 0xf3bc, 0x0140, 0x0300, 0x0700,
					  0x43bc, 0x3406, 0xc0c0, 0x60c6, 0x0005, 0x60c6, 0x0003, 0x30c0,
					  0xc380, 0x6382, 0x0002, 0x6384, 0x0005, 0x6386, 0x0004, 0xf30c,
					  0x708a, 0x0009, 0xf304, 0x708a, 0x0009, 0xf37a, 0x708e, 0x0037,
					  0xf30c, 0x7b34, 0x000b, 0x708e, 0x003d, 0x0006, 0x0002, 0x0003,
					  0x43dd, 0x708c, 0x00c0, 0xf00c, 0x7548, 0x0000, 0x754a, 0x0000,
					  0x753e, 0xfff3, 0x7540, 0x00fd, 0x7542, 0x0004, 0x7820, 0x8080,
					  0x3800, 0x708e, 0x003f, 0x7548, 0xff2f, 0x754a, 0x007e, 0x7020,
					  0x0080, 0xf050, 0x3800, 0x0003, 0x20a4, 0x0007, 0x005b, 0x0001,
					  0xffff, 0x01e6, 0x01e4, 0x01e2, 0x01e0, 0x01de, 0x01dc, 0x01da,
					  0x01d8, 0x01d6, 0x01d4, 0x01d2, 0x01cf, 0x01cc, 0x01c9, 0x01c6,
					  0x01c3, 0x01c0, 0x01bd, 0x01ba, 0x01b7, 0x01b4, 0x01b0, 0x01ac,
					  0x01a8, 0x01a4, 0x01a0, 0x019c, 0x0198, 0x0194, 0x0190, 0x018c,
					  0x0187, 0x0182, 0x017d, 0x0178, 0x0173, 0x016e, 0x0169, 0x0164,
					  0x015f, 0x015a, 0x0154, 0x014e, 0x0148, 0x0142, 0x013c, 0x0136,
					  0x0130, 0x012a, 0x0124, 0x011e, 0x0118, 0x0112, 0x010c, 0x0106,
					  0x0100, 0x00fb, 0x00f6, 0x00f1, 0x00ec, 0x00e7, 0x00e2, 0x00dd,
					  0x00d8, 0x00d3, 0x00ce, 0x00c9, 0x00c4, 0x00bf, 0x00ba, 0x00b5,
					  0x00b0, 0x00ab, 0x00a6, 0x00a1, 0x009c, 0x0098, 0x0094, 0x0090,
					  0x008c, 0x0088, 0x0084, 0x0080, 0x007c, 0x0078, 0x0074, 0x0071,
					  0x006e, 0x006b, 0x0068, 0x0065, 0x0000, 0x0000, 0x0000, 0x0000,
					  0x20a4, 0x0007, 0x215c, 0x0007, 0x6a8d, 0x0002};

/* ----------------------------------------------------------------------------
 * public functions
 */

void s1d135xx_hard_reset(struct pl_gpio *gpio,
			 const struct s1d13541_data *data)
{
	if (data->reset == PL_GPIO_NONE) {
		printf("Warning: no hard reset");
		return;
	}

	nrf_gpio_pin_write(data->reset, 0);
	nrf_delay_ms(4);
	nrf_gpio_pin_write(data->reset, 1);
	nrf_delay_ms(10);
}

int s1d135xx_soft_reset(struct s1d13541 *p)
{
	s1d135xx_write_reg(p, S1D135XX_REG_SOFTWARE_RESET, 0xFF);

	return s1d135xx_wait_idle(p);
}

int s1d135xx_check_prod_code(struct s1d13541 *p, uint16_t ref_code)
{
	uint16_t prod_code;

	prod_code = s1d135xx_read_reg(p, S1D135XX_REG_REV_CODE);

	printf("Product code: 0x%04X\n", prod_code);

	if (prod_code != ref_code) {
		printf("Invalid product code, expected 0x%04X\n", ref_code);
		return -1;
	}

	return 0;
}

//int s1d135xx_load_init_code(struct s1d135xx *p)
//{
//	static const char init_code_path[] = "bin/Ecode.bin";
//	FIL init_code_file;
//	uint16_t checksum;
//	int stat;
//
//	if (f_open(&init_code_file, init_code_path, FA_READ) != FR_OK)
//		return -1;
//
//	if (s1d135xx_wait_idle(p))
//		return -1;
//
//	set_cs(p, 0);
//	send_cmd(p, S1D135XX_CMD_INIT_SET);
//	stat = transfer_file(&init_code_file);
//	set_cs(p, 1);
//	f_close(&init_code_file);
//
//	if (stat) {
//		LOG("Failed to transfer init code file");
//		return -1;
//	}
//
//	if (s1d135xx_wait_idle(p))
//		return -1;
//
//	set_cs(p, 0);
//	send_cmd(p, S1D135XX_CMD_INIT_STBY);
//	send_param(0x0500);
//	set_cs(p, 1);
//	mdelay(100);
//
//	if (s1d135xx_wait_idle(p))
//		return -1;
//
//	checksum = s1d135xx_read_reg(p, S1D135XX_REG_SEQ_AUTOBOOT_CMD);
//
//	if (!(checksum & (uint16_t)S1D135XX_INIT_CODE_CHECKSUM_OK)) {
//		LOG("Init code checksum error");
//		return -1;
//	}
//
//	return 0;
//}

int s1d135xx_load_init_code_withoutSD(struct s1d13541 *p)
{
	uint16_t checksum;

	if (s1d135xx_wait_idle(p))
		return -1;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_INIT_SET);
	ecode_transfer_data();
	set_cs(p, 1);

	if (s1d135xx_wait_idle(p))
		return -1;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_INIT_STBY);
	send_param(0x0500);
	set_cs(p, 1);
	nrf_delay_ms(100);

	if (s1d135xx_wait_idle(p))
		return -1;

	checksum = s1d135xx_read_reg(p, S1D135XX_REG_SEQ_AUTOBOOT_CMD);
	printf("checksum: %x\tchecksum_ok:%x\n", checksum, (uint16_t)S1D135XX_INIT_CODE_CHECKSUM_OK);

	if (!(checksum & (uint16_t)S1D135XX_INIT_CODE_CHECKSUM_OK)) {
		printf("Init code checksum error\n");
		return -1;
	}

	return 0;
}

int s1d135xx_load_wflib(struct s1d13541 *p, struct pl_wflib *wflib,
			uint32_t addr)
{
	uint16_t params[4];
	uint32_t size2 = wflib->size / 2;
        printf("load_wflib: %04x\n", size2);

	if (s1d135xx_wait_idle(p))
		return -1;
        printf("load_wflib1\n");
	params[0] = addr & 0xFFFF;
	params[1] = (addr >> 16) & 0xFFFF;
	params[2] = size2 & 0xFFFF;
	params[3] = (size2 >> 16) & 0xFFFF;
	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_BST_WR_SDR);
	send_params(params, sizeof(params));
	set_cs(p, 1);

        printf("load_wflib2\n");

	if (wflib->xfer(wflib, wflib_wr, p))
		return -1;

        printf("load_wflib3\n");

	if (s1d135xx_wait_idle(p))
		return -1;
        printf("load_wflib4\n");

	send_cmd_cs(p, S1D135XX_CMD_BST_END_SDR);

        printf("load_wflib5\n");

	return s1d135xx_wait_idle(p);
}

int s1d135xx_init_gate_drv(struct s1d13541 *p)
{
	send_cmd_cs(p, S1D135XX_CMD_EPD_GDRV_CLR);

	return s1d135xx_wait_idle(p);
}

int s1d135xx_wait_dspe_trig(struct s1d13541 *p)
{
	send_cmd_cs(p, S1D135XX_CMD_WAIT_DSPE_TRG);

	return s1d135xx_wait_idle(p);
}

int s1d135xx_clear_init(struct s1d13541 *p)
{
	send_cmd_cs(p, S1D135XX_CMD_UPD_INIT);

	if (s1d135xx_wait_idle(p))
		return -1;

	return s1d135xx_wait_dspe_trig(p);
}

int s1d135xx_fill(struct s1d13541 *p, uint16_t mode, unsigned bpp,
		  const struct pl_area *a, uint8_t grey)
{
	struct pl_area full_area;
	const struct pl_area *fill_area;

	set_cs(p, 0);

	if (a != NULL) {
		send_cmd_area(p, S1D135XX_CMD_LD_IMG_AREA, mode, a);
		fill_area = a;
	} else {
		send_cmd(p, S1D135XX_CMD_LD_IMG);
		send_param(mode);
		full_area.top = 0;
		full_area.left = 0;
		full_area.width = p->xres;
		full_area.height = p->yres;
		fill_area = &full_area;
	}

	set_cs(p, 1);

	return do_fill(p, fill_area, bpp, grey);
}

// filling pixel by pixel with sending (x,y) coordinates as parameters - 5.16.2018, SJE
// less time efficient... don't need to send (x,y) positions all the times
int s1d135xx_fill_pixel_xy(struct s1d13541 *p, uint8_t grey, uint16_t pixel_x, uint16_t pixel_y)
{
	struct pl_area full_area;
	const struct pl_area *fill_area;

	set_cs(p, 0);

	//send_cmd(p, S1D135XX_CMD_LD_IMG);
	//send_param(mode);

	full_area.top = pixel_y;
	full_area.left = pixel_x;
	full_area.width = 1;
	full_area.height = 1;
	fill_area = &full_area;

	send_cmd_area(p, S1D135XX_CMD_LD_IMG_AREA, S1D13541_LD_IMG_8BPP, fill_area);

	set_cs(p, 1);

	return do_fill(p, fill_area, 8, grey);
}

// This function writes one pixel to the display at a specified location -- RMV 1/26/19
// filling pixel by pixel - 9.3.2018, SJE
// increment (x,y) positions locally inside the function
// packet number indicates the location of pixels
// more time efficient - don't need to send extra parameters (compare to s1d135xx_fill above)
int s1d135xx_fill_pixel(struct s1d13541 *p, uint32_t location, uint8_t grey)
{
	struct pl_area full_area;
	const struct pl_area *fill_area;

	if(grey != PL_WHITE){	// only update colors other than white - saves updating time
		set_cs(p, 0);

		full_area.top = location / 400;       // indicates y coordinate - factors of 400 (one line)
		full_area.left = location % 400;      // indicates x coordinate - remains of 400 (one line)
		full_area.width = 1;
		full_area.height = 1;
		fill_area = &full_area;

		send_cmd_area(p, S1D135XX_CMD_LD_IMG_AREA, S1D13541_LD_IMG_8BPP, fill_area);
		//printf("x: %d, y: %d \n", full_area.left, full_area.top);		// debugging

		set_cs(p, 1);
	}

//	if(grey == PL_WHITE){	// if white color, no need to update
//		return 0;
//	}else{					// other than white color, update the pixels
//		return do_fill_pixels(p, grey);
//	}
        return do_fill_pixels(p, grey);
}

int s1d135xx_horiz_line_pattern(struct s1d13541 *p, uint16_t height, uint16_t width, uint16_t line, uint16_t mode)
{
	uint16_t i = 0, j = 0, k = 0;
	uint16_t val = 0;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_LD_IMG);
	send_param(mode);
	set_cs(p, 1);

	if (s1d135xx_wait_idle(p))
		return -1;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_WRITE_REG);
	send_param(S1D135XX_REG_HOST_MEM_PORT);

	for (i = 0; i < height; i += 1) {
		if(i % line == 0)
		{
			k = 1 - k;
		}
		for (j = 0; j < width; j += 1) {
			if(k == 1)
			{
				val = 0xFFFF;
			}
			else
			{
				val = 0x0;
			}
			send_param(val);
		}
	}

	set_cs(p, 1);

	if (s1d135xx_wait_idle(p))
		return -1;

	send_cmd_cs(p, S1D135XX_CMD_LD_IMG_END);

	return 0;
}

int s1d135xx_horiz_line_180_pattern(struct s1d13541 *p, uint16_t height, uint16_t width, uint16_t line, uint16_t mode)
{
	uint16_t i = 0, j = 0, k = 0;
	uint16_t val = 0;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_LD_IMG);
	send_param(mode);
	set_cs(p, 1);

	if (s1d135xx_wait_idle(p))
		return -1;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_WRITE_REG);
	send_param(S1D135XX_REG_HOST_MEM_PORT);

	for (i = height; i > 0; i -= 1) {
		if(i % line == 0)
		{
			k = 1 - k;
		}
		for (j = width; j > 0; j -= 1) {
			if(k == 1)
			{
				val = 0xFFFF;
			}
			else
			{
				val = 0x0;
			}
			send_param(val);
		}
	}

	set_cs(p, 1);

	if (s1d135xx_wait_idle(p))
		return -1;

	send_cmd_cs(p, S1D135XX_CMD_LD_IMG_END);

	return 0;
}

int s1d135xx_vert_line_pattern(struct s1d13541 *p, uint16_t height, uint16_t width, uint16_t line, uint16_t mode)
{
	uint16_t i = 0, j = 0, k = 0;
	uint16_t val = 0;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_LD_IMG);
	send_param(mode);
	set_cs(p, 1);

	if (s1d135xx_wait_idle(p))
		return -1;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_WRITE_REG);
	send_param(S1D135XX_REG_HOST_MEM_PORT);

	for (i = 0; i < height; i += 1) {
		for (j = 0; j < width; j += 1) {
			if(j % line == 0){
				k = 1 - k;
			}
			if(k == 1){
				val = 0xFFFF;
			}
			else{
				val = 0x0;
			}
			send_param(val);
		}
	}

	set_cs(p, 1);

	if (s1d135xx_wait_idle(p))
		return -1;

	send_cmd_cs(p, S1D135XX_CMD_LD_IMG_END);

	return 0;
}

int s1d135xx_pattern_check(struct s1d13541 *p, uint16_t height, uint16_t width, uint16_t checker_size, uint16_t mode)
{
	uint16_t i = 0, j = 0, k = 0;
	uint16_t val = 0;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_LD_IMG);
	send_param(mode);
	set_cs(p, 1);

	if (s1d135xx_wait_idle(p))
		return -1;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_WRITE_REG);
	send_param(S1D135XX_REG_HOST_MEM_PORT);

	for (i = 0; i < height; i++) {
		k = i / checker_size;
		for (j = 0; j < width; j += 2) {
			val = (k + (j / checker_size)) % 2 ? 0xFFFF : 0x0;
			send_param(val);
		}
	}

	set_cs(p, 1);

	if (s1d135xx_wait_idle(p))
		return -1;

	send_cmd_cs(p, S1D135XX_CMD_LD_IMG_END);

	return 0;
}

int s1d135xx_load_app_image(struct s1d13541 *p, uint16_t mode,
			unsigned bpp, const struct pl_area *area, uint8_t data[])
{
	int stat;

	set_cs(p, 0);

        send_cmd_area(p, S1D135XX_CMD_LD_IMG_AREA, mode, area);
        //send_cmd(p, S1D135XX_CMD_LD_IMG);
        send_param(mode);

	set_cs(p, 1);

	if (s1d135xx_wait_idle(p))
		return -1;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_WRITE_REG);
	send_param(S1D135XX_REG_HOST_MEM_PORT);

        transfer_data(&data[0], 4800);
        stat = 0;

	set_cs(p, 1);

	if (stat)
		return -1;

	if (s1d135xx_wait_idle(p))
		return -1;

	send_cmd_cs(p, S1D135XX_CMD_LD_IMG_END);

	//printf("Image uploaded\n");	// for debugging - 10.17.17

	return s1d135xx_wait_idle(p);
}

//int s1d135xx_load_image(struct s1d135xx *p, const char *path, uint16_t mode,
//			unsigned bpp, const struct pl_area *area, int left,
//			int top)
//{
//	struct pnm_header hdr;
//	FIL img_file;
//	int stat;
//
//	if (f_open(&img_file, path, FA_READ) != FR_OK)
//		return -1;
//
//	if (pnm_read_header(&img_file, &hdr))
//		return -1;
//
//	set_cs(p, 0);
//
//	if (area != NULL) {
//		send_cmd_area(p, S1D135XX_CMD_LD_IMG_AREA, mode, area);
//	} else {
//		send_cmd(p, S1D135XX_CMD_LD_IMG);
//		send_param(mode);
//	}
//
//	set_cs(p, 1);
//
//	if (s1d135xx_wait_idle(p))
//		return -1;
//
//	set_cs(p, 0);
//	send_cmd(p, S1D135XX_CMD_WRITE_REG);
//	send_param(S1D135XX_REG_HOST_MEM_PORT);
//
//	if (area == NULL)
//		stat = transfer_file(&img_file);
//	else
//		stat = transfer_image(&img_file, area, left, top, hdr.width);
//
//	set_cs(p, 1);
//	f_close(&img_file);
//
//	if (stat)
//		return -1;
//
//	if (s1d135xx_wait_idle(p))
//		return -1;
//
//	send_cmd_cs(p, S1D135XX_CMD_LD_IMG_END);
//
//	LOG("Image uploaded");	// for debugging - 10.17.17
//
//	return s1d135xx_wait_idle(p);
//}

// added function for rotating 180 degrees for right display - 10.12.17
// not finished yet - alternate choice for rotating image, just upload rotated image - 10.17.17
//int s1d135xx_load_180_image(struct s1d135xx *p, const char *path, uint16_t mode,
//			unsigned bpp, const struct pl_area *area, int left,
//			int top)
//{
//	struct pnm_header hdr;
//	FIL img_file;
//	int stat;
//
//	if (f_open(&img_file, path, FA_READ) != FR_OK)
//		return -1;
//
//	if (pnm_read_header(&img_file, &hdr))
//		return -1;
//
//	set_cs(p, 0);
//
//	if (area != NULL) {
//		send_cmd_area(p, S1D135XX_CMD_LD_IMG_AREA, mode, area);
//	} else {
//		send_cmd(p, S1D135XX_CMD_LD_IMG);
//		send_param(mode);
//	}
//
//	set_cs(p, 1);
//
//	if (s1d135xx_wait_idle(p))
//		return -1;
//
//	set_cs(p, 0);
//	send_cmd(p, S1D135XX_CMD_WRITE_REG);
//	send_param(S1D135XX_REG_HOST_MEM_PORT);
//
//	if (area == NULL)
//		stat = transfer_file(&img_file);
//	else
//		stat = transfer_image(&img_file, area, left, top, hdr.width);
//
//	set_cs(p, 1);
//	f_close(&img_file);
//
//	if (stat)
//		return -1;
//
//	if (s1d135xx_wait_idle(p))
//		return -1;
//
//	send_cmd_cs(p, S1D135XX_CMD_LD_IMG_END);
//
//	return s1d135xx_wait_idle(p);
//}

int s1d135xx_update(struct s1d13541 *p, int wfid, const struct pl_area *area)
{
	if (area != NULL)
		printf("update area %d (%d, %d) %dx%d\n", wfid,
		    area->left, area->top, area->width, area->height);
	else
		printf("update %d\n", wfid);

	set_cs(p, 0);

	/* wfid = S1D135XX_WF_MODE(wfid); */

	if (area != NULL) {
		send_cmd_area(p, S1D135XX_CMD_UPDATE_FULL_AREA,
			      S1D135XX_WF_MODE(wfid), area);
	} else {
		send_cmd(p, S1D135XX_CMD_UPDATE_FULL);
		send_param(S1D135XX_WF_MODE(wfid));
	}

	set_cs(p, 1);

	if (s1d135xx_wait_idle(p))
		return -1;

	return s1d135xx_wait_dspe_trig(p);
}

int s1d135xx_wait_update_end(struct s1d13541 *p)
{
	send_cmd_cs(p, S1D135XX_CMD_WAIT_DSPE_FREND);

	return s1d135xx_wait_idle(p);
}

int s1d135xx_wait_idle(struct s1d13541 *p)
{
	unsigned long timeout = 1000000;
        
	while (!get_hrdy(p) && --timeout);;
	if (!timeout) {
		return -1;
	}
	return 0;
}

int s1d135xx_set_power_state(struct s1d13541 *p,
			     enum pl_epdc_power_state state)
{
	const struct s1d13541_data *data = p->data;
	int stat;
	set_cs(p, 1);
	set_hdc(p, 1);
	nrf_gpio_pin_write(data->vcc_en, 0);
	nrf_gpio_pin_write(data->clk_en, 1);
	if (s1d135xx_wait_idle(p))
		return -1;
	switch (state) {
	case PL_EPDC_RUN:
		send_cmd_cs(p, S1D135XX_CMD_RUN);
		stat = s1d135xx_wait_idle(p);
		break;

	case PL_EPDC_STANDBY:
		send_cmd_cs(p, S1D135XX_CMD_STBY);
		stat = s1d135xx_wait_idle(p);
		break;

	case PL_EPDC_SLEEP:
		send_cmd_cs(p, S1D135XX_CMD_STBY);
		stat = s1d135xx_wait_idle(p);
		nrf_gpio_pin_write(data->clk_en, 0);
		break;

	case PL_EPDC_OFF:
		send_cmd_cs(p, S1D135XX_CMD_SLEEP);
		stat = s1d135xx_wait_idle(p);
		nrf_gpio_pin_write(data->clk_en, 0);
		nrf_gpio_pin_write(data->vcc_en, 1);
		set_hdc(p, 0);
		set_cs(p, 0);
		break;
	}

	return stat;
}

int s1d135xx_set_epd_power(struct s1d13541 *p, int on)
{
	uint16_t arg = on ? S1D135XX_PWR_CTRL_UP : S1D135XX_PWR_CTRL_DOWN;
	uint16_t tmp;

//#if VERBOSE
//	LOG("EPD power o%s", on ? "n" : "ff");
//#endif

	if (s1d135xx_wait_idle(p))
		return -1;

	s1d135xx_write_reg(p, S1D135XX_REG_PWR_CTRL, arg);

	do {
		tmp = s1d135xx_read_reg(p, S1D135XX_REG_PWR_CTRL);
	} while (tmp & S1D135XX_PWR_CTRL_BUSY);

	if (on && ((tmp & S1D135XX_PWR_CTRL_CHECK_ON) !=
		   S1D135XX_PWR_CTRL_CHECK_ON)) {
		printf("Failed to turn the EPDC power on\n");
		return -1;
	}

	return 0;
}

void s1d135xx_cmd(struct s1d13541 *p, uint16_t cmd, const uint16_t *params,
		  size_t n)
{
	set_cs(p, 0);
	send_cmd(p, cmd);
	send_params(params, n);
	set_cs(p, 1);
}

uint16_t s1d135xx_read_reg(struct s1d13541 *p, uint16_t reg)
{
	uint16_t val;
	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_READ_REG);
	send_param(reg);
	spi_read_bytes((uint8_t *)&val, sizeof(uint16_t));
	spi_read_bytes((uint8_t *)&val, sizeof(uint16_t));
	set_cs(p, 1);
        swap16(&val);
        return val;
}

void s1d135xx_write_reg(struct s1d13541 *p, uint16_t reg, uint16_t val)
{
	const uint16_t params[] = { reg, val };

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_WRITE_REG);
	send_params(params, ARRAY_SIZE(params));
	set_cs(p, 1);
}


//int s1d135xx_load_register_overrides(struct s1d135xx *p)
//{
//	static const char override_path[] = "bin/override.txt";
//	static const char sep[] = ", ";
//	FIL file;
//	FRESULT res;
//	int stat;
//	uint16_t reg, val;
//
//	res = f_open(&file, override_path, FA_READ);
//	if (res != FR_OK) {
//		if (res == FR_NO_FILE) {
//			return 0;
//		}
//		else {
//			LOG("Failed to open register override file");
//			return -1;
//		}
//	}
//
//	stat = 0;
//	while (!stat) {
//		char line[81];
//		int len;
//		stat = parser_read_file_line(&file, line, sizeof(line));
//		if (stat < 0) {
//			LOG("Failed to read line");
//			break;
//		}
//		else if (!stat) {
//			/* End of file */
//			break;
//		}
//
//		if ((line[0] == '\0') || (line[0] == '#')) {
//			stat = 0;
//			continue;
//		}
//
//		/* Assume failure */
//		stat = -1;
//
//		len = parser_read_word(line, sep, &reg);
//		if (len <= 0)
//			break;
//
//		len = parser_read_word(line + len, sep, &val);
//		if (len <= 0)
//			break;
//
//		s1d135xx_write_reg(p, reg, val);
//		if (val == s1d135xx_read_reg(p, reg)) {
//			stat = 0;	/* success */
//		}
//	}
//
//	f_close(&file);
//
//	return stat;
//}

/* ----------------------------------------------------------------------------
 * private functions
 */

static int get_hrdy(struct s1d13541 *p)
{
	uint16_t status;

	if (p->data->hrdy != PL_GPIO_NONE)
		return nrf_gpio_pin_read(p->data->hrdy);  //change from nrf_gpio_port_in_read(p->data->hrdy)
       // printf("E135:1056:GHRDY\n"); 
	status = s1d135xx_read_reg(p, S1D135XX_REG_SYSTEM_STATUS);

	return ((status & p->hrdy_mask) == p->hrdy_result);
}

static int do_fill(struct s1d13541 *p, const struct pl_area *area,
		   unsigned bpp, uint8_t g)
{
	uint16_t val16;
	uint16_t lines;
	uint16_t pixels;

	/* Only 16-bit transfers for now... */
//	assert(!(area->width % 2));

	switch (bpp) {
	case 1:
	case 2:
		printf("Unsupported bpp\n");
		return -1;
	case 4:
		val16 = g & 0xF0;
		val16 |= val16 >> 4;
		val16 |= val16 << 8;
		pixels = area->width / 4;
		break;
	case 8:
		val16 = g | (g << 8);
		pixels = area->width / 2;
		break;
	default:
		//assert_fail("Invalid bpp");
                printf("Invalid bpp\n");
                break;
	}

	val16 = g | (g << 8);
	pixels = area->width / 2;

	lines = area->height;

	if (s1d135xx_wait_idle(p))
		return -1;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_WRITE_REG);
	send_param(S1D135XX_REG_HOST_MEM_PORT);

	while (lines--) {
		uint16_t x = pixels;

		while (x--)
			send_param(val16);
	}

	set_cs(p, 1);

	if (s1d135xx_wait_idle(p))
		return -1;

	send_cmd_cs(p, S1D135XX_CMD_LD_IMG_END);

	return s1d135xx_wait_idle(p);
}

static int do_fill_pixels(struct s1d13541 *p, uint8_t g)
{
	uint16_t val16;
	uint16_t lines;

	val16 = g | (g << 8);
	lines = 2;

	if (s1d135xx_wait_idle(p))
		return -1;

       // printf("Do Fill Pixels 1\n");
	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_WRITE_REG);
	send_param(S1D135XX_REG_HOST_MEM_PORT);

	while (lines--) {
		send_param(val16);
	}

	set_cs(p, 1);

       // printf("Do Fill Pixels 2\n");

	if (s1d135xx_wait_idle(p))
		return -1;
        
       // printf("Do Fill Pixels 3\n");
	send_cmd_cs(p, S1D135XX_CMD_LD_IMG_END);
       // printf("Do Fill Pixels 4\n");

	return s1d135xx_wait_idle(p);
}

static int wflib_wr(void *ctx, const uint8_t *data, size_t n)
{
	struct s1d13541 *p = ctx;

	set_cs(p, 0);
	send_cmd(p, S1D135XX_CMD_WRITE_REG);
	send_param(S1D135XX_REG_HOST_MEM_PORT);
	transfer_data(data, n);
	set_cs(p, 1);

	return 0;
}

//static int transfer_file(FIL *file)
//{
//	uint8_t data[DATA_BUFFER_LENGTH];
//
//	for (;;) {
//		size_t count;
//
//		if (f_read(file, data, sizeof(data), &count) != FR_OK)
//			return -1;
//
//		if (!count)
//			break;
//
//		transfer_data(data, count);
//	}
//
//	return 0;
//}
//
// area represents the size of the image patch to be transferred
// 
//static int transfer_image(FIL *f, const struct pl_area *area, int left,
//			  int top, int width)
//{
//	uint8_t data[DATA_BUFFER_LENGTH];
//	size_t line;
//
//	/* Simple bounds check */
//	if (width < area->width || width < (left + area->width)) {
//		LOG("Invalid combination of width/left/area");
//		return -1;
//	}
//
//	if (f_lseek(f, f->fptr + ((long)top * (unsigned long)width)) != FR_OK)
//		return -1;
//
//	for (line = area->height; line; --line) {
//		size_t count;
//		size_t remaining = area->width;
//
//		/* Find the first relevant pixel (byte) on this line */
//		if (f_lseek(f, f->fptr + (unsigned long)left) != FR_OK)
//			return -1;
//
//		/* Transfer data of interest in chunks */
//		while (remaining) {
//			size_t btr = (remaining <= DATA_BUFFER_LENGTH) ?
//					remaining : DATA_BUFFER_LENGTH;
//
//			if (f_read(f, data, btr, &count) != FR_OK)
//				return -1;
//
//			transfer_data(&data[0], btr);
//
//			remaining -= btr;
//		}
//
//		/* Move file pointer to end of line */
//		if (f_lseek(f, f->fptr + (width - (left + area->width))) != FR_OK)
//			return -1;
//	}
//
//	return 0;
//}

static void transfer_data(const uint8_t *data, size_t n)
{
	const uint16_t *data16 = (const uint16_t *)data;

	n /= 2;

	while (n--)
		send_param(*data16++);
}

static void ecode_transfer_data(void)
{
	uint32_t size = sizeof(ecode);  //changed from uint16_t
	uint32_t i = 0;                 //changed from uint16_t

	while (size--){
		send_param(ecode[i]);
		i++;

//		LOG("size: %d \t i: %d", size, i);
	}
}

static void send_cmd_area(struct s1d13541 *p, uint16_t cmd, uint16_t mode,
			  const struct pl_area *area)
{
	const uint16_t args[] = {
		mode,
		(area->left & S1D135XX_XMASK),
		(area->top & S1D135XX_YMASK),
		(area->width & S1D135XX_XMASK),
		(area->height & S1D135XX_YMASK),
	};

	send_cmd(p, cmd);
	send_params(args, ARRAY_SIZE(args));
}

static void send_cmd_cs(struct s1d13541 *p, uint16_t cmd)
{
	set_cs(p, 0);
	send_cmd(p, cmd);
	set_cs(p, 1);
}

static void send_cmd(struct s1d13541 *p, uint16_t cmd)
{
        swap16(&cmd);
	set_hdc(p, 0);
        // printf("E135:1283:SW\n");
	spi_write_bytes((uint8_t *)&cmd, sizeof(uint16_t));
         //printf("E135:1283: Set HDC\n");
	set_hdc(p, 1);
}

static void send_params(const uint16_t *params, size_t n)
{
	size_t i;

	for (i = 0; i < n; ++i)
		send_param(params[i]);
}

static void send_param(uint16_t param)
{
	swap16(&param);
	spi_write_bytes((uint8_t *)&param, sizeof(uint16_t));
        //nrf_delay_us(10);
}

static void set_cs(struct s1d13541 *p, int state)
{
      if(state == 1)
            nrf_gpio_pin_set(5);
      //    nrf_gpio_pin_set(p->data->cs0);
      else if(state == 0)
            nrf_gpio_pin_clear(5);
      //    nrf_gpio_pin_clear(p->data->cs0);
}

static void set_hdc(struct s1d13541 *p, int state)
{
	const unsigned hdc = p->data->hdc;

      if (hdc != PL_GPIO_NONE){
          if(state == 1)
              nrf_gpio_pin_set(7);
          else if(state == 0)
              nrf_gpio_pin_clear(7);
      }
}