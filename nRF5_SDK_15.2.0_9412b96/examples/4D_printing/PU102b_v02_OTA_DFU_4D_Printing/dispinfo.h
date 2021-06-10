#ifndef INCLUDE_PL_DISPINFO_H
#define INCLUDE_PL_DISPINFO_H 1

#include <stdint.h>

#define CONFIG_DISPLAY_TYPE         "Type18"

#define PL_DISPINFO_MAGIC           0x46574C50UL
#define PL_DISPINFO_VERSION         1

#define PL_DISPINFO_STR_LEN         63
#define PL_DISPINFO_STR_SIZE (PL_DISPINFO_STR_LEN + 1)

#define PL_DISPINFO_PANEL_ID        "00084130904600"
#define PL_DISPINFO_WAVEFORM_ID     "EPS_V6C221_16GL_2PHASE_V2.bin"
#define PL_DISPINFO_WAVEFORM_TARGET "S1D13541"

struct __attribute__((__packed__)) pl_dispinfo_vermagic {
	uint32_t magic;
	uint16_t version;
};

struct __attribute__((__packed__)) pl_dispinfo_info {
	char panel_id[PL_DISPINFO_STR_SIZE];
	char panel_type[PL_DISPINFO_STR_SIZE];
	int32_t vcom;
	uint8_t waveform_md5[16];
	uint32_t waveform_full_length;
	uint32_t waveform_lzss_length;
	char waveform_id[PL_DISPINFO_STR_SIZE];
	char waveform_target[PL_DISPINFO_STR_SIZE];
};

struct __attribute__((__packed__)) pl_dispinfo {
	struct pl_dispinfo_vermagic vermagic;
	struct pl_dispinfo_info info;
	uint16_t info_crc;
};

// Pull the display data of Type 18 - SJE, 7.13.2018
extern void type18_pl_dispinfo(struct pl_dispinfo *p);
extern void pl_dispinfo_log(const struct pl_dispinfo *p);

#endif /* INCLUDE_PL_DISPINFO_H */