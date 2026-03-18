#pragma once
#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include <locale.h>
#include <ctype.h>
#include "../rack/rack.h"

#define MODULE_WIDTH   24
#define MODULE_HEIGHT  16
#define GRID_COLUMNS    4

typedef enum {
    UI_MODE_NORMAL,
    UI_MODE_FIELD_EDIT,
    UI_MODE_DEVICE_PICKER,
    UI_MODE_ADD_MODULE,
} UiMode;

extern int    cursor_index;
extern int    selected_field;
extern UiMode ui_mode;

void ui_init(void);
void ui_free(void);
void ui_draw(Rack *rack);
int  ui_poll_key(void);
int  ui_handle_key(int key, Rack *rack);

#include "overlay.h"
#include "status_bar.h"
#include "device_picker.h"
#include "add_module.h"
#include "grid.h"
#include "input.h"
#include "ui.c"
#include "input.c"
