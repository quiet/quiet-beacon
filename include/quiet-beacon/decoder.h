#include <stdlib.h>
#include <math.h>

#include "quiet-beacon.h"

struct quiet_beacon_decoder {
    quiet_beacon_sample_t coeff;
    quiet_beacon_sample_t thresh;
    size_t block_len;
};
