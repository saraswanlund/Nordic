#include "wflib.h"
#include "dispinfo.h"
#include "pl_endian.h"
//#include "crc16.h"          // combined with lzss.c
#include "lzss.h"
#include "eeprom-i2c.h"

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

//#define LOG_TAG "wflib"
//#include "utils.h"

/* ----------------------------------------------------------------------------
 * FatFS
 */

#define DATA_BUFFER_LENGTH 256

//static int pl_wflib_fatfs_xfer(struct pl_wflib *wflib, pl_wflib_wr_t wr,
//			       void *ctx)
//{
//	FIL *f = wflib->priv;
//	size_t left = wflib->size;
//
//	if (f_lseek(f, 0) != FR_OK)
//		return -1;
//
//	while (left) {
//		uint8_t data[DATA_BUFFER_LENGTH];
//		const size_t n = min(left, sizeof(data));
//		size_t count;
//
//		if ((f_read(f, data, n, &count) != FR_OK) || (count != n)) {
//			LOG("Failed to read from file");
//			return -1;
//		}
//
//		if (wr(ctx, data, n))
//			return -1;
//
//		left -= n;
//	}
//
//	return 0;
//}

//int pl_wflib_init_fatfs(struct pl_wflib *wflib, FIL *f, const char *path)
//{
//	if (f_open(f, path, FA_READ) != FR_OK) {
//		LOG("Failed to open wflib: %s", path);
//		return -1;
//	}
//
//	wflib->xfer = pl_wflib_fatfs_xfer;
//	wflib->size = f_size(f);
//	wflib->priv = f;
//
//	LOG("FatFS (%s)", path);
//
//	return 0;
//}

/* ----------------------------------------------------------------------------
 * EEPROM + LZSS
 */

/* LZSS parameters */
#define PLWF_LZSS_EI 7
#define PLWF_LZSS_EJ 4

#ifndef min
#define min(x,y)	( (x) < (y) ? (x) : (y) )
#endif

/* Context to read from the EEPROM */
struct lzss_rd_ctx {
	uint8_t buffer[64];        /* buffer to read a block of data */
	size_t buflen;             /* length of payload in buffer */
	size_t datalen;            /* length of the data left to read */
	size_t index;              /* index of current byte in buffer */
	size_t offset;             /* offset where to read next in EEPROM */
	uint16_t crc;              /* accumulated CRC for all data read */
	const struct i2c_eeprom *eeprom; /* EEPROM instance */
};

/* Context to write to the EPDC */
struct lzss_wr_ctx {
	uint8_t buffer[128];       /* buffer to read a block of data */
	size_t buflen;             /* length of payload in buffer */
	size_t index;              /* index of current byte in buffer */
	pl_wflib_wr_t wflib_wr;    /* function to write to the output */
	void *wflib_ctx;           /* write context for wflib */
};

static int pl_wflib_lzss_rd(struct lzss_rd_ctx *ctx)
{
	if (ctx->index == ctx->buflen) {
		if (!ctx->datalen)
			return EOF;

		ctx->index = 0;
		ctx->buflen = min(sizeof(ctx->buffer), ctx->datalen);

		if (eeprom_read(ctx->eeprom, ctx->offset, ctx->buflen,
				ctx->buffer)) {
			printf("Failed to read LZSS data from EEPROM\n");
			return LZSS_ERROR;
		}

		ctx->crc = crc16_run(ctx->crc, ctx->buffer, ctx->buflen);
		ctx->offset += ctx->buflen;
		ctx->datalen -= ctx->buflen;
	}

	return ctx->buffer[ctx->index++];
}

static int pl_wflib_lzss_wr(int c, struct lzss_wr_ctx *ctx)
{
	ctx->buffer[ctx->index++] = c;

	if (ctx->index == ctx->buflen) {
		if (ctx->wflib_wr(ctx->wflib_ctx, ctx->buffer, ctx->buflen)) {
			printf("Failed to write waveform data\n");
			return LZSS_ERROR;
		}

		ctx->index = 0;
	}

	return c;
}

static int pl_wflib_eeprom_xfer(struct pl_wflib *wflib, pl_wflib_wr_t wr,
				void *ctx)
{
	struct pl_wflib_eeprom_ctx *p = wflib->priv;
	struct lzss lzss;
	struct lzss_io io;
	struct lzss_rd_ctx rd_ctx;
	struct lzss_wr_ctx wr_ctx;
	char lzss_buffer[LZSS_BUFFER_SIZE(PLWF_LZSS_EI)];
	uint16_t crc;

        //printf("CRC: %04x %04x\n", rd_ctx.crc, crc);		// for debugging

	if (lzss_init(&lzss, PLWF_LZSS_EI, PLWF_LZSS_EJ)) {
		printf("Failed to initialise LZSS\n");
		return -1;
	}

	lzss.buffer = lzss_buffer;

	rd_ctx.buflen = 0;
	rd_ctx.datalen = p->dispinfo->info.waveform_lzss_length;
	rd_ctx.index = 0;
	rd_ctx.offset = sizeof(struct pl_dispinfo);
	rd_ctx.crc = crc16_init;
	rd_ctx.eeprom = p->eeprom;

	io.rd = (lzss_rd_t)pl_wflib_lzss_rd;
	io.i = &rd_ctx;

	wr_ctx.buflen = sizeof(wr_ctx.buffer);
	wr_ctx.index = 0;
	wr_ctx.wflib_wr = wr;
	wr_ctx.wflib_ctx = ctx;

	io.wr = (lzss_wr_t)pl_wflib_lzss_wr;
	io.o = &wr_ctx;

        printf("Initial CRC : %04x\n", rd_ctx.crc);		// for debugging

	if (lzss_decode(&lzss, &io)) {
		printf("Failed to decode LZSS data\n");
		return -1;
	}
        
        printf("READ CRC: %04x\n", rd_ctx.crc);		// for debugging
	
        if (wr(ctx, wr_ctx.buffer, wr_ctx.index)) {
		printf("Failed to flush output data\n");
		return -1;
	}

        printf("Write to EDPC CRC: %04x\n", rd_ctx.crc);		// for debugging

	if (eeprom_read(p->eeprom, rd_ctx.offset + rd_ctx.datalen,
			sizeof crc, (uint8_t *)&crc)) {
		printf("Failed to read CRC\n");
		return -1;
	}

        printf("READ CRC from EEPROM CRC: %04x \n",crc);		// for debugging
	
        swap16(&crc);
        
        printf("SWAPPED CRC: %04x\n", crc);		// for debugging

//        crc = 0xf709;
//        rd_ctx.crc = 0xf709;

	if (crc != rd_ctx.crc) {
		printf("CRC mismatch read wf, eeprom read: %04X instead of %04X\n", rd_ctx.crc, crc);
		return -1;
	}

	return 0;
}

int pl_wflib_init_eeprom(struct pl_wflib *wflib, struct pl_wflib_eeprom_ctx *p,
			 const struct i2c_eeprom *eeprom,
			 const struct pl_dispinfo *dispinfo)
{
	p->eeprom = eeprom;
	p->dispinfo = dispinfo;
	p->offset = 0;

	wflib->xfer = pl_wflib_eeprom_xfer;
	wflib->size = dispinfo->info.waveform_full_length;
	wflib->priv = p;

	printf("EEPROM + LZSS (%s)\n", dispinfo->info.waveform_id);

	return 0;
}
