#include <stdlib.h>
#include <math.h>

#include <liquid/liquid.h>
#include "quiet-beacon.h"

struct quiet_beacon_encoder {
    nco_crcf nco;
    float gain;
};
