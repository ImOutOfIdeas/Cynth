#pragma once
#include "rack.h"

/*
 * Module builder API
 *
 * Call these inside your module's init function after zeroing a Module and
 * assigning .label, .node, .data, and .cleanup.  Each function appends one
 * field or port to the module.
 *
 * Example:
 *
 *   Module module = {0};
 *   snprintf(module.label, MAX_NAME, "%s", label);
 *   module.data    = data;
 *   module.node    = (ma_node *)&data->node;
 *   module.cleanup = my_cleanup;
 *
 *   module_add_value_field (&module, "vol",  0.5f, 0.0f, 1.0f, 0.01f, my_set_volume);
 *   module_add_select_field(&module, "wave", 0, 4, wave_options, my_set_wave);
 *   module_add_port        (&module, "out",  PORT_OUT, 0);
 */

static inline void module_add_value_field(Module *module, const char *name,
                                          float initial, float minimum,
                                          float maximum, float step,
                                          void (*on_change)(void *, float)) {
    if (module->field_count >= MAX_FIELDS) return;
    Field *field     = &module->fields[module->field_count++];
    field->kind      = FIELD_VALUE;
    field->value     = initial;
    field->minimum   = minimum;
    field->maximum   = maximum;
    field->step      = step;
    field->on_change = on_change;
    field->display[0] = '\0';
    snprintf(field->name, sizeof(field->name), "%s", name);
}

static inline void module_add_select_field(Module *module, const char *name,
                                           int initial_index, int option_count,
                                           const char **options,
                                           void (*on_change)(void *, float)) {
    if (module->field_count >= MAX_FIELDS) return;
    Field *field          = &module->fields[module->field_count++];
    field->kind           = FIELD_SELECT;
    field->selected_index = initial_index;
    field->option_count   = option_count;
    field->on_change      = on_change;
    field->display[0]     = '\0';
    snprintf(field->name, sizeof(field->name), "%s", name);
    for (int i = 0; i < option_count && i < MAX_OPTIONS; i++)
        field->options[i] = options[i];
}

static inline void module_add_port(Module *module, const char *name,
                                   PortDirection direction, int bus) {
    if (module->port_count >= MAX_PORTS) return;
    Port *port      = &module->ports[module->port_count++];
    port->direction = direction;
    port->bus       = bus;
    snprintf(port->name, sizeof(port->name), "%s", name);
}

/* Refresh the display string - called by ui each frame */
static inline void field_refresh_display(Field *field) {
    if (field->kind == FIELD_VALUE)
        snprintf(field->display, sizeof(field->display), "%.2f", field->value);
    else
        snprintf(field->display, sizeof(field->display),
                 "%s", field->options[field->selected_index]);
}

/* Nudge a field by one step and fire on_change - called by input */
static inline void field_step(Field *field, void *module_data, int direction) {
    if (field->kind == FIELD_VALUE) {
        float next = field->value + direction * field->step;
        if (next < field->minimum) next = field->minimum;
        if (next > field->maximum) next = field->maximum;
        field->value = next;
        if (field->on_change) field->on_change(module_data, next);
    } else {
        int next = field->selected_index + direction;
        if (next < 0) next = 0;
        if (next >= field->option_count) next = field->option_count - 1;
        field->selected_index = next;
        if (field->on_change) field->on_change(module_data, (float)next);
    }
}
