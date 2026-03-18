#pragma once
#include "ui.h"

static inline void status_bar_draw(Rack *rack, UiMode mode, int cursor) {
    int terminal_rows, terminal_cols;
    getmaxyx(stdscr, terminal_rows, terminal_cols);

    WINDOW *bar = newwin(1, terminal_cols, terminal_rows - 1, 0);
    if (!bar) return;

    wbkgd(bar, COLOR_PAIR(1));
    wattron(bar, COLOR_PAIR(1) | A_BOLD);

    const char *mode_label = "NORMAL";
    const char *hints      = "hjkl:nav  HJKL:swap  Enter:edit  a:add  d:devices  q:quit";

    if (mode == UI_MODE_FIELD_EDIT) {
        mode_label = "EDIT";
        hints      = "jk:field  hl:adjust  Esc:back";
    } else if (mode == UI_MODE_DEVICE_PICKER) {
        mode_label = "DEVICES";
        hints      = "jk:navigate  Enter:select  Esc:back";
    } else if (mode == UI_MODE_ADD_MODULE) {
        mode_label = "ADD";
        hints      = "jk:navigate  Enter:add  Esc:back";
    }

    char left[64] = {0};
    if (cursor >= 0 && cursor < rack->module_count)
        snprintf(left, sizeof(left), " [%s]  %s", mode_label, rack->modules[cursor].label);
    else
        snprintf(left, sizeof(left), " [%s]", mode_label);

    mvwprintw(bar, 0, 0, "%-*s", terminal_cols, left);

    int hint_col = terminal_cols - (int)strlen(hints) - 1;
    if (hint_col > (int)strlen(left) + 1)
        mvwprintw(bar, 0, hint_col, "%s", hints);

    wattroff(bar, COLOR_PAIR(1) | A_BOLD);
    wnoutrefresh(bar);
    delwin(bar);
}
