#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "msb.h"
#include "decoder.h"

static inline void exclusive_or(uint8_t *dst, uint8_t *src, size_t n)
{
	int i;
	for (i = 0; i < n; i++)
		*dst++ ^= *src++;
}

void decoder_read_payload(struct decoder *decoder, uint8_t *payload_in)
{
	uint8_t *payload = (uint8_t *) calloc(decoder->symbol_size, sizeof(uint8_t));
	uint8_t vector;

	memcpy(payload, payload_in, decoder->symbol_size);
	vector = payload_in[decoder->symbol_size];
	
	/* get the most significant bit of the vector */
	int32_t msb = msb32(vector);
	
	/* the decoder state should be initialized to be 0 */
	while (msb >= 0 && decoder->state[msb]) {
		vector ^= decoder->state[msb];
		exclusive_or(payload, decoder->data[msb], decoder->symbol_size);
		msb = msb32(vector);
	}

	if (msb >= 0) {
		decoder->data[msb] = payload;
		decoder->state[msb] = vector;
		decoder->rank++;
	}

	decoder->count++;
}

void decoder_print(struct decoder *decoder)
{
	printf("decoder state :\n");
	uint32_t mask;
	int32_t i, j;
	for (i = decoder->symbols - 1; i >= 0; i--) {
		mask = 0x80;
		for (j = 0; j < decoder->symbols; j++) {
			if (mask & decoder->state[i])
				printf("1 ");
			else
				printf("0 ");
			mask >>= 1;
		}
		printf("\n");
	}
}
