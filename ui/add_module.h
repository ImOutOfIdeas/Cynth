#pragma once

#define ADD_MODULE_HEIGHT 16
#define ADD_MODULE_WIDTH  40

typedef struct {
    int selected_index;
} AddModuleMenu;

static inline void add_module_draw(AddModuleMenu *menu) {
    WINDOW *window = overlay_create(ADD_MODULE_HEIGHT, ADD_MODULE_WIDTH);
    if (!window) return;
    overlay_set_title(window, "add module");

    int visible_rows  = ADD_MODULE_HEIGHT - 2;
    int scroll_offset = 0;
    if (menu->selected_index >= visible_rows)
        scroll_offset = menu->selected_index - visible_rows + 1;

    for (int row = 0; row < visible_rows; row++) {
        int index = row + scroll_offset;
        if (index >= module_registry_count) break;
        overlay_draw_row(window, row + 1,
                         module_registry[index].name,
                         index == menu->selected_index);
    }

    wnoutrefresh(window);
    delwin(window);
}

/* Returns 1 if the user pressed Enter to confirm. */
static inline int add_module_handle_key(AddModuleMenu *menu, int key) {
    if (key == 'j' || key == KEY_DOWN) {
        if (menu->selected_index < module_registry_count - 1)
            menu->selected_index++;
    } else if (key == 'k' || key == KEY_UP) {
        if (menu->selected_index > 0)
            menu->selected_index--;
    } else if (key == '\n' || key == KEY_ENTER) {
        return 1;
    }
    return 0;
}
