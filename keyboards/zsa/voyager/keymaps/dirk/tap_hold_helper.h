#pragma once
#include QMK_KEYBOARD_H

// Tap the win or mac keycode based on is_mac. Optionally strips mods around the
// tap (useful for movement keys where we want to *let* shift through, vs. for
// custom-character keys where we don't).
bool win_or_mac(uint16_t keycode_win, uint16_t keycode_mac, bool is_mac, bool remove_mods, keyrecord_t *record);
