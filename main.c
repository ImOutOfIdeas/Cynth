#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "rack.h"
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
    if (rack_init(&rack) != MA_SUCCESS) {
        fprintf(stderr, "failed to init rack\n");
        return 1;
    }

    Module *oscillator_a = rack_add_module(&rack, oscillator_init(&rack, "VCO A", ma_waveform_type_sine,     0.5f, 220.0f));
    Module *oscillator_b = rack_add_module(&rack, oscillator_init(&rack, "VCO B", ma_waveform_type_triangle, 0.5f, 330.0f));
    Module *gain_a       = rack_add_module(&rack, gain_init(&rack, "GAIN A", 0.4f));
    Module *gain_b       = rack_add_module(&rack, gain_init(&rack, "GAIN B", 0.4f));
    Module *output       = rack_add_module(&rack, output_init(&rack, "OUTPUT", 0.8f));

    if (!oscillator_a || !oscillator_b || !gain_a || !gain_b || !output) {
        fprintf(stderr, "module init failed\n");
        rack_free(&rack);
        return 1;
    }

    rack_connect(oscillator_a, 0, gain_a, 0);
    rack_connect(oscillator_b, 0, gain_b, 0);
    rack_connect(gain_a, 0, output, 0);
    rack_connect(gain_b, 0, output, 0);

    ui_init();
    while (1) {
        ui_draw(&rack);
        if (input_handle(ui_poll_key(), &rack)) break;
        ma_sleep(16);
    }
    ui_free();
    rack_free(&rack);
    return 0;
}
