#pragma once

typedef struct {
    const char *name;
    Module    (*create)(Rack *rack);
} ModuleRegistryEntry;

static Module registry_make_oscillator(Rack *rack) {
    return oscillator_init(rack, "VCO", ma_waveform_type_sine, 0.5f, 440.0f);
}
static Module registry_make_gain(Rack *rack) {
    return gain_init(rack, "GAIN", 0.8f);
}
static Module registry_make_noise(Rack *rack) {
    return noise_init(rack, "NOISE", ma_noise_type_white, 0.3f);
}
static Module registry_make_output(Rack *rack) {
    return output_init(rack, "OUTPUT", 0.8f);
}

static const ModuleRegistryEntry module_registry[] = {
    { "Oscillator", registry_make_oscillator },
    { "Gain",       registry_make_gain       },
    { "Noise",      registry_make_noise      },
    { "Output",     registry_make_output     },
};

static const int module_registry_count =
    (int)(sizeof(module_registry) / sizeof(module_registry[0]));

/* Spawn the entry at index into the rack. Returns NULL on failure. */
static inline Module *registry_create(int index, Rack *rack) {
    if (index < 0 || index >= module_registry_count) return NULL;
    return rack_add_module(rack, module_registry[index].create(rack));
}
