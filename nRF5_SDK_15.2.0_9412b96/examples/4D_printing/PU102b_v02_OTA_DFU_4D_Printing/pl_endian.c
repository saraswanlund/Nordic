#include "pl_endian.h"

#include <stdlib.h>
#include <stdio.h>

void swap32(void *x)
{
	uint8_t *b = x;
	uint8_t tmp;

	tmp = b[0];
	b[0] = b[3];
	b[3] = tmp;
	tmp = b[1];
	b[1] = b[2];
	b[2] = tmp;
}

void swap32_array(int32_t **x, uint16_t n)
{
	while (n--)
		swap32(*x++);
}

void swap16(void *x)
{
	uint8_t *b = x;
	uint8_t tmp;

	tmp = b[0];
	b[0] = b[1];
	b[1] = tmp;
}

void swap16_array(int16_t **x, uint16_t n)
{
	while (n--)
		swap16(*x++);
}