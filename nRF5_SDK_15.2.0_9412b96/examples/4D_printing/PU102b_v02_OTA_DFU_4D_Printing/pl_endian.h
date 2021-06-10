#ifndef PL_ENDIAN_H
#define PL_ENDIAN_H 1

#include <stdint.h>

/** Swap the 4 bytes of a uint32_t in place, unaligned */
extern void swap32(void *x);
/** Apply swap32 to an arrary of 32-bit words */
extern void swap32_array(int32_t **x, uint16_t n);
/** Swap the 2 bytes of a uint16_t in place, unaligned */
extern void swap16(void *x);
/** Apply swap16 to an array of 16-bit words */
extern void swap16_array(int16_t **x, uint16_t n);

//#define htobe16(_x) _swap_bytes(_x)
//#define htole16(_x) (_x)
//#define be16toh(_x) _swap_bytes(_x)
//#define le16toh(_x) (_x)

#endif /* PL_ENDIAN_H */