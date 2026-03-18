#include "rack.h"
#include <string.h>

ma_result rack_init(Rack *rack) {
    memset(rack, 0, sizeof(*rack));
    ma_engine_config config = ma_engine_config_init();
    return ma_engine_init(&config, &rack->engine);
}

void rack_free(Rack *rack) {
    /* free in reverse so outputs are cleaned before their inputs */
    for (int i = rack->module_count - 1; i >= 0; i--) {
        Module *module = &rack->modules[i];
        if (module->cleanup) module->cleanup(module);
    }
    ma_engine_uninit(&rack->engine);
    memset(rack, 0, sizeof(*rack));
}

Module *rack_add_module(Rack *rack, Module module) {
    if (rack->module_count >= MAX_MODULES) return NULL;
    rack->modules[rack->module_count++] = module;
    return &rack->modules[rack->module_count - 1];
}

void rack_swap_modules(Rack *rack, int index_a, int index_b) {
    if (index_a < 0 || index_b < 0) return;
    if (index_a >= rack->module_count || index_b >= rack->module_count) return;
    Module temp               = rack->modules[index_a];
    rack->modules[index_a]    = rack->modules[index_b];
    rack->modules[index_b]    = temp;
}

ma_result rack_connect(Module *source, int source_bus, Module *dest, int dest_bus) {
    return ma_node_attach_output_bus(source->node, source_bus, dest->node, dest_bus);
}
