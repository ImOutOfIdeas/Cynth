#include "input.h"
#include "ui.h"

static int toIndex(int column, int row) { return row * GRID_COLUMNS + column; }
static int columnOf(int index)          { return index % GRID_COLUMNS; }
static int rowOf(int index)             { return index / GRID_COLUMNS; }

int inputHandle(int key, Rack *rack) {
    int moduleCount = rack->moduleCount;
    int current     = cursorIndex;
    int column      = columnOf(current);
    int row         = rowOf(current);

    /* field mode: jk move between fields, hl adjust value, escape exits */
    if (selectedField >= 0) {
        Module *module = &rack->modules[current];
        switch (key) {
            case 'j': case KEY_DOWN:
                if (selectedField < module->fieldCount - 1) selectedField++;
                return 0;
            case 'k': case KEY_UP:
                if (selectedField > 0) selectedField--;
                return 0;
            case 'l': case KEY_RIGHT:
                fieldAdjust(&module->fields[selectedField], module->data, module->fields->step);
                return 0;
            case 'h': case KEY_LEFT:
                fieldAdjust(&module->fields[selectedField], module->data, -1 * module->fields->step);
                return 0;
            case 27:
                selectedField = -1;
                return 0;
        }
        return 0;
    }

    /* module navigation: hjkl */
    int next = current;
    switch (key) {
        case 'h': if (column > 0)                       next = toIndex(column - 1, row); break;
        case 'l': if (column < GRID_COLUMNS - 1)        next = toIndex(column + 1, row); break;
        case 'k': if (row > 0)                          next = toIndex(column, row - 1); break;
        case 'j': if (toIndex(column, row + 1) < moduleCount) next = toIndex(column, row + 1); break;
    }
    if (next != current && next < moduleCount) cursorIndex = next;

    /* module swap: HJKL moves module, cursor follows */
    int swapTarget = -1;
    switch (key) {
        case 'H': if (column > 0)              swapTarget = toIndex(column - 1, row); break;
        case 'L': if (column < GRID_COLUMNS-1) swapTarget = toIndex(column + 1, row); break;
        case 'K': if (row > 0)                 swapTarget = toIndex(column, row - 1); break;
        case 'J': if (toIndex(column, row+1) < moduleCount) swapTarget = toIndex(column, row + 1); break;
    }
    if (swapTarget >= 0) {
        rackSwapModules(rack, current, swapTarget);
        cursorIndex = swapTarget;
    }

    /* enter dives into the focused module's fields */
    if ((key == '\n' || key == KEY_ENTER) && rack->modules[current].fieldCount > 0)
        selectedField = 0;

    if (key == 'q') return 1;
    return 0;
}
