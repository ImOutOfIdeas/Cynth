#include "ui.h"
#include <stdlib.h>
#include <string.h>

int cursorIndex  = 0;
int selectedField = -1;

static WINDOW **windows    = NULL;
static int      windowCount = 0;

void uiInit(void) {
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

void uiFree(void) {
    for (int i = 0; i < windowCount; i++)
        if (windows[i]) delwin(windows[i]);
    free(windows);
    windows     = NULL;
    windowCount = 0;
    endwin();
}

/* recreate windows only when module count changes */
static void syncWindows(const Rack *rack) {
    if (windowCount == rack->moduleCount) return;
    for (int i = 0; i < windowCount; i++)
        if (windows[i]) delwin(windows[i]);
    free(windows);
    windowCount = rack->moduleCount;
    windows     = calloc(windowCount, sizeof(WINDOW *));
    for (int i = 0; i < windowCount; i++) {
        int column = i % GRID_COLUMNS;
        int row    = i / GRID_COLUMNS;
        windows[i] = newwin(MODULE_HEIGHT, MODULE_WIDTH,
                            row * MODULE_HEIGHT, column * MODULE_WIDTH);
    }
}

static void drawModule(WINDOW *window, Module *module, int focused) {
    int innerWidth = MODULE_WIDTH - 2;

    werase(window);

    if (focused) wattron(window, COLOR_PAIR(1) | A_BOLD);
    box(window, 0, 0);

    /* label sits on top border */
    char labelBuffer[MAX_NAME + 2];
    snprintf(labelBuffer, sizeof(labelBuffer), " %s ", module->label);
    mvwprintw(window, 0, 1, "%-.*s", innerWidth, labelBuffer);
    if (focused) wattroff(window, COLOR_PAIR(1) | A_BOLD);

    /* fields */
    for (int i = 0; i < module->fieldCount; i++) {
        Field *field = &module->fields[i];
        fieldRefreshDisplay(field);

        int fieldRow  = 1 + i;
        if (fieldRow >= MODULE_HEIGHT - 2) break;

        int highlighted = focused && (selectedField == i);
        if (highlighted) wattron(window, A_REVERSE);

        /* name left, value right */
        int nameLength  = (int)strlen(field->name);
        int valueLength = (int)strlen(field->displayValue);
        int gap         = innerWidth - nameLength - valueLength;
        if (gap < 1) gap = 1;
        mvwprintw(window, fieldRow, 1, "%s%*s%s",
                  field->name, gap, "", field->displayValue);

        if (highlighted) wattroff(window, A_REVERSE);
    }

    /* ports: inputs on left, outputs on right of bottom row */
    char inputPorts[MODULE_WIDTH]  = {0};
    char outputPorts[MODULE_WIDTH] = {0};
    for (int i = 0; i < module->portCount; i++) {
        Port *port = &module->ports[i];
        char *target = port->direction == PORT_IN ? inputPorts : outputPorts;
        int   space  = port->direction == PORT_IN
                       ? (int)sizeof(inputPorts) : (int)sizeof(outputPorts);
        if (target[0]) strncat(target, " ", space - strlen(target) - 1);
        strncat(target, port->name, space - strlen(target) - 1);
    }
    if (inputPorts[0] || outputPorts[0]) {
        int gap = innerWidth - (int)strlen(inputPorts) - (int)strlen(outputPorts);
        if (gap < 0) gap = 0;
        mvwprintw(window, MODULE_HEIGHT - 2, 1, "%s%*s%s",
                  inputPorts, gap, "", outputPorts);
    }

    wnoutrefresh(window);
}

void uiDraw(Rack *rack) {
    syncWindows(rack);
    for (int i = 0; i < rack->moduleCount; i++)
        drawModule(windows[i], &rack->modules[i], i == cursorIndex);
    doupdate();
}

int uiPollKey(void) {
    return getch();
}
