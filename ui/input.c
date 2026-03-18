#pragma once

/* ui state defined in ui.c */
extern DevicePicker  device_picker;
extern AddModuleMenu add_module_menu;

static int to_index(int col, int row) { return row * GRID_COLUMNS + col; }
static int col_of(int index)          { return index % GRID_COLUMNS; }
static int row_of(int index)          { return index / GRID_COLUMNS; }

int ui_handle_key(int key, Rack *rack) {

    /* ── device picker ───────────────────────────────────────────────────── */
    if (ui_mode == UI_MODE_DEVICE_PICKER) {
        if (key == 27) { ui_mode = UI_MODE_NORMAL; return 0; }
        device_picker_handle_key(&device_picker, key);
        return 0;
    }

    /* ── add module ──────────────────────────────────────────────────────── */
    if (ui_mode == UI_MODE_ADD_MODULE) {
        if (key == 27) { ui_mode = UI_MODE_NORMAL; return 0; }
        if (add_module_handle_key(&add_module_menu, key))  {
            registry_create(add_module_menu.selected_index, rack);
            ui_mode = UI_MODE_NORMAL;
        }
        return 0;
    }

    /* ── field edit ──────────────────────────────────────────────────────── */
    if (ui_mode == UI_MODE_FIELD_EDIT) {
        Module *module = &rack->modules[cursor_index];
        if (key == 27) {
            ui_mode = UI_MODE_NORMAL;
            selected_field = -1;
        } else if (key == 'j' || key == KEY_DOWN) {
            if (selected_field < module->field_count - 1) selected_field++;
        } else if (key == 'k' || key == KEY_UP) {
            if (selected_field > 0) selected_field--;
        } else if (key == 'l' || key == KEY_RIGHT) {
            field_step(&module->fields[selected_field], module->data, +1);
        } else if (key == 'h' || key == KEY_LEFT) {
            field_step(&module->fields[selected_field], module->data, -1);
        }
        return 0;
    }

    /* ── normal mode ─────────────────────────────────────────────────────── */
    int count  = rack->module_count;
    int index  = cursor_index;
    int col    = col_of(index);
    int row    = row_of(index);

    /* navigate */
    int next = index;
    if      ((key == 'h') && col > 0)                              next = to_index(col - 1, row);
    else if ((key == 'l') && col < GRID_COLUMNS - 1)              next = to_index(col + 1, row);
    else if ((key == 'k') && row > 0)                              next = to_index(col, row - 1);
    else if ((key == 'j') && to_index(col, row + 1) < count)      next = to_index(col, row + 1);
    if (next != index && next < count) cursor_index = next;

    /* swap */
    int swap = -1;
    if      ((key == 'H') && col > 0)                              swap = to_index(col - 1, row);
    else if ((key == 'L') && col < GRID_COLUMNS - 1)              swap = to_index(col + 1, row);
    else if ((key == 'K') && row > 0)                              swap = to_index(col, row - 1);
    else if ((key == 'J') && to_index(col, row + 1) < count)      swap = to_index(col, row + 1);
    if (swap >= 0) { rack_swap_modules(rack, index, swap); cursor_index = swap; }

    /* enter field edit */
    if ((key == '\n' || key == KEY_ENTER) && rack->modules[cursor_index].field_count > 0) {
        ui_mode        = UI_MODE_FIELD_EDIT;
        selected_field = 0;
    }

    if (key == 'd') {
        device_picker.selected_index = 0;
        device_picker_populate(&device_picker);
        ui_mode = UI_MODE_DEVICE_PICKER;
    }

    if (key == 'a') {
        add_module_menu.selected_index = 0;
        ui_mode = UI_MODE_ADD_MODULE;
    }

    if (key == 'q') return 1;
    return 0;
}
