#pragma once
#include "../rack.h"
#include <stdlib.h>

// == MODULE DATA ===================================================
typedef struct {
    ma_waveform         waveform;
    ma_data_source_node dataSourceNode;
} OscillatorData;

static const char *waveOptions[] = { "SIN", "SQR", "TRI", "SAW" };
static const ma_waveform_type waveTypes[] = {
    ma_waveform_type_sine,
    ma_waveform_type_square,
    ma_waveform_type_triangle,
    ma_waveform_type_sawtooth,
};

// == MODULE SETTERS/GETTERS ========================================
static void oscillatorSetFrequency(void *data, float value) {
    ma_waveform_set_frequency(&((OscillatorData *)data)->waveform, value);
}

static void oscillatorSetAmplitude(void *data, float value) {
    ma_waveform_set_amplitude(&((OscillatorData *)data)->waveform, value);
}

static void oscillatorSetWave(void *data, float value) {
    int index = (int)value;
    ma_waveform_set_type(&((OscillatorData *)data)->waveform, waveTypes[index]);
}

// == MODULE LIFECYCLE ==============================================
static void oscillatorCleanup(Module *module) {
    OscillatorData *data = (OscillatorData *)module->data;
    ma_data_source_node_uninit(&data->dataSourceNode, NULL);
    ma_waveform_uninit(&data->waveform);
    free(data);
}

static inline Module oscillatorInit(Rack *rack, const char *label,
                                     ma_waveform_type waveType,
                                     float amplitude, float frequency) {
    Module module = {0};
    OscillatorData *data = calloc(1, sizeof(OscillatorData));
    if (!data) return module;

    ma_waveform_config waveConfig = ma_waveform_config_init(
        ma_format_f32,
        ma_engine_get_channels(&rack->engine),
        ma_engine_get_sample_rate(&rack->engine),
        waveType, amplitude, frequency);

    if (ma_waveform_init(&waveConfig, &data->waveform) != MA_SUCCESS) {
        free(data); return module;
    }

    ma_data_source_node_config nodeConfig = ma_data_source_node_config_init(&data->waveform);
    if (ma_data_source_node_init(ma_engine_get_node_graph(&rack->engine),
                                  &nodeConfig, NULL, &data->dataSourceNode) != MA_SUCCESS) {
        ma_waveform_uninit(&data->waveform);
        free(data); return module;
    }

    /* find initial wave index */
    int initialWaveIndex = 0;
    for (int i = 0; i < 4; i++)
        if (waveTypes[i] == waveType) { initialWaveIndex = i; break; }

    snprintf(module.label, MAX_NAME, "%s", label);
    module.data    = data;
    module.node    = (ma_node *)&data->dataSourceNode;
    module.cleanup = oscillatorCleanup;

    addValueField (&module, "FRQ", frequency, 20.0f, 2000.0f, 1.0f,  oscillatorSetFrequency);
    addValueField (&module, "AMP",  amplitude, 0.0f,  1.0f,    0.01f, oscillatorSetAmplitude);
    addSelectField(&module, "WAV", initialWaveIndex, 4, waveOptions,  oscillatorSetWave);
    addPort       (&module, "OUT", PORT_OUT, 0);

    return module;
}
