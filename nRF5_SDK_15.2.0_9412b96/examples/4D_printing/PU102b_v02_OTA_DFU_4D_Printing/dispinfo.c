#include <string.h>
#include "dispinfo.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

void type18_pl_dispinfo(struct pl_dispinfo *p)
{
	p->vermagic.version = PL_DISPINFO_VERSION;
	p->vermagic.magic = PL_DISPINFO_MAGIC;
	p->info_crc = 0xD5C2;
	strncpy(p->info.panel_id, PL_DISPINFO_PANEL_ID, sizeof p->info.panel_id);
	strncpy(p->info.panel_type, CONFIG_DISPLAY_TYPE, sizeof p->info.panel_type);
	p->info.vcom = 3648;
	p->info.waveform_full_length = 45516;
	p->info.waveform_lzss_length = 9070;
	strncpy(p->info.waveform_id, PL_DISPINFO_WAVEFORM_ID, sizeof p->info.waveform_id);
	strncpy(p->info.waveform_target, PL_DISPINFO_WAVEFORM_TARGET, sizeof p->info.waveform_target);

	p->info.waveform_md5[0] = 0x46;
	p->info.waveform_md5[1] = 0x26;
	p->info.waveform_md5[2] = 0xe9;
	p->info.waveform_md5[3] = 0x11;
	p->info.waveform_md5[4] = 0x1e;
	p->info.waveform_md5[5] = 0x50;
	p->info.waveform_md5[6] = 0x16;
	p->info.waveform_md5[7] = 0x17;
	p->info.waveform_md5[8] = 0x2a;
	p->info.waveform_md5[9] = 0xdc;
	p->info.waveform_md5[10] = 0x3c;
	p->info.waveform_md5[11] = 0x53;
	p->info.waveform_md5[12] = 0xcd;
	p->info.waveform_md5[13] = 0x14;
	p->info.waveform_md5[14] = 0x47;
	p->info.waveform_md5[15] = 0x12;
}

void pl_dispinfo_log(const struct pl_dispinfo *p)
{
	const char *magic = (const char *)&p->vermagic.magic;

	printf("Version: %d\n", p->vermagic.version);
	printf("Magic: 0x%lX %c%c%c%c\n",
	    p->vermagic.magic, magic[0], magic[1], magic[2], magic[3]);
	printf("Info CRC: 0x%04X\n", p->info_crc);
	printf("Panel ID: %s\n", p->info.panel_id);

	printf("Panel Type: %s\n", p->info.panel_type);
	printf("VCOM: %li\n", p->info.vcom);

	printf("Waveform Length: %lu\n", p->info.waveform_full_length);
	printf("Waveform Compressed Length: %lu\n",p->info.waveform_lzss_length);
	printf("Waveform ID: %s\n", p->info.waveform_id);

	printf("Waveform Target: %s\n", p->info.waveform_target);

	printf("Waveform MD5: 0x");
	{
		int i;

		for (i = 0; i < sizeof(p->info.waveform_md5); i++)
			printf("%02x", p->info.waveform_md5[i]);
	}
	printf("\n");
}