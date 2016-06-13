#include "quiet-beacon/decoder.h"
#include <stdio.h>

quiet_beacon_decoder *quiet_beacon_decoder_create(float sample_rate, float target_freq, size_t block_len, float thresh) {
    quiet_beacon_decoder *d = calloc(1, sizeof(quiet_beacon_decoder));

    int k = (int)(0.5 + (block_len * target_freq)/sample_rate);
    float omega = (2 * M_PI/block_len) * k;

    d->coeff = 2 * cos(omega);

    d->block_len = block_len;
    d->thresh = thresh;

    return d;
}

void quiet_beacon_decoder_destroy(quiet_beacon_decoder *d) {
    free(d);
}

void quiet_beacon_decoder_detect(quiet_beacon_decoder *d, quiet_beacon_sample_t *buf, bool *detect, size_t detect_len) {
    // Goertzel algorithm for tone detection
    for (size_t i = 0; i < detect_len; i++) {
        quiet_beacon_sample_t q0, q1, q2;
        q0 = 0;
        q1 = 0;
        q2 = 0;
        ptrdiff_t offset = i * d->block_len;
        for (size_t j = 0; j < d->block_len; j++) {
            q0 = d->coeff * q1 - q2 + buf[offset + j];
            q2 = q1;
            q1 = q0;
        }
        quiet_beacon_sample_t mag = sqrtf(pow(q1, 2) + pow(q2, 2) - (q1 * q2 * d->coeff));
        detect[i] = (mag > d->thresh);
    }
}
