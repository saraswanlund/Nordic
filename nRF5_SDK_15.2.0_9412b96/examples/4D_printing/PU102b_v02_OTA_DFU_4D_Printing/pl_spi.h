#ifndef PL_SPI_H__
#define PL_SPI_H__

#define PL_CS      5
#define PL_MISO    4
#define PL_MOSI    2
#define PL_SCK     3

extern void PL_SPI_init(void);
extern void spi_write_bytes(uint8_t *data, uint16_t size);
extern void spi_read_bytes(uint8_t *data, uint8_t size);
#endif