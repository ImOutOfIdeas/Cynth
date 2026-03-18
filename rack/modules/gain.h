#pragma once
#include "../rack.h"
#include <stdlib.h>

typedef struct {
    ma_splitter_node node;
} GainData;

static void gain_set_volume(void *data, float value) {
    ma_node_set_output_bus_volume((ma_node *)&((GainData *)data)->node, 0, value);
}

static void gain_cleanup(Module *module) {
    ma_splitter_node_uninit(&((GainData *)module->data)->node, NULL);
    free(module->data);
}

static inline Module gain_init(Rack *rack, const char *label, float volume) {
    Module module = {0};
    GainData *data = calloc(1, sizeof(GainData));
    if (!data) return module;

    ma_splitter_node_config config =
        ma_splitter_node_config_init(ma_engine_get_channels(&rack->engine));

    if (ma_splitter_node_init(ma_engine_get_node_graph(&rack->engine),
                              &config, NULL, &data->node) != MA_SUCCESS) {
        free(data); return module;
    }

    ma_node_set_output_bus_volume((ma_node *)&data->node, 0, volume);

    snprintf(module.label, MAX_NAME, "%s", label);
    module.data    = data;
    module.node    = (ma_node *)&data->node;
    module.cleanup = gain_cleanup;

    module_add_value_field(&module, "volume", volume, 0.0f, 1.0f, 0.01f, gain_set_volume);
    module_add_port       (&module, "in",  PORT_IN,  0);
    module_add_port       (&module, "out", PORT_OUT, 0);

    return module;
}
