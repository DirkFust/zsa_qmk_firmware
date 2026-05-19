#include "tap_hold_helper.h"

bool win_or_mac(uint16_t keycode_win, uint16_t keycode_mac, bool is_mac, bool remove_mods, keyrecord_t *record) {
  if (record->event.pressed) {
    uint8_t mod_state = get_mods();
    if (remove_mods) {
      del_mods(mod_state);
    }
    if (is_mac) {
        tap_code16(keycode_mac);
    } else {
      tap_code16(keycode_win);
    }
    if (remove_mods) {
        set_mods(mod_state);
    }
  }
  return true;
}
