#include "dynamic_mt.h"

// Defined in keymap.c
extern const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS];

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

// Read the hand designation ('L' / 'R' / '*') for a matrix position.
// Off-matrix events (combos etc.) report as '*' so they bypass the hand check.
static char hand_at(uint8_t row, uint8_t col) {
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return '*';
    return pgm_read_byte(&chordal_hold_layout[row][col]);
}

static void send_tap(dynamic_mt_state_t *state) {
    if (state->tap_handler) {
        state->tap_handler();
    } else if (state->simple_keycode) {
        tap_code16(state->simple_keycode);
    }
}

// Called on every key press. Decides hold-or-tap for any in-flight dynamic_mt:
//   - same hand within tapping term  → cancel the mod, emit the tap *before* the
//                                       interrupting key proceeds (chordal-hold rule)
//   - different hand / thumb / past term → commit as hold (mod stays registered)
void check_mt_interruptions(uint16_t keycode, keyrecord_t *record, dynamic_mt_state_t *states, size_t count) {
    if (!record->event.pressed) return;

    char other_hand = hand_at(record->event.key.row, record->event.key.col);

    for (size_t i = 0; i < count; i++) {
        dynamic_mt_state_t *state = &states[i];
        if (!state->timer || state->resolved || keycode == state->keycode) continue;

        char self_hand = hand_at(state->row, state->col);
        bool same_hand = (self_hand != '*' && other_hand != '*' && self_hand == other_hand);

        if (same_hand && timer_elapsed(state->timer) < TAPPING_TERM) {
            uint16_t mod_key = is_mac ? state->mac_mod : state->win_mod;
            unregister_code(mod_key);
            send_tap(state);
        }

        state->resolved    = true;
        state->interrupted = true; // suppress late tap on release in both branches
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
        state->timer       = timer_read();
        state->interrupted = false;
        state->resolved    = false;
        state->row         = record->event.key.row;
        state->col         = record->event.key.col;
        register_code(mod_key);
    } else {
        // Key released
        unregister_code(mod_key);

        if (timer_elapsed(state->timer) < TAPPING_TERM && !state->interrupted) {
            send_tap(state);
        }

        // Reset state so a released key can't be "interrupted" by later keypresses.
        state->timer       = 0;
        state->interrupted = false;
        state->resolved    = false;
    }

    return false; // We handled this keycode
}
