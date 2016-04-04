#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "msb.h"
#include "encoder.h"

static inline void exclusive_or(uint8_t *dst, uint8_t *src, size_t n)
{
    int i;
    for (i = 0; i < n; i++)
        *dst++ ^= *src++;
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
	return encoder;
}

void encoder_write_payload(struct encoder *encoder, uint8_t *payload_out)
{
	/* get the least significant 8 bits for vector randomly */
	uint8_t vector = (uint8_t) rand();

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
