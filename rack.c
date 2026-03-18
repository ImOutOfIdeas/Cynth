#include "rack.h"

ma_result rackInit(Rack *rack) {
    memset(rack, 0, sizeof(*rack));
    ma_engine_config config = ma_engine_config_init();
    return ma_engine_init(&config, &rack->engine);
}

void rackFree(Rack *rack) {
    /* free in reverse so outputs are cleaned before their inputs */
    for (int i = rack->moduleCount - 1; i >= 0; i--) {
        Module *module = &rack->modules[i];
        if (module->cleanup) module->cleanup(module);
    }
    ma_engine_uninit(&rack->engine);
    memset(rack, 0, sizeof(*rack));
}

Module *rackAddModule(Rack *rack, Module module) {
    if (rack->moduleCount >= MAX_MODULES) return NULL;
    rack->modules[rack->moduleCount++] = module;
    return &rack->modules[rack->moduleCount - 1];
}

void rackSwapModules(Rack *rack, int indexA, int indexB) {
    if (indexA < 0 || indexB < 0) return;
    if (indexA >= rack->moduleCount || indexB >= rack->moduleCount) return;
    Module temp            = rack->modules[indexA];
    rack->modules[indexA]  = rack->modules[indexB];
    rack->modules[indexB]  = temp;
}

ma_result rackConnect(Module *source, int sourceBus, Module *dest, int destBus) {
    return ma_node_attach_output_bus(source->node, sourceBus, dest->node, destBus);
}

ma_result rackConnectToOutput(Rack *rack, Module *source, int sourceBus) {
    return ma_node_attach_output_bus(source->node, sourceBus,
                                     ma_engine_get_endpoint(&rack->engine), 0);
}
