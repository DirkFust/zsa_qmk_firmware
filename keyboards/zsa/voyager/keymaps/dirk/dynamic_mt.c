#include "dynamic_mt.h"

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

// Helper function to check for interruptions
void check_mt_interruptions(uint16_t keycode, keyrecord_t *record, dynamic_mt_state_t *states, size_t count) {
    if (!record->event.pressed) return;

    // Check all active timers for interruption
    for (size_t i = 0; i < count; i++) {
        dynamic_mt_state_t *state = &states[i];
        if (state->timer && timer_elapsed(state->timer) < TAPPING_TERM && keycode != state->keycode) { // Don't interrupt self
            state->interrupted = true;
        }
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
        register_code(mod_key);
    } else {
        // Key released
        unregister_code(mod_key);

        if (timer_elapsed(state->timer) < TAPPING_TERM && !state->interrupted) {
            // Handle tap behavior
            if (state->tap_handler) {
                state->tap_handler();
            } else if (state->simple_keycode) {
                tap_code16(state->simple_keycode);
            }
        }

        // Reset state so a released key can't be "interrupted" by later keypresses.
        state->timer       = 0;
        state->interrupted = false;
    }

    return false; // We handled this keycode
}
