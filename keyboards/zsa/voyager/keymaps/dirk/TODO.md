# TODO — Bugs & Tech Debt

Ordered by severity. Line numbers refer to state as of this writing; re-check before editing.

## Bugs (correctness)

### Known limitation: cross-hand rolls between dynMT and another key

Same-hand rolls on SYM_NUM (e.g. `HR_4 → HR_5` typing `45`) are now handled by chordal-hold. Cross-hand rolls within `TAPPING_TERM` still resolve as hold — e.g. `HR_EQL → 7` produces `ctrl+7` / `cmd+7` instead of `=7`. This matches how the regular `MT()` homerow mods on BASE/MAC behave (your `CHORDAL_HOLD` config defers cross-hand to the timer), so it's a consistent constraint rather than a bug.

Mitigation if it bothers you: type slower past `TAPPING_TERM` (200ms), or mark specific dynMT keys as `'*'` in `chordal_hold_layout` so any roll resolves as tap (breaks intentional cross-hand chording with that key).

## Code organization

### 1. `.c` files included as headers — `keymap.c:6–7`

```c
#include "tap_hold_helper.c"
#include "color_helper.c"
```

`dynamic_mt` does this correctly: header in `dynamic_mt.h`, implementation in `dynamic_mt.c`, compiled via `SRC += dynamic_mt.c` in `rules.mk`. Do the same for the other two: create `.h` files, move the implementations to their own translation units.

### 2. Two `#define`s misplaced in `keymap.c` — lines 11–12

```c
#define OS_DETECTION_KEYBOARD_RESET
#define OS_DETECTION_DEBOUNCE 250
```

These are QMK-core feature flags. They need to be visible during QMK's own compilation, not only in the keymap TU. **Move both to `config.h`.**

## Dead code / cleanup

### 3. Unused functions in `tap_hold_helper.c`
- `key_and_shift` — no callers
- `controlify_on_hold` — no callers
- `tap_or_hold` — no callers

`win_or_mac` is the only one used. Delete the rest.

### 4. Unused callbacks in `color_helper.c`
`change_color_red_callback`, `change_color_green_callback`, `change_color_blue_callback`, `set_color` — none referenced. Only `change_led_effect_heatmap_callback` is used. Delete the others.

### 5. Redundant `current_layer` tracking — `keymap.c:70`
`get_highest_layer(layer_state)` provides this directly. The static mirror at line 70 invites desync bugs and adds nothing.

### 6. Padding-macro zoo — `keymap.c:19–41`
14 width variants of `XXX*` and 8 of `___*`. Cosmetic alignment only. A typo in underscore/X count silently compiles to the wrong padding (all map to KC_TRANSPARENT/KC_NO). Standard QMK uses one width each. Consider dropping all but `_______` and `XXXXXXX`.

## Verify before editing

### 7. `is_mouse_record_kb` vs `is_mouse_record_user` — `keymap.c:261`
This function uses the `_kb` suffix from a keymap. The standard keymap-level hook is `is_mouse_record_user`. Verify QMK actually invokes `_kb` overrides defined in a keymap TU — if it doesn't, auto-mouse-layer won't recognise the trackball buttons / drag-scroll as mouse activity, so the MOUSE layer may deactivate while the trackball is in active use. Easy test: run with `CONSOLE_ENABLE = yes` and watch the layer transitions.
