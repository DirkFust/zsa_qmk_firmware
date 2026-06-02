# TODO — Bugs & Tech Debt

Known limitations and standing tech debt for this keymap.

## Bugs (correctness)

### Known limitation: cross-hand rolls between dynMT and another key

Same-hand rolls on SYM_NUM (e.g. `HR_4 → HR_5` typing `45`) are now handled by chordal-hold. Cross-hand rolls within `TAPPING_TERM` still resolve as hold — e.g. `HR_EQL → 7` produces `ctrl+7` / `cmd+7` instead of `=7`. This matches how the regular `MT()` homerow mods on BASE/MAC behave (your `CHORDAL_HOLD` config defers cross-hand to the timer), so it's a consistent constraint rather than a bug.

Mitigation if it bothers you: type slower past `TAPPING_TERM` (200ms), or mark specific dynMT keys as `'*'` in `chordal_hold_layout` so any roll resolves as tap (breaks intentional cross-hand chording with that key).
