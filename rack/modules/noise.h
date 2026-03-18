#pragma once
#include "../rack.h"
#include <stdlib.h>

typedef struct {
    ma_noise            noise;
    ma_data_source_node node;
} NoiseData;

static void noise_set_amplitude(void *data, float value) {
    ma_noise_set_amplitude(&((NoiseData *)data)->noise, value);
}

static void noise_cleanup(Module *module) {
    NoiseData *data = (NoiseData *)module->data;
    ma_data_source_node_uninit(&data->node, NULL);
    ma_noise_uninit(&data->noise, NULL);
    free(data);
}

static inline Module noise_init(Rack *rack, const char *label,
                                ma_noise_type noise_type, float amplitude) {
    Module module = {0};
    NoiseData *data = calloc(1, sizeof(NoiseData));
    if (!data) return module;

    ma_noise_config noise_config = ma_noise_config_init(
        ma_format_f32, ma_engine_get_channels(&rack->engine),
        noise_type, 0, amplitude);

    if (ma_noise_init(&noise_config, NULL, &data->noise) != MA_SUCCESS) {
        free(data); return module;
    }

    ma_data_source_node_config node_config =
        ma_data_source_node_config_init(&data->noise);

    if (ma_data_source_node_init(ma_engine_get_node_graph(&rack->engine),
                                 &node_config, NULL, &data->node) != MA_SUCCESS) {
        ma_noise_uninit(&data->noise, NULL);
        free(data); return module;
    }

    snprintf(module.label, MAX_NAME, "%s", label);
    module.data    = data;
    module.node    = (ma_node *)&data->node;
    module.cleanup = noise_cleanup;

    module_add_value_field(&module, "amplitude", amplitude, 0.0f, 1.0f, 0.01f, noise_set_amplitude);
    module_add_port       (&module, "out", PORT_OUT, 0);

    return module;
}
