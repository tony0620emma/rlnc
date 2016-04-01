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

	struct encoder *encoder = (struct encoder *) calloc(1, sizeof(struct encoder));
	struct decoder *decoder = (struct decoder *) calloc(1, sizeof(struct decoder));

	encoder->symbols = decoder->symbols = 8;
	encoder->symbol_size = 32;
	encoder->symbol = (uint8_t **) calloc(encoder->symbols, sizeof(uint8_t *));

	printf("generating random data ...\n");
	int32_t i, j;
	for (i = 0; i < encoder->symbols; i++) {
		encoder->symbol[i] = (uint8_t *) calloc(encoder->symbol_size, sizeof(uint8_t));
		/* generate random data */
		for (j = 0; j < encoder->symbol_size; j++) {
			encoder->symbol[i][j] = (uint8_t) rand();
		}
	}

	decoder->state = (uint8_t *) calloc(encoder->symbols, sizeof(uint8_t));
	decoder->data = (uint8_t **) calloc(encoder->symbols, sizeof(uint8_t *));
	decoder->symbol_size = encoder->symbol_size;

	uint8_t *payload = calloc(encoder->symbol_size + 1, sizeof(uint8_t));

	while (decoder->rank < 8) {
		encoder_write_payload(encoder, payload);
		decoder_read_payload(decoder, payload);
		printf("decoder rank = %u\n", decoder->rank);
	}

	decoder_print(decoder);

	printf("decode finished\n");

	return 0;
}
