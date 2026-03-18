#pragma once
#include "ui.h"

/* Create a bordered popup window centred on the terminal. */
static inline WINDOW *overlay_create(int height, int width) {
    int terminal_rows, terminal_cols;
    getmaxyx(stdscr, terminal_rows, terminal_cols);
    int start_row = (terminal_rows - height) / 2;
    int start_col = (terminal_cols - width)  / 2;
    WINDOW *window = newwin(height, width, start_row, start_col);
    if (!window) return NULL;
    box(window, 0, 0);
    return window;
}

/* Stamp a title in caps over the top-left of the border, e.g. " DEVICES ". */
static inline void overlay_set_title(WINDOW *window, const char *title) {
    char upper[64] = {0};
    for (int i = 0; title[i] && i < (int)sizeof(upper) - 3; i++)
        upper[i] = (char)toupper((unsigned char)title[i]);

    char padded[72];
    snprintf(padded, sizeof(padded), " %s ", upper);

    wattron(window, A_BOLD);
    mvwprintw(window, 0, 1, "%s", padded);
    wattroff(window, A_BOLD);
}

/* Draw one menu row, highlighted when selected. */
static inline void overlay_draw_row(WINDOW *window, int row,
                                    const char *text, int selected) {
    int height, width;
    getmaxyx(window, height, width);
    (void)height;
    if (selected) wattron(window, A_REVERSE);
    mvwprintw(window, row, 1, "%-*.*s", width - 2, width - 2, text);
    if (selected) wattroff(window, A_REVERSE);
}
