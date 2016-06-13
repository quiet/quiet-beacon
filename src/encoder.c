#include "quiet-beacon/encoder.h"

quiet_beacon_encoder *quiet_beacon_encoder_create(float sample_rate, float target_freq, float gain) {
    quiet_beacon_encoder *e = calloc(1, sizeof(quiet_beacon_encoder));

    float omega = (target_freq/sample_rate) * 2 * M_PI;

    e->nco = nco_crcf_create(LIQUID_NCO);
    nco_crcf_set_phase(e->nco, 0.0f);
    nco_crcf_set_frequency(e->nco, omega);

    e->gain = gain;

    return e;
}

void quiet_beacon_encoder_destroy(quiet_beacon_encoder *e) {
    nco_crcf_destroy(e->nco);
    free(e);
}

void quiet_beacon_encoder_emit(quiet_beacon_encoder *e, quiet_beacon_sample_t *buf, size_t buflen) {
    for (size_t i = 0; i < buflen; i++) {
        buf[i] = nco_crcf_cos(e->nco) * e->gain;
        nco_crcf_step(e->nco);
    }
}
