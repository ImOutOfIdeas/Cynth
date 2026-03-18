#pragma once
#include "../rack.h"
#include "../module.h"
#include <stdlib.h>

typedef struct {
    ma_waveform         waveform;
    ma_data_source_node node;
} OscillatorData;

static const char *oscillator_wave_options[] = { "sin", "sqr", "tri", "saw" };
static const ma_waveform_type oscillator_wave_types[] = {
    ma_waveform_type_sine,
    ma_waveform_type_square,
    ma_waveform_type_triangle,
    ma_waveform_type_sawtooth,
};

static void oscillator_set_frequency(void *data, float value) {
    ma_waveform_set_frequency(&((OscillatorData *)data)->waveform, value);
}

static void oscillator_set_amplitude(void *data, float value) {
    ma_waveform_set_amplitude(&((OscillatorData *)data)->waveform, value);
}

static void oscillator_set_wave(void *data, float value) {
    ma_waveform_set_type(&((OscillatorData *)data)->waveform,
                         oscillator_wave_types[(int)value]);
}

static void oscillator_cleanup(Module *module) {
    OscillatorData *data = (OscillatorData *)module->data;
    ma_data_source_node_uninit(&data->node, NULL);
    ma_waveform_uninit(&data->waveform);
    free(data);
}

static inline Module oscillator_init(Rack *rack, const char *label,
                                     ma_waveform_type wave_type,
                                     float amplitude, float frequency) {
    Module module = {0};
    OscillatorData *data = calloc(1, sizeof(OscillatorData));
    if (!data) return module;

    ma_waveform_config wave_config = ma_waveform_config_init(
        ma_format_f32,
        ma_engine_get_channels(&rack->engine),
        ma_engine_get_sample_rate(&rack->engine),
        wave_type, amplitude, frequency);

    if (ma_waveform_init(&wave_config, &data->waveform) != MA_SUCCESS) {
        free(data); return module;
    }

    ma_data_source_node_config node_config = ma_data_source_node_config_init(&data->waveform);
    if (ma_data_source_node_init(ma_engine_get_node_graph(&rack->engine),
                                  &node_config, NULL, &data->node) != MA_SUCCESS) {
        ma_waveform_uninit(&data->waveform);
        free(data); return module;
    }

    int initial_wave_index = 0;
    for (int i = 0; i < 4; i++)
        if (oscillator_wave_types[i] == wave_type) { initial_wave_index = i; break; }

    snprintf(module.label, MAX_NAME, "%s", label);
    module.data    = data;
    module.node    = (ma_node *)&data->node;
    module.cleanup = oscillator_cleanup;

    module_add_value_field (&module, "frequency", frequency,  20.0f, 2000.0f, 1.0f,  oscillator_set_frequency);
    module_add_value_field (&module, "amplitude", amplitude,   0.0f,    1.0f, 0.01f, oscillator_set_amplitude);
    module_add_select_field(&module, "wave", initial_wave_index, 4,
                            oscillator_wave_options, oscillator_set_wave);
    module_add_port        (&module, "out", PORT_OUT, 0);

    return module;
}
