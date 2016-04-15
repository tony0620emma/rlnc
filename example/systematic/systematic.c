#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "encoder.h"
#include "decoder.h"

#include <sys/stat.h>

static double diff_in_second(struct timespec t1, struct timespec t2)
{
	struct timespec diff;
	if (t2.tv_nsec-t1.tv_nsec < 0) {
		diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
		diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
	} else {
		diff.tv_sec  = t2.tv_sec - t1.tv_sec;
		diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
	}
	return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}


int main(int argc, char *argv[])
{
	srand(time(NULL));

	struct timespec start, end;
	double cpu_time = 0.0;
	if (argc < 3) {
		fprintf(stdout, "Usage: %s [file_name] [decode file]\n", argv[0]);
	}

	FILE *fp;
	fp = fopen(argv[1], "r");

	FILE *fb;
	fb = fopen(argv[2], "w");

	uint32_t symbol_size = 16*70;//16 bytes = 128 bits for SIMD
	struct encoder *encoder = encoder_create(symbol_size);

	/*int32_t i;
	for (i = 0; i < encoder->block_size; i++) {
		encoder->block[i] = (uint8_t) rand();
	}*/

	uint8_t *payload = calloc(encoder->symbol_size + 1, sizeof(uint8_t));

	clock_gettime(CLOCK_REALTIME, &start);
	while (!feof(fp)) {
		int number = fread(encoder->block, sizeof(char), symbol_size, fp);

		struct decoder *decoder = decoder_create(symbol_size);

		/* systematic */
		int o;
		for (o = 0; o < encoder->symbols; o++) {
			encoder->flag = 1;
			encoder_write_payload(encoder, payload);

			/* with 75% probability to trigger systematic */
			if ((rand() % 4) != 0) {
				decoder_read_payload(decoder, payload);
				printf("decoder rank = %u\n", decoder->rank);
				decoder_print(decoder);
			}
		}

		/* rlnc */
		while (decoder->rank < 8) {
			encoder->flag = 0;
			encoder_write_payload(encoder, payload);
			decoder_read_payload(decoder, payload);
			//printf("decoder rank = %u\n", decoder->rank);
			//decoder_print(decoder);
		}

		printf("\ndecoding block...\n\n");
		decoder_decode_block(decoder);
		decoder_print(decoder);

		printf("\ndecode finished\n\n");

		if (memcmp(decoder->block, encoder->block, decoder->symbols * decoder->symbol_size) == 0)
			printf("decode success\n");
		else
			printf("decode fail\n");

		fwrite(decoder->block, sizeof(char), number, fb);
		memset(payload, 0, encoder->symbol_size + 1);
		decoder_flush(decoder);
		decoder_destroy(&decoder);
	}
	clock_gettime(CLOCK_REALTIME, &end);
	cpu_time = diff_in_second(start, end);
	printf("execution time of transmission : %lf sec\n", cpu_time);

	fclose(fb);
	free(payload);
	encoder_destroy(&encoder);
	return 0;
}
