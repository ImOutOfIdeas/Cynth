#pragma once
#include "../miniaudio.h"
#include <stdio.h>

#define MAX_MODULES  64
#define MAX_FIELDS    8
#define MAX_PORTS     4
#define MAX_OPTIONS   8
#define MAX_NAME     32

typedef enum { FIELD_VALUE, FIELD_SELECT } FieldKind;

typedef struct {
    FieldKind   kind;
    char        name[16];
    char        display[16];

    float       value, minimum, maximum, step;

    int         selected_index;
    int         option_count;
    const char *options[MAX_OPTIONS];

    void (*on_change)(void *data, float value);
} Field;

typedef enum { PORT_IN, PORT_OUT } PortDirection;

typedef struct {
    char          name[16];
    PortDirection direction;
    int           bus;
} Port;

typedef struct Module {
    char     label[MAX_NAME];
    ma_node *node;
    void    *data;
    void   (*cleanup)(struct Module *);

    Field    fields[MAX_FIELDS];
    int      field_count;

    Port     ports[MAX_PORTS];
    int      port_count;
} Module;

typedef struct {
    ma_engine engine;
    Module    modules[MAX_MODULES];
    int       module_count;
} Rack;

ma_result rack_init(Rack *rack);
void      rack_free(Rack *rack);
Module   *rack_add_module(Rack *rack, Module module);
void      rack_swap_modules(Rack *rack, int index_a, int index_b);
ma_result rack_connect(Module *source, int source_bus, Module *dest, int dest_bus);

#include "module.h"
#include "modules/oscillator.h"
#include "modules/gain.h"
#include "modules/noise.h"
#include "modules/output.h"
#include "module_registry.h"
#include "rack.c"
