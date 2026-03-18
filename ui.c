#include "ui.h"
#include "module.h"
#include <stdlib.h>
#include <string.h>

int cursor_index   = 0;
int selected_field = -1;

static WINDOW **windows      = NULL;
static int      window_count = 0;

void ui_init(void) {
    setlocale(LC_ALL, "");
    set_escdelay(0);
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
}

void ui_free(void) {
    for (int i = 0; i < window_count; i++)
        if (windows[i]) delwin(windows[i]);
    free(windows);
    windows      = NULL;
    window_count = 0;
    endwin();
}

/* recreate windows only when module count changes */
static void sync_windows(const Rack *rack) {
    if (window_count == rack->module_count) return;
    for (int i = 0; i < window_count; i++)
        if (windows[i]) delwin(windows[i]);
    free(windows);
    window_count = rack->module_count;
    windows      = calloc(window_count, sizeof(WINDOW *));
    for (int i = 0; i < window_count; i++) {
        int column = i % GRID_COLUMNS;
        int row    = i / GRID_COLUMNS;
        windows[i] = newwin(MODULE_HEIGHT, MODULE_WIDTH,
                            row * MODULE_HEIGHT, column * MODULE_WIDTH);
    }
}

static void draw_module(WINDOW *window, Module *module, int focused) {
    int inner_width = MODULE_WIDTH - 2;

    werase(window);

    if (focused) wattron(window, COLOR_PAIR(1) | A_BOLD);
    box(window, 0, 0);

    char label_buffer[MAX_NAME + 2];
    snprintf(label_buffer, sizeof(label_buffer), " %s ", module->label);
    mvwprintw(window, 0, 1, "%-.*s", inner_width, label_buffer);
    if (focused) wattroff(window, COLOR_PAIR(1) | A_BOLD);

    for (int i = 0; i < module->field_count; i++) {
        Field *field = &module->fields[i];
        field_refresh_display(field);

        int field_row = 1 + i;
        if (field_row >= MODULE_HEIGHT - 2) break;

        int highlighted = focused && (selected_field == i);
        if (highlighted) wattron(window, A_REVERSE);

        int name_length  = (int)strlen(field->name);
        int value_length = (int)strlen(field->display);
        int gap          = inner_width - name_length - value_length;
        if (gap < 1) gap = 1;
        mvwprintw(window, field_row, 1, "%s%*s%s",
                  field->name, gap, "", field->display);

        if (highlighted) wattroff(window, A_REVERSE);
    }

    /* inputs on left, outputs on right of the bottom row */
    char input_ports[MODULE_WIDTH]  = {0};
    char output_ports[MODULE_WIDTH] = {0};
    for (int i = 0; i < module->port_count; i++) {
        Port *port   = &module->ports[i];
        char *target = port->direction == PORT_IN ? input_ports : output_ports;
        int   space  = port->direction == PORT_IN
                       ? (int)sizeof(input_ports) : (int)sizeof(output_ports);
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

void ui_draw(Rack *rack) {
    sync_windows(rack);
    for (int i = 0; i < rack->module_count; i++)
        draw_module(windows[i], &rack->modules[i], i == cursor_index);
    doupdate();
}

int ui_poll_key(void) {
    return getch();
}
