#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"   // must come first, defines all miniaudio symbols

#include "rack.h"        // types everything else depends on
#include "rack.c"

#include "ui.h"
#include "ui.c"

#include "input.h"
#include "input.c"

#include "modules/oscillator.h"
#include "modules/gain.h"
#include "modules/noise.h"
#include "modules/output.h"

int main(void) {
    Rack rack;
    if (rackInit(&rack) != MA_SUCCESS) {
        fprintf(stderr, "Failed to init rack\n");
        return 1;
    }

    Module *oscillatorA = rackAddModule(&rack, oscillatorInit(&rack, "VCO A", ma_waveform_type_sine,     0.5f, 220.0f));
    Module *oscillatorB = rackAddModule(&rack, oscillatorInit(&rack, "VCO B", ma_waveform_type_triangle,  0.5f, 330.0f));
    Module *gainA       = rackAddModule(&rack, gainInit(&rack, "GAIN A", 0.4f));
    Module *gainB       = rackAddModule(&rack, gainInit(&rack, "GAIN B", 0.4f));
    Module *out         = rackAddModule(&rack, outputInit(&rack, "OUTPUT", 0.8f));

    if (!oscillatorA || !oscillatorB || !gainA || !gainB || !out) {
        fprintf(stderr, "Module init failed\n");
        rackFree(&rack);
        return 1;
    }

    rackConnect(oscillatorA, 0, gainA, 0);
    rackConnect(oscillatorB, 0, gainB, 0);
    rackConnect(gainA, 0, out, 0);
    rackConnect(gainB, 0, out, 0);

    uiInit();
    while (1) {
        uiDraw(&rack);
        if (inputHandle(uiPollKey(), &rack)) break;
        ma_sleep(16);
    }
    uiFree();
    rackFree(&rack);
    return 0;
}
