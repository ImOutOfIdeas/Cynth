## Setup

```bash
curl -O https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h
make
```

## Project layout

```
miniaudio.h         single-file audio library (fetched separately)
main.c              patch setup: add modules, wire them, run the loop

rack/               all audio state — one include from main: rack/rack.h
  rack.h            public boundary: types, API declarations, pulls in everything below
  rack.c            rack lifecycle and patching
  module.h          builder API for authoring new modules
  module_registry.h table of spawnable module types used by the add-module menu
  modules/
    oscillator.h
    gain.h
    noise.h
    output.h

ui/                 all rendering and input — one include from main: ui/ui.h
  ui.h              public boundary: UiMode, extern state, pulls in everything below
  ui.c              ncurses init/free, draw loop, global ui state
  input.c           keyboard routing through ui modes
  grid.h            module grid rendering
  overlay.h         reusable bordered popup primitive
  status_bar.h      bottom status strip
  device_picker.h   audio device picker overlay
  add_module.h      module-add overlay (reads from rack/module_registry.h)
```

## Adding a new module type

1. Create `rack/modules/my_module.h` following the pattern of the existing modules
2. `#include` it in `rack/rack.h`
3. Add an entry to the `module_registry[]` table in `rack/module_registry.h`

That's it. The add-module menu picks it up automatically.

## Patching in main.c

```c
Module *osc = rack_add_module(&rack, oscillator_init(&rack, "VCO", ma_waveform_type_sine, 0.5f, 440.0f));
Module *out = rack_add_module(&rack, output_init(&rack, "OUTPUT", 0.8f));
rack_connect(osc, 0, out, 0);
```

## Controls

| Key       | Action                        |
|-----------|-------------------------------|
| h j k l   | navigate modules              |
| H J K L   | swap module with neighbour    |
| Enter     | enter field edit mode         |
| j / k     | (field mode) next/prev field  |
| h / l     | (field mode) adjust value     |
| Escape    | exit field edit mode          |
| a         | open add-module menu          |
| d         | open device picker            |
| q         | quit                          |
