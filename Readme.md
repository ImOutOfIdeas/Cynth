## Setup

```bash
curl -O https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h
```

```bash
make
```

## Project layout

```
rack.h          types: Rack, Module, Field, Port
rack.c          rack lifecycle and patching
module.h        builder API for authoring new modules
ui.h / ui.c     ncurses rendering
input.h / input.c  keyboard handling
modules/
  oscillator.h
  gain.h
  noise.h
  output.h
main.c          patch setup - add modules and wire them here
```

## Writing a new module

1. Create `modules/my_module.h`
2. Define a `MyModuleData` struct holding your miniaudio node(s)
3. Write `static void` setters that cast `void *data` to your struct
4. Write `static void my_module_cleanup(Module *)` that uninits nodes and frees data
5. Write `static inline Module my_module_init(Rack *, ...)` following this template:

```c
static inline Module my_module_init(Rack *rack, const char *label, ...) {
    Module module = {0};
    MyModuleData *data = calloc(1, sizeof(MyModuleData));
    if (!data) return module;

    /* init your miniaudio node(s) here, return module on failure */

    snprintf(module.label, MAX_NAME, "%s", label);
    module.data    = data;
    module.node    = (ma_node *)&data->node;
    module.cleanup = my_module_cleanup;

    module_add_value_field (&module, "gain",  1.0f, 0.0f, 2.0f, 0.01f, my_set_gain);
    module_add_select_field(&module, "mode",  0, 3, mode_options, my_set_mode);
    module_add_port        (&module, "in",  PORT_IN,  0);
    module_add_port        (&module, "out", PORT_OUT, 0);

    return module;
}
```

6. `#include "modules/my_module.h"` in `main.c`
7. Call `rack_add_module(&rack, my_module_init(&rack, "MY MODULE", ...))` and wire with `rack_connect`

## Controls

| Key       | Action                        |
|-----------|-------------------------------|
| h j k l   | navigate modules              |
| H J K L   | swap module with neighbour    |
| Enter     | enter field edit mode         |
| j / k     | (field mode) next/prev field  |
| h / l     | (field mode) adjust value     |
| Escape    | exit field edit mode          |
| q         | quit                          |
