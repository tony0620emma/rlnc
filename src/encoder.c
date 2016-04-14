#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <xmmintrin.h>

#include "msb.h"
#include "encoder.h"
#include <stdio.h>

static inline void exclusive_or(uint8_t *dst, uint8_t *src, size_t n)
{
    int i;
#ifdef _XMMINTRIN_H_INCLUDED
	/* Implement AVX/SIMD : xor 128-bits in one turn*/
	for (i=0; i<n ; i=i+16) {
		__m128i xmm1 = _mm_loadu_si128((__m128i *)(dst+i));
		__m128i xmm2 = _mm_loadu_si128((__m128i *)(src+i));
		xmm1 = _mm_xor_si128(xmm1, xmm2);
		_mm_store_si128((__m128i *)(dst+i), xmm1);
	}
#else
    for (i = 0; i < n; i++)
        *dst++ ^= *src++;
#endif
}

struct encoder *encoder_create(uint32_t symbol_size)
{
	struct encoder *encoder;
	encoder = (struct encoder *) calloc(1, sizeof(struct encoder));

	/* TODO: implement more symbols support */
	encoder->symbols = 8;
	encoder->symbol_size = symbol_size;
	encoder->block_size  = symbol_size * encoder->symbols;
	encoder->block = (uint8_t *) calloc(encoder->block_size, sizeof(uint8_t));
	encoder->symbol = (uint8_t **) calloc(encoder->symbols, sizeof(uint8_t *));
	int32_t i;
	uint8_t *mem = encoder->block;
	for (i = 0; i < encoder->symbols; i++, mem += symbol_size) {
		encoder->symbol[i] = mem;
	}
	encoder->flags = 0;
	return encoder;
}

void encoder_destroy(struct encoder **encoder_t)
{
	if (*encoder_t) {
		if ((*encoder_t)->symbol) {
			free((*encoder_t)->symbol);
			(*encoder_t)->symbol = NULL;
		}
		if ((*encoder_t)->block) {
			free((*encoder_t)->block);
			(*encoder_t)->block = NULL;
		}
		free(*encoder_t);
		*encoder_t = NULL;
	}
}

void encoder_write_payload(struct encoder *encoder, uint8_t *payload_out)
{
	uint8_t vector;
	if (encoder->flag) {	/* systematic */
		printf("systematic\n");
		vector = 0x80;
		vector >>= encoder->flags;
		encoder->flags ++;
		if (encoder->flags >= encoder->symbols)
			encoder->flags = 0;
	}
	else {					/* rlnc */
		/* get the least significant 8 bits for vector randomly */
		vector = (uint8_t) rand();
	}

	/* clear memory, and the 1 is for 8-bit vector */
	memset(payload_out, 0, encoder->symbol_size + 1);
	
	/* mask = 0b10000000 */
	uint8_t mask = 0x80;
	int32_t i;
	for (i = 0; i < encoder->symbols; i++) {
		if (vector & mask)
			exclusive_or(payload_out, encoder->symbol[i], encoder->symbol_size);
		mask >>= 1;
	}

	/* attach vector for decoder */
	payload_out[encoder->symbol_size] = vector;
}
