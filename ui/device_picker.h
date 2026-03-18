#pragma once
#include "ui.h"
#include "overlay.h"

#define DEVICE_PICKER_MAX    32
#define DEVICE_PICKER_HEIGHT 16
#define DEVICE_PICKER_WIDTH  52

typedef struct {
    ma_device_info devices[DEVICE_PICKER_MAX];
    int            device_count;
    int            selected_index;
} DevicePicker;

/* Query the system for playback devices and populate the picker. */
static inline void device_picker_populate(DevicePicker *picker) {
    picker->device_count = 0;

    ma_context context;
    if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) return;

    ma_device_info *playback_devices;
    ma_uint32       playback_count;
    if (ma_context_get_devices(&context, &playback_devices, &playback_count,
                               NULL, NULL) == MA_SUCCESS) {
        picker->device_count = (int)playback_count;
        if (picker->device_count > DEVICE_PICKER_MAX)
            picker->device_count = DEVICE_PICKER_MAX;
        for (int i = 0; i < picker->device_count; i++)
            picker->devices[i] = playback_devices[i];
    }

    ma_context_uninit(&context);
}

static inline void device_picker_draw(DevicePicker *picker) {
    WINDOW *window = overlay_create(DEVICE_PICKER_HEIGHT, DEVICE_PICKER_WIDTH);
    if (!window) return;
    overlay_set_title(window, "devices");

    int visible_rows  = DEVICE_PICKER_HEIGHT - 2;
    int scroll_offset = 0;
    if (picker->selected_index >= visible_rows)
        scroll_offset = picker->selected_index - visible_rows + 1;

    for (int row = 0; row < visible_rows; row++) {
        int index = row + scroll_offset;
        if (index >= picker->device_count) break;
        overlay_draw_row(window, row + 1,
                         picker->devices[index].name,
                         index == picker->selected_index);
    }

    if (picker->device_count == 0)
        mvwprintw(window, 1, 1, "no devices found");

    wnoutrefresh(window);
    delwin(window);
}

/* Returns 1 if the user pressed Enter to confirm a selection. */
static inline int device_picker_handle_key(DevicePicker *picker, int key) {
    if (key == 'j' || key == KEY_DOWN) {
        if (picker->selected_index < picker->device_count - 1)
            picker->selected_index++;
    } else if (key == 'k' || key == KEY_UP) {
        if (picker->selected_index > 0)
            picker->selected_index--;
    } else if (key == '\n' || key == KEY_ENTER) {
        return 1;
    }
    return 0;
}
