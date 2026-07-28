// Copyright 2026 Dirk (@DirkFust)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "dynamic_mt.h"

// Defined in keymap.c
extern const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS];

// Defined in quantum/quantum.c but not declared in any header; QMK's own
// process_key_override.c pulls it in the same way.
extern uint8_t extract_mod_bits(uint16_t code);

// Static variables to hold the configuration
static dynamic_mt_state_t *g_mt_states = NULL;
static size_t              g_mt_count  = 0;

// Initialize the dynamic mod-tap system
void init_dynamic_mt_states(dynamic_mt_state_t *states, size_t count) {
    g_mt_states = states;
    g_mt_count  = count;
}

// Find the state for a given keycode
dynamic_mt_state_t *find_mt_state(uint16_t keycode, dynamic_mt_state_t *states, size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (states[i].keycode == keycode) {
            return &states[i];
        }
    }
    return NULL; // Not found
}

// QUICK_TAP_TERM comes from quantum/action_tapping.h (via QMK_KEYBOARD_H); it
// defaults to TAPPING_TERM unless config.h overrides it.

// A key pressed again right after its own tap repeats that tap instead of acting
// as a modifier — the "tap, then press and hold to repeat" that QUICK_TAP_TERM
// gives regular mod-taps. Only keys with a plain tap keycode can be held down;
// a custom tap handler has nothing to keep pressed.
static bool quick_tap_repeats(dynamic_mt_state_t *state) {
    return state->tapped && state->simple_keycode && timer_elapsed(state->tap_time) < QUICK_TAP_TERM;
}

// Read the hand designation ('L' / 'R' / '*') for a matrix position.
// Off-matrix events (combos etc.) report as '*' so they bypass the hand check.
static char hand_at(uint8_t row, uint8_t col) {
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return '*';
    return pgm_read_byte(&chordal_hold_layout[row][col]);
}

// Modifiers held by keys whose hold/tap decision is still open. Those are always
// same-hand chord partners (an opposite-hand press resolves a key immediately),
// so they must not leak into a tap: rolling "4 5" has to type 45, not GUI+4.
static uint8_t undecided_mods(dynamic_mt_state_t *self, dynamic_mt_state_t *states, size_t count) {
    uint8_t mods = 0;
    for (size_t i = 0; i < count; i++) {
        dynamic_mt_state_t *state = &states[i];
        if (state == self || !state->timer || state->resolved) continue;
        mods |= MOD_BIT(is_mac ? state->mac_mod : state->win_mod);
    }
    return mods;
}

static void send_tap(dynamic_mt_state_t *state, dynamic_mt_state_t *states, size_t count) {
    uint8_t chord_mods = undecided_mods(state, states, count);
    del_mods(chord_mods);

    if (state->tap_handler) {
        state->tap_handler();
    } else if (state->simple_keycode) {
        tap_code16(state->simple_keycode);
    }

    add_mods(chord_mods);

    state->tapped   = true;
    state->tap_time = timer_read();
}

// Give up the hold and hand back the tap. Marked resolved so the key's own
// release stays silent and it stops counting as an active chord partner.
static void settle_as_tap(dynamic_mt_state_t *state, dynamic_mt_state_t *states, size_t count) {
    unregister_code(is_mac ? state->mac_mod : state->win_mod);
    state->resolved    = true;
    state->interrupted = true;
    send_tap(state, states, count);
}

// Settle every still-undecided key as a tap, oldest press first, so a rolled
// chord keeps its typing order no matter in which order the keys are released.
// With `before` given, only keys pressed earlier than that one are settled.
static void settle_undecided_taps(dynamic_mt_state_t *states, size_t count, dynamic_mt_state_t *before) {
    for (;;) {
        dynamic_mt_state_t *oldest = NULL;

        for (size_t i = 0; i < count; i++) {
            dynamic_mt_state_t *state = &states[i];
            if (state == before || !state->timer || state->resolved) continue;
            if (before && timer_elapsed(state->timer) <= timer_elapsed(before->timer)) continue; // pressed later
            if (!oldest || timer_elapsed(state->timer) > timer_elapsed(oldest->timer)) oldest = state;
        }

        if (!oldest) return;
        settle_as_tap(oldest, states, count);
    }
}

// Called on every key press. Decides hold-or-tap for any in-flight dynamic_mt:
//   - different hand / thumb / past term → commit as hold (mod stays registered)
//   - same hand within tapping term, interrupted by another dynamic_mt key
//                                       → leave the decision open: if both keys
//                                         are still held once the term runs out,
//                                         both count as mods. This mirrors QMK's
//                                         CHORDAL_HOLD, which only settles a chord
//                                         as tap when the interrupting key is a
//                                         regular key (see is_tap_record() in
//                                         quantum/action_tapping.c)
//   - same hand within tapping term, any other key → cancel the mod, emit the tap
//                                         *before* the interrupting key proceeds
void check_mt_interruptions(uint16_t keycode, keyrecord_t *record, dynamic_mt_state_t *states, size_t count) {
    if (!record->event.pressed) return;

    char other_hand = hand_at(record->event.key.row, record->event.key.col);

    // Expire the quick-tap arming. Same threshold quick_tap_repeats() uses, so this
    // changes no decision — it just stops `tapped` lingering indefinitely, which
    // timer_elapsed()'s ~65 s wraparound could otherwise fold back into the window.
    for (size_t i = 0; i < count; i++) {
        if (states[i].tapped && timer_elapsed(states[i].tap_time) >= QUICK_TAP_TERM) {
            states[i].tapped = false;
        }
    }

    // Commit everything that is not a same-hand chord as hold.
    for (size_t i = 0; i < count; i++) {
        dynamic_mt_state_t *state = &states[i];
        if (!state->timer || state->resolved || keycode == state->keycode) continue;

        char self_hand = hand_at(state->row, state->col);
        bool same_hand = (self_hand != '*' && other_hand != '*' && self_hand == other_hand);
        if (same_hand && timer_elapsed(state->timer) < TAPPING_TERM) continue; // handled below

        state->resolved    = true;
        state->interrupted = true; // suppress late tap on release
    }

    // What is left are same-hand chords. Another dynamic_mt key can still turn
    // them into mods, every other key settles them as taps right now — including
    // a dynamic_mt key that is only about to repeat its own tap.
    dynamic_mt_state_t *interrupter = find_mt_state(keycode, states, count);
    if (interrupter == NULL || quick_tap_repeats(interrupter)) {
        settle_undecided_taps(states, count, NULL);
    }
}

// Main function to call from process_record_user
bool process_dynamic_mt(uint16_t keycode, keyrecord_t *record) {
    if (!g_mt_states) {
        return true; // Not initialized
    }

    // Check for interruptions first
    check_mt_interruptions(keycode, record, g_mt_states, g_mt_count);

    // Find the state for this keycode
    dynamic_mt_state_t *state = find_mt_state(keycode, g_mt_states, g_mt_count);

    if (!state) {
        return true; // Not a dynamic MT key
    }

    uint16_t mod_key = is_mac ? state->mac_mod : state->win_mod;

    if (record->event.pressed) {
        // Key pressed
        if (quick_tap_repeats(state)) {
            state->repeating = true;
            state->tapped    = false;
            // Held down -> the OS repeats it. Registered as *real* mods rather than
            // via register_code16(): that uses weak mods, which QMK clears on every
            // key press, so a held "?" would lose its shift and repeat as "/".
            register_mods(extract_mod_bits(state->simple_keycode));
            register_code(state->simple_keycode & 0xFF);
            return false;
        }

        state->timer       = timer_read();
        state->interrupted = false;
        state->resolved    = false;
        state->tapped      = false;
        state->repeating   = false; // self-heal if a release was ever missed
        state->row         = record->event.key.row;
        state->col         = record->event.key.col;
        register_code(mod_key);
    } else {
        // Key released
        if (state->repeating) {
            unregister_code(state->simple_keycode & 0xFF);
            unregister_mods(extract_mod_bits(state->simple_keycode));
            state->repeating = false;
            state->tapped    = true; // pressing again right away keeps repeating
            state->tap_time  = timer_read();
            return false;
        }

        bool taps = timer_elapsed(state->timer) < TAPPING_TERM && !state->interrupted;

        // A chord partner pressed before this key has to type first.
        if (taps) {
            settle_undecided_taps(g_mt_states, g_mt_count, state);
        }

        unregister_code(mod_key);

        if (taps) {
            send_tap(state, g_mt_states, g_mt_count);
        }

        // Reset state so a released key can't be "interrupted" by later keypresses.
        state->timer       = 0;
        state->interrupted = false;
        state->resolved    = false;
    }

    return false; // We handled this keycode
}
