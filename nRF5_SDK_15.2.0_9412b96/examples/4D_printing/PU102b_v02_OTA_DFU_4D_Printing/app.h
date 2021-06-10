#ifndef INCLUDE_APP_H
#define INCLUDE_APP_H 1

#include <stdint.h>

struct pl_platform;

extern int app_stop;
extern int read_index;
extern int pattern_size;
extern int area_size;
extern int line;
extern int screen;
extern int display1;
extern int update_count;

extern int app_demo(struct pl_platform *plat, uint16_t update, uint16_t display);
extern int app_clear(struct pl_platform *plat);
//extern int app_update_pixels(struct pl_platform *plat);
extern int app_update_display(struct pl_platform *plat);
extern int app_update_display_EEPROM(struct pl_platform *plat, struct i2c_eeprom *eeprom, uint8_t pic_num);

extern int convert_binary_graylevel(unsigned char RX_data, unsigned int i);
extern int app_manual_pixels(struct pl_platform *plat);

extern int app_fill_pixel(struct pl_platform *plat, unsigned int image_location, unsigned char image_data);
extern int app_load_image(struct pl_platform *plat, const struct pl_area *area, uint8_t data[]);

extern int app_power(struct pl_platform *plat, const char *path);
extern int app_slideshow(struct pl_platform *plat, const char *path);
extern int display_image(struct pl_platform *plat, const char *path, const char *file_name);	// function for displaying one image at a time for multiple displays 5.14.2017
extern int app_sequencer(struct pl_platform *plat, const char *path);
extern int app_pattern_horiz_line(struct pl_platform *plat);							// function for displaying some random patterns - 10.9.17
extern int app_pattern_vert_line(struct pl_platform *plat);
extern int app_pattern_check(struct pl_platform *plat);
extern int app_right_display(struct pl_platform *plat, const char *dir, const char *file);	// function for changing right display based on configuration image - 10.9.17
extern int app_right_display_video(struct pl_platform *plat, const char *dir, const char *file);
extern int app_left_display_image(struct pl_platform *plat, const char *dir, const char *file);	// function for changing left display based on configuration image - 10.9.17
extern int app_left_display_time(struct pl_platform *plat, const char *dir, const char *file);
#endif /* INCLUDE_APP_H */