#pragma once
#include "ui.h"
#include <string.h>

static WINDOW **grid_windows      = NULL;
static int      grid_window_count = 0;

/* Recreate the window pool only when the module count changes. */
static void grid_sync_windows(const Rack *rack) {
    if (grid_window_count == rack->module_count) return;

    for (int i = 0; i < grid_window_count; i++)
        if (grid_windows[i]) delwin(grid_windows[i]);
    free(grid_windows);

    grid_window_count = rack->module_count;
    grid_windows      = calloc(grid_window_count, sizeof(WINDOW *));

    for (int i = 0; i < grid_window_count; i++) {
        int col = i % GRID_COLUMNS;
        int row = i / GRID_COLUMNS;
        grid_windows[i] = newwin(MODULE_HEIGHT, MODULE_WIDTH,
                                 row * MODULE_HEIGHT, col * MODULE_WIDTH);
    }
}

static void grid_draw_module(WINDOW *window, Module *module, int focused) {
    int inner_width = MODULE_WIDTH - 2;
    werase(window);

    if (focused) wattron(window, COLOR_PAIR(1) | A_BOLD);
    box(window, 0, 0);
    char label_buf[MAX_NAME + 2];
    snprintf(label_buf, sizeof(label_buf), " %s ", module->label);
    mvwprintw(window, 0, 1, "%-.*s", inner_width, label_buf);
    if (focused) wattroff(window, COLOR_PAIR(1) | A_BOLD);

    for (int i = 0; i < module->field_count; i++) {
        Field *field     = &module->fields[i];
        int    field_row = 1 + i;
        if (field_row >= MODULE_HEIGHT - 2) break;

        field_refresh_display(field);

        int highlighted = focused && (selected_field == i);
        if (highlighted) wattron(window, A_REVERSE);

        int name_len  = (int)strlen(field->name);
        int value_len = (int)strlen(field->display);
        int gap       = inner_width - name_len - value_len;
        if (gap < 1) gap = 1;
        mvwprintw(window, field_row, 1, "%s%*s%s",
                  field->name, gap, "", field->display);

        if (highlighted) wattroff(window, A_REVERSE);
    }

    /* port labels: inputs on left, outputs on right, bottom row */
    char input_ports[MODULE_WIDTH]  = {0};
    char output_ports[MODULE_WIDTH] = {0};
    for (int i = 0; i < module->port_count; i++) {
        Port *port   = &module->ports[i];
        char *target = port->direction == PORT_IN ? input_ports : output_ports;
        int   space  = (int)(port->direction == PORT_IN
                             ? sizeof(input_ports) : sizeof(output_ports));
        if (target[0]) strncat(target, " ", space - strlen(target) - 1);
        strncat(target, port->name, space - strlen(target) - 1);
    }
    if (input_ports[0] || output_ports[0]) {
        int gap = inner_width - (int)strlen(input_ports) - (int)strlen(output_ports);
        if (gap < 0) gap = 0;
        mvwprintw(window, MODULE_HEIGHT - 2, 1, "%s%*s%s",
                  input_ports, gap, "", output_ports);
    }

    wnoutrefresh(window);
}

static inline void grid_draw(Rack *rack) {
    grid_sync_windows(rack);
    for (int i = 0; i < rack->module_count; i++)
        grid_draw_module(grid_windows[i], &rack->modules[i], i == cursor_index);
}

static inline void grid_free(void) {
    for (int i = 0; i < grid_window_count; i++)
        if (grid_windows[i]) delwin(grid_windows[i]);
    free(grid_windows);
    grid_windows      = NULL;
    grid_window_count = 0;
}
