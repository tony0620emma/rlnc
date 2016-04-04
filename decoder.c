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

struct decoder *decoder_create(uint32_t symbol_size)
{
	struct decoder *decoder;
	decoder = (struct decoder *) calloc(1, sizeof(struct decoder));
	
	/* TODO: implement more symbols support */
	decoder->symbols = 8;
	decoder->symbol_size = symbol_size;
	decoder->state = (uint8_t *) calloc(decoder->symbols, sizeof(uint8_t));
	decoder->block_size = decoder->symbols * decoder->symbol_size;
	decoder->block = (uint8_t *) calloc(decoder->block_size, sizeof(uint8_t));
	decoder->data = (uint8_t **) calloc(decoder->symbols, sizeof(uint8_t *));
	return decoder;
}

void decoder_destroy(struct decoder **decoder_t)
{
	int32_t i;
	if (*decoder_t) {
		if ((*decoder_t)->data) {
			for (i = 0; i < (*decoder_t)->symbols; i++) {
				if ((*decoder_t)->data[i]) {
					free((*decoder_t)->data[i]);
					(*decoder_t)->data[i] = NULL;
				}
			}
			free((*decoder_t)->data);
			(*decoder_t)->data = NULL;
		}
		if ((*decoder_t)->state) {
			free((*decoder_t)->state);
			(*decoder_t)->state = NULL;
		}
		if ((*decoder_t)->block) {
			free((*decoder_t)->block);
			(*decoder_t)->block = NULL;
		}
		free(*decoder_t);
		*decoder_t = NULL;
	}
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
	} else {
		free(payload);
	}

	decoder->count++;
}

void decoder_decode_block(struct decoder *decoder)
{
	uint32_t mask;
	int32_t i, j;
	for (i = 1; i < decoder->symbols; i++) {
		mask = 0x1;
		for (j = 0; j < i; j++) {
			if (mask & decoder->state[i]) {
				decoder->state[i] &= (~mask);
				exclusive_or(decoder->data[i], decoder->data[j], decoder->symbol_size);
			}
			mask <<= 1;
		}
	}
	uint8_t *mem = decoder->block;
	for (i = decoder->symbols - 1; i >= 0; i--) {
		memcpy(mem, decoder->data[i], decoder->symbol_size);
		mem += decoder->symbol_size;
	}
}

void decoder_flush(struct decoder *decoder)
{
	int32_t i;
	for (i = 0; i < decoder->symbols; i++) {
		free(decoder->data[i]);
		decoder->data[i] = NULL;
	}
	memset(decoder->block, 0, decoder->block_size);
}

void decoder_print(struct decoder *decoder)
{
	printf("decoder state :\n");
	uint32_t mask;
	int32_t i, j;
	for (i = decoder->symbols - 1; i >= 0; i--) {
		mask = 0x80;
		printf("%03d: ", decoder->symbols - i);
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
