#pragma once

#include <stdint.h>

struct encoder {
	uint8_t *block;
	uint32_t block_size;
	uint8_t **symbol;

	uint32_t symbols;
	uint32_t symbol_size;
};

struct encoder *encoder_create(uint32_t symbol_size);
void encoder_write_payload(struct encoder *encoder, uint8_t *payload_out);
