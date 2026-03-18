#pragma once
#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include <locale.h>
#include "rack.h"

#define MODULE_WIDTH   24
#define MODULE_HEIGHT  16
#define GRID_COLUMNS    4

extern int cursor_index;
extern int selected_field;

void ui_init(void);
void ui_free(void);
void ui_draw(Rack *rack);
int  ui_poll_key(void);
