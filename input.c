#include "input.h"
#include "module.h"
#include "ui.h"

static int to_index(int column, int row) { return row * GRID_COLUMNS + column; }
static int column_of(int index)          { return index % GRID_COLUMNS; }
static int row_of(int index)             { return index / GRID_COLUMNS; }

int input_handle(int key, Rack *rack) {
    int module_count = rack->module_count;
    int current      = cursor_index;
    int column       = column_of(current);
    int row          = row_of(current);

    /* field mode: jk move between fields, hl adjust value, escape exits */
    if (selected_field >= 0) {
        Module *module = &rack->modules[current];
        switch (key) {
            case 'j': case KEY_DOWN:
                if (selected_field < module->field_count - 1) selected_field++;
                return 0;
            case 'k': case KEY_UP:
                if (selected_field > 0) selected_field--;
                return 0;
            case 'l': case KEY_RIGHT:
                field_step(&module->fields[selected_field], module->data, +1);
                return 0;
            case 'h': case KEY_LEFT:
                field_step(&module->fields[selected_field], module->data, -1);
                return 0;
            case 27:
                selected_field = -1;
                return 0;
        }
        return 0;
    }

    /* module navigation: hjkl */
    int next = current;
    switch (key) {
        case 'h': if (column > 0)                            next = to_index(column - 1, row); break;
        case 'l': if (column < GRID_COLUMNS - 1)             next = to_index(column + 1, row); break;
        case 'k': if (row > 0)                               next = to_index(column, row - 1); break;
        case 'j': if (to_index(column, row + 1) < module_count) next = to_index(column, row + 1); break;
    }
    if (next != current && next < module_count) cursor_index = next;

    /* module swap: HJKL moves the module, cursor follows */
    int swap_target = -1;
    switch (key) {
        case 'H': if (column > 0)               swap_target = to_index(column - 1, row); break;
        case 'L': if (column < GRID_COLUMNS - 1) swap_target = to_index(column + 1, row); break;
        case 'K': if (row > 0)                   swap_target = to_index(column, row - 1); break;
        case 'J': if (to_index(column, row + 1) < module_count) swap_target = to_index(column, row + 1); break;
    }
    if (swap_target >= 0) {
        rack_swap_modules(rack, current, swap_target);
        cursor_index = swap_target;
    }

    /* enter dives into the focused module's fields */
    if ((key == '\n' || key == KEY_ENTER) && rack->modules[current].field_count > 0)
        selected_field = 0;

    if (key == 'q') return 1;
    return 0;
}
