#include <stdint.h>
#include "hwinfo.h"
#include "nrf_log.h"

#define VERBOSE 1

void pl_hwinfo_log(const struct pl_hwinfo *info)
{
#if VERBOSE
	const struct pl_hw_vcom_info *vcom = &info->vcom;
#endif
	const struct pl_hw_board_info *board = &info->board;

#if VERBOSE
	printf("Version: %d\n", info->version);
	printf("VCOM DAC info: dac[%d]=%d, dac[%d]=%d\n",
	    vcom->dac_x1, vcom->dac_y1, vcom->dac_x2, vcom->dac_y2);
	printf("Gate PSU info: VGPOS=%ld, VGNEG=%ld, swing=%ld\n",
	    vcom->vgpos_mv, vcom->vgneg_mv, vcom->swing_ideal);
#endif
	printf("Board type: %s, version: %d.%d\n",
	    board->board_type, board->board_ver_maj, board->board_ver_min);
#if VERBOSE
	printf("vcom_mode=%d, hv_pmic=%d, vcom_dac=%d, vcom_adc=%d\n",
	    board->vcom_mode, board->hv_pmic, board->vcom_dac, board->vcom_adc);
	printf("io_config=%d, i2c_mode=%d, temp_sensor=%d, frame_buffer=%d\n",
	    board->io_config, board->i2c_mode, board->temp_sensor,
	    board->frame_buffer);
	printf("epdc_ref=%d, adc_scale_1=%d, adc_scale_2=%d\n",
	    board->epdc_ref, board->adc_scale_1, board->adc_scale_2);
	printf("CRC16: %04X\n", info->crc);
#endif
}
