#include <stdbool.h>
#include <stddef.h>

typedef float quiet_beacon_sample_t;

struct quiet_beacon_encoder;
typedef struct quiet_beacon_encoder quiet_beacon_encoder;

quiet_beacon_encoder *quiet_beacon_encoder_create(float sample_rate, float target_freq,
                                                  float gain);

void quiet_beacon_encoder_destroy(quiet_beacon_encoder *e);

void quiet_beacon_encoder_emit(quiet_beacon_encoder *e, quiet_beacon_sample_t *buf, size_t buflen);

struct quiet_beacon_decoder;
typedef struct quiet_beacon_decoder quiet_beacon_decoder;

quiet_beacon_decoder *quiet_beacon_decoder_create(float sample_rate, float target_freq,
                                                  size_t block_len, float thresh);

void quiet_beacon_decoder_destroy(quiet_beacon_decoder *d);

void quiet_beacon_decoder_detect(quiet_beacon_decoder *d, quiet_beacon_sample_t *buf,
                                 bool *detect, size_t detect_len);
