#pragma once
#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include <locale.h>
#include "rack.h"

#define MODULE_WIDTH  24
#define MODULE_HEIGHT 16
#define GRID_COLUMNS   4

extern int cursorIndex;     /* which module is focused */
extern int selectedField;   /* -1 = no field selected, >= 0 = field index */

void uiInit(void);
void uiFree(void);
void uiDraw(Rack *rack);
int  uiPollKey(void);
