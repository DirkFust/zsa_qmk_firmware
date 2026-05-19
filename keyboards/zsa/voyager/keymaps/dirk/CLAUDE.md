# Dirk's Voyager Keymap — Orientation

QMK keymap for the ZSA Voyager (split ortholinear, 52 keys + 2 thumb clusters, with optional Navigator trackball). Built to drive both macOS and Windows from the same physical keyboard, with US-International base layout plus German umlauts.

## Files

| File | Purpose |
|------|---------|
| `keymap.c` | Layers, custom keycodes, `process_record_user`, layer indicator LEDs, OS detection, combos |
| `config.h` | Tap/hold timings, combo settings, trackball settings, RGB matrix effect exclusions |
| `rules.mk` | QMK feature toggles, additional source files |
| `dynamic_mt.{c,h}` | OS-aware mod-tap system (see below) |
| `tap_hold_helper.c` | Utility wrappers: `win_or_mac`, `key_and_shift`, `tap_or_hold`, `controlify_on_hold` |
| `color_helper.c` | RGB matrix HSV/effect callbacks for `defer_exec` |
| `rgb_matrix_user.inc` | Custom RGB matrix effects (e.g. `overwatch`) |
| `keymap.json` | Oryx export — source of truth for the Keymapp visualization, not the firmware |
| `voyager_dirk.bin` | **Compiled artifact, should not be tracked** |

## Layers

```
0  BASE      — Windows-oriented homerow mods (LGUI on A, LSFT on F, etc.)
1  MAC       — Overlay swapping GUI↔CTRL for Mac homerow mods
2  UMLAUT    — ä ö ü € ° § etc. Held while TG_UML is down.
3  SYM_NUM   — Symbols + numpad with dynamic_mt homerow mods
4  MOVEMENT  — Arrows, home/end, page up/down, OS toggle
5  MOUSE     — Trackball buttons, CPI, drag-scroll (auto-activated by trackball motion)
6  GAMING    — Reached via TO(GAMING) from MOVEMENT
7  FUNCTION  — F-keys, reached via one-shot combo (BSPC+TAB thumb keys)
```

`AUTO_MOUSE_DEFAULT_LAYER 5` couples trackball motion to the MOUSE layer.

## Dual-OS architecture

`is_mac` (global bool in `keymap.c`) is the source of truth. It's set by:

1. `detect_os_callback` — fires once at startup via `defer_exec(500, ...)` and inspects `detected_host_os()`. Linux → Win, iOS → Mac, Unsure → Mac.
2. `OS_TOGGLE` custom keycode — flips it manually and calls `layer_move(MAC|BASE)`.

Three mechanisms react to `is_mac`:

- **Static MT() pairs** — `WIN_HR_*` vs `MAC_HR_*` macros. The MAC layer overlays just the homerow keys, swapping which physical key holds GUI vs CTRL.
- **`dynamic_mt` system** — for the SYM_NUM homerow, where MT() can't help because the keycode itself differs by OS. See below.
- **`win_or_mac(...)` helper** — for custom keycodes like `CU_HOME`, `CU_PSCR`, umlauts. Picks one of two keycodes based on `is_mac`; optional mod-stripping.

## `dynamic_mt` (the custom mod-tap)

A mod-tap variant where the *modifier* differs between Mac and Win, while the tap action is the same. Used on the SYM_NUM homerow (`HR_EQL`, `HR_QUES`, `HR_4`, etc.).

- States are declared via `DEFINE_DYNAMIC_MT_SIMPLE(kc, mac_mod, win_mod, tap_key)` or `..._CUSTOM(kc, mac_mod, win_mod, tap_func)` in `mt_states[]`.
- `init_dynamic_mt_states()` registers them at boot.
- `process_dynamic_mt()` is called first in `process_record_user`.

**Important caveat (already in keymap.c comment):** dynamic_mt does NOT participate in `CHORDAL_HOLD` and uses a "register mod on press, suppress tap on any interruption" model — closer to hold-preferred than to QMK's `PERMISSIVE_HOLD`. It mixes badly with regular `MT()` on the same layer, which is why it's confined to SYM_NUM.

## Other behavioral details

- `caps_word_press_user` is extended to continue Caps Word through `US_?DIA` (umlauts), `KC_MINS`, `TG_UML`, `US_SS` (ß), and the BSPC/LEFT/RIGHT layer-tap keys on MOVEMENT.
- `chordal_hold_layout` marks thumb keys as `*` (no hand) so layer-tap on a thumb + same-hand symbol like `(` works without blocking.
- `HR_QUOT` uses a custom tap handler because it sends `"` unshifted and `'` shifted (reversed from US standard) — plus a trailing space to escape the US-International dead-key behavior.
- `TG_UML` activates the UMLAUT layer **on press** and deactivates it on the **next non-TG_UML key release** (held-layer semantics via custom handler, not standard `MO`/`LT`).
- `OSL(FUNCTION)` is triggered by a combo of the two thumb-keys (`LT(MOVEMENT, KC_BSPC)` + `LT(SYM_NUM, KC_TAB)`), declared in `key_combos[]`. Stays active through chained modifier presses for shortcuts like `Ctrl+Shift+F5`.
- LED 1 = "above MAC layer active". LED 2 = "Windows mode" (off = Mac). LED 3 = Caps Word active. LED 4 unused.

## Conventions

- The padding macros (`____`–`____________`, `XXXX`–`XXXXXXXXXXXXXXXXXX`) exist purely to align the LAYOUT_voyager grid. All `___*` map to `KC_TRANSPARENT`, all `XXX*` to `KC_NO`. Typos in width silently still compile.
- `// clang-format off` at the top of `keymap.c` — don't auto-format the grid.
- `#ifdef CONSOLE_ENABLE` guards all `uprintf` / `print` calls. `CONSOLE_ENABLE = no` in `rules.mk` by default.

## Editing rules

- **Surgical edits only.** Don't reformat or reorder the LAYOUT_voyager grids. Don't rewrite whole files.
- **Don't add features beyond what the task asks for.** The user prefers minimal diffs.
- **Per-OS behavior** belongs in the existing dispatch points (`is_mac` branch, `win_or_mac`, or a new `dynamic_mt` entry) — don't introduce a fourth mechanism.
- **New custom keycodes** go in the `enum custom_keycodes` block (keymap.c:76) AND get a case in `handle_custom_keycodes` (keymap.c:493). Both, or it silently no-ops.
- **New combos** must be added to the `combos` enum BEFORE `COMBO_LENGTH` (the "nifty trick" auto-sizes `COMBO_LEN`).
- **`config.h` is the right place** for QMK feature `#define`s, not `keymap.c`. (Two are currently misplaced — see TODO.md.)
- **Never commit `voyager_dirk.bin`.** It's a build artifact.
- **Commits:** no mention of Claude/AI. Conventional commits (`fix:`, `feat:`, `refactor:`).

## Known bugs and tech debt

See `TODO.md` in this directory.
