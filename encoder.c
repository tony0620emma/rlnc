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


void encoder_write_payload(struct encoder *encoder, uint8_t *payload_out)
{
	/* get the least significant 8 bits for vector randomly */
	uint8_t vector = (uint8_t) rand();

	/* set 0, and the 1 is for 8-bit vector */
	memset(payload_out, 0, encoder->symbol_size + 1);
	
	/* mask = 0b10000000 */
	uint8_t mask = 0x80;

	int32_t i;
	for (i = encoder->symbols - 1; i >= 0; i--) {
		if (vector & mask)
			exclusive_or(payload_out, encoder->symbol[i], encoder->symbol_size);
		mask >>= 1;
	}

	/* attach vector for decoder */
	payload_out[encoder->symbol_size] = vector;
}
