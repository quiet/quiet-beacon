#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "quiet-beacon.h"

int main() {
    size_t block_len = 400;
    size_t num_blocks = 256;
    size_t buflen = num_blocks * block_len;
    float tone_freq = 15000;
    float bin_size = 44100/(2 * (float)block_len);
    printf("bin size: %f\n", bin_size);
    float wrong_tone_freq = tone_freq - 2 * bin_size;
    quiet_beacon_decoder *d = quiet_beacon_decoder_create(44100, 15000, block_len, 10.0);

    bool *detections = malloc(num_blocks * sizeof(bool));
    quiet_beacon_sample_t *buf = calloc(buflen, sizeof(quiet_beacon_sample_t));

    quiet_beacon_decoder_detect(d, buf, detections, num_blocks);
    size_t ndetections = 0;
    for (size_t i = 0; i < num_blocks; i++) {
        if (detections[i]) {
            ndetections++;
        }
    }
    printf("number of detections on empty buffer: %zu\n", ndetections);

    quiet_beacon_encoder *e = quiet_beacon_encoder_create(44100, wrong_tone_freq, 0.3);
    quiet_beacon_encoder_emit(e, buf, buflen);
    quiet_beacon_decoder_detect(d, buf, detections, num_blocks);

    ndetections = 0;
    for (size_t i = 0; i < num_blocks; i++) {
        if (detections[i]) {
            ndetections++;
        }
    }
    printf("number of detections on wrong tone: %zu\n", ndetections);
    quiet_beacon_encoder_destroy(e);

    e = quiet_beacon_encoder_create(44100, tone_freq, 0.3);
    quiet_beacon_encoder_emit(e, buf, buflen);
    quiet_beacon_decoder_detect(d, buf, detections, num_blocks);

    ndetections = 0;
    for (size_t i = 0; i < num_blocks; i++) {
        if (detections[i]) {
            ndetections++;
        }
    }
    printf("number of detections on correct tone: %zu\n", ndetections);

    srand(time(NULL));

    float p_tone = 0.1;

    memset(buf, 0, buflen * sizeof(quiet_beacon_sample_t));
    bool *pattern = calloc(num_blocks, sizeof(bool));
    for (size_t i = 0; i < num_blocks; i++) {
        if (rand()/(float)RAND_MAX > (1 - p_tone)) {
            quiet_beacon_encoder_emit(e, buf + i * block_len, block_len);
            pattern[i] = true;
        }
    }
    quiet_beacon_decoder_detect(d, buf, detections, num_blocks);

    bool match = memcmp(detections, pattern, num_blocks * sizeof(bool)) == 0;

    printf("detections on correct tone matches random pattern: %s\n", match ? "TRUE" : "FALSE");

    clock_t start, end;
    size_t niter = 1000;
    clock_t total_elapsed = 0;
    for (size_t i = 0; i < niter; i++) {
        start = clock();
        quiet_beacon_decoder_detect(d, buf, detections, num_blocks);
        end = clock();
        total_elapsed += (end - start);
    }

    double throughput = (double)niter * (double)buflen/((double)total_elapsed/CLOCKS_PER_SEC);
    printf("decoder throughput: %f samples/sec (%f ns/sample)\n", throughput, (1/throughput) * 1000000000);
    printf("decoder cpu util (sample rate = 44100Hz): %f\n", 44100/throughput);
    free(pattern);

    quiet_beacon_encoder_destroy(e);
    free(buf);
    free(detections);
    quiet_beacon_decoder_destroy(d);

    return 0;
}
