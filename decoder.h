#pragma once

#include <stdint.h>

struct decoder {
	uint32_t symbols;
	uint32_t symbol_size;

	/* FIXME: enable arbitrary vector size */
	/* assume vector only have one size that is 8-bits long */
	uint8_t *state;
	uint8_t **data;

	uint32_t count;
	uint32_t rank;
};

void decoder_read_payload(struct decoder *decoder, uint8_t *payload_in);
void decoder_decode_block(struct decoder *decoder);
void decoder_print(struct decoder *decoder);
