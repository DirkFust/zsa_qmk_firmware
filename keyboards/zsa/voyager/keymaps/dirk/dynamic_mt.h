#pragma once
#include QMK_KEYBOARD_H

// Forward declaration - you'll define this in your keymap.c
extern bool is_mac;

// Structure to hold mod-tap state for each key
typedef struct {
    bool     interrupted;
    bool     resolved;         // hold/tap decision made? (chordal-hold path sets this)
    bool     tapped;           // last action was a tap -> arms the quick-tap repeat
    bool     repeating;        // currently held down as its tap keycode, not as a mod
    uint16_t tap_time;         // when that tap was emitted
    uint16_t timer;
    uint16_t keycode; // The actual keycode this config is for
    uint16_t mac_mod;
    uint16_t win_mod;
    void (*tap_handler)(void); // Function pointer for custom tap behavior
    uint16_t simple_keycode;   // For simple keys, just tap this keycode (0 if using tap_handler)
    uint8_t  row;              // matrix position of last press, for chordal-hold check
    uint8_t  col;
} dynamic_mt_state_t;

// Macros for cleaner configuration

// Define a dynamic-mt with a custom tap-handler function
#define DEFINE_DYNAMIC_MT_CUSTOM(kc, macmod, winmod, tap_func) {.keycode = (kc), .mac_mod = (macmod), .win_mod = (winmod), .tap_handler = (tap_func), .simple_keycode = 0, .interrupted = false, .resolved = false, .tapped = false, .repeating = false, .tap_time = 0, .timer = 0, .row = 0, .col = 0}

// Define a dynamic-mt with a given keycode
#define DEFINE_DYNAMIC_MT_SIMPLE(kc, macmod, winmod, tap_key) {.keycode = (kc), .mac_mod = (macmod), .win_mod = (winmod), .tap_handler = NULL, .simple_keycode = (tap_key), .interrupted = false, .resolved = false, .tapped = false, .repeating = false, .tap_time = 0, .timer = 0, .row = 0, .col = 0}

// Function declarations
bool process_dynamic_mt(uint16_t keycode, keyrecord_t *record);
void init_dynamic_mt_states(dynamic_mt_state_t *states, size_t count);

// Internal functions (don't call these directly)
dynamic_mt_state_t *find_mt_state(uint16_t keycode, dynamic_mt_state_t *states, size_t count);
void                check_mt_interruptions(uint16_t keycode, keyrecord_t *record, dynamic_mt_state_t *states, size_t count);
