# Dirk's Voyager Keymap — Orientation

QMK keymap for the ZSA Voyager (split ortholinear, 52 keys + 2 thumb clusters, with the Navigator trackpad). Built to drive both macOS and Windows from the same physical keyboard, with US-International base layout plus German umlauts.

## Branches & repo layout

This fork (`origin` = `github.com/DirkFust/zsa_qmk_firmware`) mirrors ZSA's repo (`upstream` = `github.com/zsa/qmk_firmware`) with one branch pair per firmware generation:

- **`firmwareX`** (`firmware24`, `firmware25`, `firmware26`, …) — tracks the **ZSA upstream** branch of that generation, verbatim. **No personal changes ever land here.** Update by pulling/resetting onto `upstream/firmwareX` (e.g. `git fetch upstream && git branch -f firmware25 upstream/firmware25`).
- **`dirk-firmwareX`** — all of my work (this keymap + any module overrides), **rebased on top of `firmwareX`**. This is the branch I actually develop and flash from.

**Workflow rules:**

- Do all keymap work on `dirk-firmwareX`. Never commit personal changes to `firmwareX`.
- When upstream ships a new generation, the upstream `branchX` is brought into `firmwareX`; I then **rebase `dirk-firmwareX` onto the updated `firmwareX`** (not merge) so my history stays linear on top of the original.
- Because `dirk-firmwareX` is rebased, pushing it to `origin` needs `git push --force-with-lease`. Same for `firmwareX` after it's reset to upstream.
- If personal commits ever end up on `firmwareX` by mistake: move them off (`git branch -f dirk-firmwareX firmwareX`), reset `firmwareX` back to `upstream/firmwareX`, then force-push both.

## Files

| File | Purpose |
|------|---------|
| `keymap.c` | Layers, custom keycodes, `process_record_user`, layer indicator LEDs, OS detection, combos |
| `config.h` | Tap/hold timings, combo settings, RGB matrix effect exclusions. Old `POINTING_DEVICE`/`AUTO_MOUSE` defines remain but are dormant (see Pointing device below) |
| `rules.mk` | QMK feature toggles, additional source files |
| `dynamic_mt.{c,h}` | OS-aware mod-tap system (see below) |
| `tap_hold_helper.{c,h}` | Utility wrapper: `win_or_mac` (picks a Win/Mac keycode, optional mod-strip) |
| `color_helper.c` | RGB matrix HSV/effect callbacks for `defer_exec` |
| `rgb_matrix_user.inc` | Custom RGB matrix effects (e.g. `overwatch`) |
| `keymap.json` | Declares the community `modules` compiled into the firmware (`zsa/oryx`, `zsa/defaults`, `zsa/navigator_trackpad`) — a real build input, not just an Oryx export |
| `voyager_dirk.bin` | Compiled artifact, tracked so old builds stay flashable from git history |

## Layers

```
0  BASE      — Windows-oriented homerow mods (LGUI on A, LSFT on F, etc.)
1  MAC       — Overlay swapping GUI↔CTRL for Mac homerow mods
2  UMLAUT    — ä ö ü € ° § etc. Held while TG_UML is down.
3  SYM_NUM   — Symbols + numpad with dynamic_mt homerow mods
4  MOVEMENT  — Arrows, home/end, page up/down, OS toggle
5  MOUSE     — Mouse buttons; CPI/drag-scroll handlers present but dormant (trackpad is a digitizer, POINTING_DEVICE off). No longer auto-activated.
6  GAMING    — Reached via TO(GAMING) from MOVEMENT
7  FUNCTION  — F-keys, reached via one-shot combo (BSPC+TAB thumb keys)
```

## Pointing device — trackpad (was trackball)

The keymap targets the **Navigator trackpad**. The trackpad is a **DIGITIZER / Windows Precision Touchpad (PTP)** device, *not* a QMK pointing device:

- Loaded via the `zsa/navigator_trackpad` community module in `keymap.json` (the module sets `DIGITIZER_ENABLE` + `DIGITIZER_MODE = touchpad`). `POINTING_DEVICE_ENABLE = no` in `rules.mk`.
- The **OS** drives cursor + gestures (Windows: native PTP; macOS: the module's own scroll/inertia paths plus a generic mouse fallback). So `AUTO_MOUSE`, drag-scroll, CPI and the old layer-5 auto-activation no longer apply.
- The trackball-coupled code in `keymap.c` (`pointing_device_*`, `is_mouse_record_kb`, the `DRAG_SCROLL`/`NAVIGATOR_*` cases) is kept but `#ifdef POINTING_DEVICE_ENABLE`-guarded out — dormant, ready to re-wire later. `DRAG_SCROLL`/`NAVIGATOR_*` keycodes now come from the `zsa/defaults` module, not a local enum.
- Trackpad options (rotation, physical size, I²C address, mouse-fallback sensitivity) live in `modules/zsa/navigator_trackpad/config.h` + `post_config.h`, each `#ifndef`-overridable from this keymap's `config.h`.

## Tap dance — G click-drag

`G` on BASE is a tap-dance key (`G_DRAG` = `TD(TD_G_DRAG)`, needs `TAP_DANCE_ENABLE = yes`): **tap = `g`, hold = hold the left mouse button** for click-drag with the trackpad. `MT()` can't express this — its hold is modifier-only. It's interrupt-friendly (`state->pressed && !state->interrupted`) so rolls still type `g`; the hold engages after `TAPPING_TERM`. Lives inline in `keymap.c` (`tap_dance_codes` enum, `g_drag_finished`/`g_drag_reset`, `tap_dance_actions[]`).

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

- `caps_word_press_user` is extended to continue Caps Word through `US_?DIA` (umlauts), `KC_MINS`, `TG_UML`, `US_SS` (ß), and `KC_BSPC`/`KC_LEFT`/`KC_RIGHT`. The latter three also cover the MOVEMENT layer-tap keys: `process_caps_word.c` unwraps a tapped layer-tap to its tap keycode before calling the hook (and ignores a held one outright), so listing `LT(...)` forms is dead code.
- `CAPS_WORD_INVERT_ON_SHIFT` (`config.h`) makes a **held shift invert** the shift of the next key instead of ending Caps Word. It exists because `KC_MINS` is shifted to `_` for SCREAMING_SNAKE_CASE, which would otherwise put a literal `-` out of reach: **left shift (F) + `-` types `-`** and the mode stays on. Use the *left* shift — `KC_MINS` is a right-hand key, so `J + -` is a same-hand chord that `CHORDAL_HOLD` settles as a tap. Two consequences: shift + letter now yields a lowercase letter mid-word, and while Caps Word is active the shift home-row mods emit no real shift at all (so Shift+Arrow selection is unavailable in the mode).
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
- **Tap dance** (`tap_dance_codes` enum + `tap_dance_actions[]`, inline in `keymap.c`) is a separate mechanism from `MT()`/`dynamic_mt`. Reach for it only when a hold must emit a non-modifier keycode (e.g. `G_DRAG` holds a mouse button).
- **New combos** must be added to the `combos` enum BEFORE `COMBO_LENGTH` (the "nifty trick" auto-sizes `COMBO_LEN`).
- **`config.h` is the right place** for QMK feature `#define`s, not `keymap.c`.
- **Commits:** no mention of Claude/AI. Conventional commits (`fix:`, `feat:`, `refactor:`).

## Known bugs and tech debt

See `TODO.md` in this directory.
