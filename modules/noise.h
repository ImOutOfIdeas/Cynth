#pragma once
#include "../rack.h"
#include <stdlib.h>

typedef struct {
    ma_noise            noise;
    ma_data_source_node dataSourceNode;
} NoiseData;

static void noiseSetAmplitude(void *data, float value) {
    ma_noise_set_amplitude(&((NoiseData *)data)->noise, value);
}

static void noiseCleanup(Module *module) {
    NoiseData *data = (NoiseData *)module->data;
    ma_data_source_node_uninit(&data->dataSourceNode, NULL);
    ma_noise_uninit(&data->noise, NULL);
    free(data);
}

static inline Module noiseInit(Rack *rack, const char *label,
                                ma_noise_type noiseType, float amplitude) {
    Module module = {0};
    NoiseData *data = calloc(1, sizeof(NoiseData));
    if (!data) return module;

    ma_noise_config noiseConfig = ma_noise_config_init(
        ma_format_f32, ma_engine_get_channels(&rack->engine),
        noiseType, 0, amplitude);

    if (ma_noise_init(&noiseConfig, NULL, &data->noise) != MA_SUCCESS) {
        free(data); return module;
    }

    ma_data_source_node_config nodeConfig = ma_data_source_node_config_init(&data->noise);
    if (ma_data_source_node_init(ma_engine_get_node_graph(&rack->engine),
                                  &nodeConfig, NULL, &data->dataSourceNode) != MA_SUCCESS) {
        ma_noise_uninit(&data->noise, NULL);
        free(data); return module;
    }

    snprintf(module.label, MAX_NAME, "%s", label);
    module.data    = data;
    module.node    = (ma_node *)&data->dataSourceNode;
    module.cleanup = noiseCleanup;

    addValueField(&module, "amp", amplitude, 0.0f, 1.0f, 0.01f, noiseSetAmplitude);
    addPort      (&module, "out", PORT_OUT, 0);

    return module;
}
