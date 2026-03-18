#pragma once

int    cursor_index   = 0;
int    selected_field = -1;
UiMode ui_mode        = UI_MODE_NORMAL;

static DevicePicker  device_picker  = {0};
static AddModuleMenu add_module_menu = {0};

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
    grid_free();
    endwin();
}

void ui_draw(Rack *rack) {
    grid_draw(rack);

    if (ui_mode == UI_MODE_DEVICE_PICKER)
        device_picker_draw(&device_picker);
    else if (ui_mode == UI_MODE_ADD_MODULE)
        add_module_draw(&add_module_menu);

    status_bar_draw(rack, ui_mode, cursor_index);
    doupdate();
}

int ui_poll_key(void) {
    return getch();
}
