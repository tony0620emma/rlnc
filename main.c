#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "encoder.h"
#include "decoder.h"

int main(int argc, char *argv[])
{
	srand(time(NULL));

	uint32_t symbol_size = 32;
	struct encoder *encoder = encoder_create(symbol_size);
	struct decoder *decoder = decoder_create(symbol_size);

	int32_t i;
	for (i = 0; i < encoder->block_size; i++) {
		encoder->block[i] = (uint8_t) rand();
	}

	uint8_t *payload = calloc(encoder->symbol_size + 1, sizeof(uint8_t));

	while (decoder->rank < 8) {
		encoder_write_payload(encoder, payload);
		decoder_read_payload(decoder, payload);
		printf("decoder rank = %u\n", decoder->rank);
		decoder_print(decoder);
	}

	printf("\ndecoding block...\n\n");
	decoder_decode_block(decoder);
	decoder_print(decoder);

	printf("\ndecode finished\n\n");

	if (memcmp(decoder->block, encoder->block, decoder->symbols * decoder->symbol_size) == 0)
		printf("decode success\n");
	else
		printf("decode fail\n");

	free(payload);
	decoder_destroy(&decoder);
	encoder_destroy(&encoder);
	return 0;
}
