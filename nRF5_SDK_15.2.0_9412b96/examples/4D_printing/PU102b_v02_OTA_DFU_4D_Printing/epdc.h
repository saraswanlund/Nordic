#ifndef INCLUDE_PL_EPDC_H
#define INCLUDE_PL_EPDC_H 1

#include <stdint.h>
#include "wflib.h"

/* Set to 1 to enable stub EPDC implementation */
#define PL_EPDC_STUB 0

/* Use this macro to convert a 16-greyscale value to 8 bits */
#define PL_GL16(_g) ({			\
	uint8_t g16 = (_g) & 0xF;	\
	g16 | g16 << 4;			\
})

#define PL_WHITE PL_GL16(15)
#define PL_14 PL_GL16(14)
#define PL_13 PL_GL16(13)
#define PL_12 PL_GL16(12)
#define PL_11 PL_GL16(11)
#define PL_10 PL_GL16(10)
#define PL_9 PL_GL16(9)
#define PL_8 PL_GL16(8)
#define PL_7 PL_GL16(7)
#define PL_6 PL_GL16(6)
#define PL_5 PL_GL16(5)
#define PL_4 PL_GL16(4)
#define PL_3 PL_GL16(3)
#define PL_2 PL_GL16(2)
#define PL_1 PL_GL16(1)
#define PL_BLACK PL_GL16(0)

enum pl_epdc_power_state {
	PL_EPDC_RUN = 0,
	PL_EPDC_STANDBY,
	PL_EPDC_SLEEP,
	PL_EPDC_OFF,
};

enum pl_epdc_temp_mode {
	PL_EPDC_TEMP_MANUAL,
	PL_EPDC_TEMP_EXTERNAL,
	PL_EPDC_TEMP_INTERNAL,
};

struct pl_area;
struct pl_dispinfo;
struct pl_epdpsu;

struct pl_wfid {
	const char *path;
	int id;
};

struct pl_epdc{
	int (*clear_init)(struct pl_epdc *p);
	int (*load_wflib)(struct pl_epdc *p);
	int (*update)(struct pl_epdc *p, int wfid, const struct pl_area *area);
	int (*wait_update_end)(struct pl_epdc *p);
	int (*set_power)(struct pl_epdc *p, enum pl_epdc_power_state state);
	int (*set_temp_mode)(struct pl_epdc *p, enum pl_epdc_temp_mode mode);
	int (*update_temp)(struct pl_epdc *p);
	int (*fill)(struct pl_epdc *p, const struct pl_area *area, uint8_t g);
	int (*fill_pixel_xy)(struct pl_epdc *p, uint8_t g, uint16_t pixel_x, uint16_t pixel_y);
	int (*fill_pixel)(struct pl_epdc *p, unsigned int image_location, uint8_t g);
	int (*pixel)(struct pl_epdc *p, uint16_t pixel);
	int (*horiz_line_pattern)(struct pl_epdc *p, uint16_t line);
	int (*horiz_line_180_pattern)(struct pl_epdc *p, uint16_t line);
	int (*vert_line_pattern)(struct pl_epdc *p, uint16_t line);
	int (*pattern_check)(struct pl_epdc *p, uint16_t size);
	int (*load_app_image)(struct pl_epdc *p, 
			  const struct pl_area *area, uint8_t data[]);
	int (*load_image)(struct pl_epdc *p, const char *path,
			  const struct pl_area *area, int left, int top);
	int (*load_180_image)(struct pl_epdc *p, const char *path,
			  const struct pl_area *area, int left, int top);
	int (*set_epd_power)(struct pl_epdc *p, int on);

	const struct pl_wfid *wf_table;
	const struct pl_dispinfo *dispinfo;
	struct pl_wflib wflib;
	enum pl_epdc_power_state power_state;
	enum pl_epdc_temp_mode temp_mode;
	int manual_temp;
	unsigned xres;
	unsigned yres;
	void *data;
};

/* --- Waveform management --- */

/** Waveform string elements */
#define WF_INIT "init"
#define WF_REFRESH "refresh"
#define WF_DELTA "delta"
#define WF_MONO "mono"

/** Optimised look-up path strings - use for improved performance */
extern const char wf_init[];          /**< init */
extern const char wf_refresh[];       /**< refresh */
extern const char wf_delta[];         /**< delta */
extern const char wf_refresh_mono[];  /**< refresh/mono */
extern const char wf_delta_mono[];    /**< delta/mono */

/** Get a waveform identifier or -1 if not found */
extern int pl_epdc_get_wfid(struct pl_epdc *p, const char *wf_path);

/** Perform a typical single image update:
 * # Update temperature
 * # Turn the EPD PSU on
 * # Generate an update with the given waveform and area
 * # Wait for the update to end
 * # Turn the EPD PSU off
 */
extern int pl_epdc_single_update(struct pl_epdc *epdc, struct pl_epdpsu *psu,
				 int wfid, const struct pl_area *area);

#endif /* INCLUDE_PL_EPDC_H */
