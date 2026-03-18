#pragma once
#include "../rack.h"
#include <stdlib.h>

typedef struct {
    ma_splitter_node node;
} OutputData;

static void output_set_volume(void *data, float value) {
    ma_node_set_output_bus_volume((ma_node *)&((OutputData *)data)->node, 0, value);
}

static void output_cleanup(Module *module) {
    ma_splitter_node_uninit(&((OutputData *)module->data)->node, NULL);
    free(module->data);
}

static inline Module output_init(Rack *rack, const char *label, float volume) {
    Module module = {0};
    OutputData *data = calloc(1, sizeof(OutputData));
    if (!data) return module;

    ma_splitter_node_config config =
        ma_splitter_node_config_init(ma_engine_get_channels(&rack->engine));

    if (ma_splitter_node_init(ma_engine_get_node_graph(&rack->engine),
                              &config, NULL, &data->node) != MA_SUCCESS) {
        free(data); return module;
    }

    ma_node_set_output_bus_volume((ma_node *)&data->node, 0, volume);

    /* wire directly to the engine speaker endpoint */
    ma_node_attach_output_bus((ma_node *)&data->node, 0,
                              ma_engine_get_endpoint(&rack->engine), 0);

    snprintf(module.label, MAX_NAME, "%s", label);
    module.data    = data;
    module.node    = (ma_node *)&data->node;
    module.cleanup = output_cleanup;

    module_add_value_field(&module, "volume", volume, 0.0f, 1.0f, 0.01f, output_set_volume);
    module_add_port       (&module, "in", PORT_IN, 0);

    return module;
}
