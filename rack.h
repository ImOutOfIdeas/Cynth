#pragma once
#include "miniaudio.h"
#include <stdio.h>

#define MAX_MODULES 64
#define MAX_FIELDS   8
#define MAX_PORTS    4
#define MAX_NAME    32
#define MAX_OPTIONS  8  /* max choices in a selection field */

/* A field is either a continuous knob or a discrete selector */
typedef enum { FIELD_VALUE, FIELD_SELECT } FieldType;

typedef struct {
    FieldType   fieldType;
    char        name[16];
    char        displayValue[16];   /* formatted string written each frame */

    /* value fields */
    float       value, minimum, maximum, step;

    /* select fields */
    int         selectedIndex;
    int         optionCount;
    const char *options[MAX_OPTIONS];

    /* callbacks into the module's miniaudio data */
    void (*onChange)(void *moduleData, float value);
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
    int      fieldCount;

    Port     ports[MAX_PORTS];
    int      portCount;
} Module;

typedef struct {
    ma_engine engine;
    Module    modules[MAX_MODULES];
    int       moduleCount;
} Rack;

/* lifecycle */
ma_result rackInit(Rack *rack);
void      rackFree(Rack *rack);

/* modules */
Module *rackAddModule(Rack *rack, Module module);
void    rackSwapModules(Rack *rack, int indexA, int indexB);

/* patching */
ma_result rackConnect(Module *source, int sourceBus, Module *dest, int destBus);
ma_result rackConnectToOutput(Rack *rack, Module *source, int sourceBus);

/* --- field helpers called inside module init functions --- */

static inline void addValueField(Module *module, const char *name,
                                  float initial, float minimum, float maximum, float step,
                                  void (*onChange)(void *, float)) {
    if (module->fieldCount >= MAX_FIELDS) return;
    Field *field        = &module->fields[module->fieldCount++];
    field->fieldType    = FIELD_VALUE;
    field->value        = initial;
    field->minimum      = minimum;
    field->maximum      = maximum;
    field->step         = step;
    field->onChange     = onChange;
    field->displayValue[0] = '\0';
    snprintf(field->name, sizeof(field->name), "%s", name);
}

static inline void addSelectField(Module *module, const char *name,
                                   int initialIndex, int optionCount, const char **options,
                                   void (*onChange)(void *, float)) {
    if (module->fieldCount >= MAX_FIELDS) return;
    Field *field         = &module->fields[module->fieldCount++];
    field->fieldType     = FIELD_SELECT;
    field->selectedIndex = initialIndex;
    field->optionCount   = optionCount;
    field->onChange      = onChange;
    field->displayValue[0] = '\0';
    snprintf(field->name, sizeof(field->name), "%s", name);
    for (int i = 0; i < optionCount && i < MAX_OPTIONS; i++)
        field->options[i] = options[i];
}

static inline void addPort(Module *module, const char *name,
                            PortDirection direction, int bus) {
    if (module->portCount >= MAX_PORTS) return;
    Port *port      = &module->ports[module->portCount++];
    port->direction = direction;
    port->bus       = bus;
    snprintf(port->name, sizeof(port->name), "%s", name);
}

/* called by ui each frame to refresh displayValue */
static inline void fieldRefreshDisplay(Field *field) {
    if (field->fieldType == FIELD_VALUE)
        snprintf(field->displayValue, sizeof(field->displayValue), "%.2f", field->value);
    else
        snprintf(field->displayValue, sizeof(field->displayValue),
                 "%s", field->options[field->selectedIndex]);
}

/* called by input to nudge a field and fire onChange */
static inline void fieldAdjust(Field *field, void *moduleData, float delta) {
    if (field->fieldType == FIELD_VALUE) {
        float newValue = field->value + delta;
        if (newValue < field->minimum) newValue = field->minimum;
        if (newValue > field->maximum) newValue = field->maximum;
        field->value = newValue;
        if (field->onChange) field->onChange(moduleData, newValue);
    } else {
        int newIndex = field->selectedIndex + (int)delta;
        if (newIndex < 0) newIndex = 0;
        if (newIndex >= field->optionCount) newIndex = field->optionCount - 1;
        field->selectedIndex = newIndex;
        if (field->onChange) field->onChange(moduleData, (float)newIndex);
    }
}
