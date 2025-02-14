// clang-format off

#include QMK_KEYBOARD_H
#include "print.h"
#include "version.h"
#include "tap_hold_helper.c"
#include "color_helper.c"
#include "keymap_us_international.h"

#define OS_DETECTION_KEYBOARD_RESET
#define OS_DETECTION_DEBOUNCE 250

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  You can use _______ in place for KC_TRANS (transparent)  *
 *  Or you can use XXXXXXX for KC_NO (NOOP)                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define ____ KC_TRANSPARENT
#define _____ KC_TRANSPARENT
#define ______ KC_TRANSPARENT
#define _______ KC_TRANSPARENT
#define ________ KC_TRANSPARENT
#define _________ KC_TRANSPARENT
#define ___________ KC_TRANSPARENT
#define ____________ KC_TRANSPARENT

#define XXXX KC_NO
#define XXXXX KC_NO
#define XXXXXX KC_NO
#define XXXXXXX KC_NO
#define XXXXXXXX KC_NO
#define XXXXXXXXX KC_NO

#define TAB_LEFT LCTL(LSFT(KC_TAB))
#define TAB_RIGHT LCTL(KC_TAB)

#define REMOVE_MODS true // remove (and reset) mods before tapping the given key for win/mac? For movement keys (especially 'home' and 'end') I want to be able to press shift to select text.

static uint8_t current_layer = 0;
static bool in_one_shot_layer_f_keys = false;
static bool is_umlaut_layer_active = false;

bool is_mac = true;  // Track the current OS mode, mac or win

enum custom_keycodes {
  TG_UML= SAFE_RANGE,
  CU_CIRC,      // ^
  OS_TOGGLE,    // toggles between mac and windows
  CU_TILD,      // ~ (win/mac)
  CU_HOME,      // home (win/mac)
  CU_END,       // end (win/mac)
  CU_PGUP,      // page up (win/mac)
  CU_PGDOWN,    // page down (win/mac)
  CU_MACSPC,    // additional space for mac layout. Only together with the command mod

  CU_QUOT,      // us KC_QUOT has ' and ". I want them the other way round... " unshifted, ' shifted
  CU_HASH,      // # and ` for us keyboard layout
  CU_PLUS,      // + and * for us keyboard layout
  CU_SLASH,     // / and backslash for us keyboard layout
  CU_COMMA,     // , and ; for us keyboard layout
  CU_DOT,       // . and : for us keyboard layout

  CU_AE,        // ä
  CU_OE,        // ö
  CU_UE,        // ü
  CU_EURO,      // €
  CU_DEG,       // ° (win/mac)
  CU_SECT,      // § (win/mac)
};

enum layers {
  BASE,
  MAC,
  UMLAUT,
  SYM_NUM,
  MOVEMENT,
  MOUSE,
  GAMING,
  FUNCTION,
};

void set_os_mac(void) {
  layer_move(MAC);  // Switch to Mac layer
  is_mac = true;
}

void set_os_win(void) {
  layer_move(BASE);  // Switch to Windows layer
  is_mac = false;
}

void toggle_os_layer(void) {
  if (is_mac) {
    set_os_win();
  } else {
    set_os_mac();
  }
}

// Feature Caps Words, see https://docs.qmk.fm/#/feature_caps_word
bool caps_word_press_user(uint16_t keycode) {
  // #ifdef CONSOLE_ENABLE
  //   uprintf("caps_word_press_user(): kc: 0x%04X, keycoode: %u\n", keycode, keycode);
  // #endif
  switch (keycode) {
  // Keycodes that continue Caps Word, with shift applied.
  case KC_A ... KC_Z:
  case US_UDIA:   // ü from us international keyboard layout
  case US_ODIA:   // ö from us international keyboard layout
  case US_ADIA:   // ä from us international keyboard layout
  case KC_MINS:   // -_
    add_weak_mods(MOD_BIT(KC_LSFT));  // Apply shift to next key.
    return true;

  // Keycodes that continue Caps Word, without shifting.
  case KC_1 ... KC_0:
  case KC_BSPC:
  case KC_DEL:
  case KC_UNDS:
  case TG_UML:
  case US_SS:
  case KC_LEFT:
  case KC_RIGHT:
  case LT(MOVEMENT, KC_BSPC):
  case LT(MOVEMENT, KC_LEFT):
  case LT(MOVEMENT, KC_RIGHT):
    return true;

  default:
    return false;  // Deactivate Caps Word.
  }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [BASE] = LAYOUT_voyager(
        KC_ESCAPE    , KC_Q    , KC_W   , KC_E    , KC_R     , KC_T    ,             KC_Z       , KC_U      , KC_I    , KC_O   , KC_P   , KC_DEL ,
        TG_UML       , KC_A    , KC_S   , KC_D    , KC_F     , KC_G    ,             KC_H       , KC_J      , KC_K    , KC_L   , CU_QUOT, CU_HASH,
        CW_TOGG      , KC_Y    , KC_X   , KC_C    , KC_V     , KC_B    ,             KC_N       , KC_M      , CU_COMMA, CU_DOT , KC_MINS, CU_PLUS,
        OSL(FUNCTION), XXXXXXXX, XXXXXXX, CU_SLASH, KC_LGUI  , XXXXXXX ,             XXXXXXXXX  , KC_RGUI   , XXXXXXXX, XXXXXXX, XXXXXXX, KC_F5  ,
                           MT(MOD_LCTL, KC_ENTER), LT(SYM_NUM, KC_TAB) ,             LT(MOVEMENT, KC_BSPC)  , KC_SPACE
  ),
  [MAC] = LAYOUT_voyager( // GUI acts as COMMAND in macOs, CTRL as control
        _______, _______, _______, _______, _______  , _______,             _______, _______, _______, _______, ____________, ________,
        _______, _______, _______, _______, _______  , _______,             _______, _______, _______, _______, ____________, ________,
        _______, _______, _______, _______, _______  , _______,             _______, _______, _______, _______, ____________, ________,
        _______, _______, _______, _______, CU_MACSPC, _______,             _______, XXXXXXX, _______, _______, S(G(KC_F16)), G(KC_R) ,
                   MT(MOD_LGUI, KC_ENTER) ,LT(SYM_NUM, KC_TAB),             LT(MOVEMENT, KC_BSPC), MT(MOD_LCTL, KC_SPACE)
  ),
  [UMLAUT] = LAYOUT_voyager(
        _______, KC_AT, _____, CU_EURO, _______, _______,                          _______, CU_UE, _______, CU_OE, _______, KC_PSCR,
        _______, CU_AE, US_SS, _______, _______, _______,                          _______, _____, _______, KC_AT, _______, _______,
        _______, _____, _____, _______, _______, _______,                          _______, _____, _______, _____, _______, _______,
        _______, _____, _____, _______, _______, _______,                          _______, _____, _______, _____, _______, _______,
                                        _______, _______,                          _______, _____
  ),
  [SYM_NUM] = LAYOUT_voyager(
        KC_ESCAPE, CU_DEG, CU_SECT, KC_PERC, KC_DLR , CU_CIRC,                       XXXXXXX, KC_7   , KC_8   , KC_9   , KC_EQL , KC_DEL,
        CU_TILD  , KC_EQL, KC_EXLM, KC_QUES, KC_LPRN, KC_RPRN,                       KC_PPLS, KC_4   , KC_5   , KC_6   , KC_PAST, XXXXXX,
        XXXXXXX  , XXXXXX, KC_AMPR, KC_PIPE, KC_LCBR, KC_RCBR,                       KC_PMNS, KC_1   , KC_2   , KC_3   , KC_PSLS, XXXXXX,
        XXXXXXX  , XXXXXX, KC_LABK, KC_RABK, KC_LBRC, KC_RBRC,                       XXXXXXX, KC_DOT , KC_COLN, KC_COMM, XXXXXXX, XXXXXX,
                                              ______, _______,                       _______, KC_KP_0
  ),
  [MOVEMENT] = LAYOUT_voyager(
        _______, CU_PGUP  , CU_HOME , KC_UP  , CU_END   , XXXXXXX,                   OS_TOGGLE, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, TO(GAMING),
        _______, CU_PGDOWN, KC_LEFT , KC_DOWN, KC_RIGHT , XXXXXXX,                   XXXXXXX  , XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXXXX ,
        _______, XXXXXXXXX, TAB_LEFT, XXXXXXX, TAB_RIGHT, XXXXXXX,                   XXXXXXX  , XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXXXX ,
        _______, XXXXXXXXX, XXXXXXXX, XXXXXXX, XXXXXXXXX, XXXXXXX,                   XXXXXXX  , XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _________ ,
                                                 _______, _______,                   ________ , _______
  ),
  [MOUSE] = LAYOUT_voyager(
        XXXXXX, XXXXXX, XXXXXXXXX   , KC_MS_UP    , XXXXXXXXX   , XXXXXXXXX ,        XXXXXXXXX , XXXXXXXXX    , KC_MS_WH_UP  , XXXXXXXXX     , XXXXXX, XXXXXX,
        XXXXXX, XXXXXX, KC_MS_LEFT  , KC_MS_DOWN  , KC_MS_RIGHT , XXXXXXXXX ,        XXXXXXXXX , KC_MS_WH_LEFT, KC_MS_WH_DOWN, KC_MS_WH_RIGHT, XXXXXX, XXXXXX,
        XXXXXX, XXXXXX, XXXXXXXXX   , XXXXXXXXX   , XXXXXXXXX   , XXXXXXXXX ,        XXXXXXXXX , XXXXXXXXX    , XXXXXXXXX    , XXXXXXXXX     , XXXXXX, XXXXXX,
        XXXXXX, XXXXXX, KC_MS_ACCEL0, KC_MS_ACCEL1, KC_MS_ACCEL2, XXXXXXXXX ,        XXXXXXXXX , ____________ , ____________ , ____________  , XXXXXX, XXXXXX,
                                                    XXXXXXXXX   , KC_MS_BTN1,        KC_MS_BTN2, XXXXXXXXX
  ),
  [GAMING] = LAYOUT_voyager(
        _______, KC_1, KC_2, KC_3, KC_4, KC_5,                                     KC_6, KC_7, KC_8    , KC_9  , KC_0     , TO(BASE),
        _______, KC_Q, KC_W, KC_E, KC_R, KC_T,                                     KC_Z, KC_U, KC_I    , KC_O  , KC_P     , KC_LBRC ,
        _______, KC_A, KC_S, KC_D, KC_F, KC_G,                                     KC_H, KC_J, KC_K    , KC_L  , KC_COLON , KC_QUOTE,
        KC_LSFT, KC_Y, KC_X, KC_C, KC_V, KC_B,                                     KC_N, KC_M, KC_COMMA, KC_DOT, KC_SLASH , KC_RSFT ,
                           _________, _________,                                     _________, _________
  ),
  [FUNCTION] = LAYOUT_voyager(
        ________, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX,                            XXXXXX, KC_F1 , KC_F2 , KC_F3 , XXXXXX, XXXXXX,
        ________, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX,                            XXXXXX, KC_F4 , KC_F5 , KC_F6 , XXXXXX, XXXXXX,
        ________, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX,                            XXXXXX, KC_F7 , KC_F8 , KC_F9 , XXXXXX, XXXXXX,
        TO(BASE), XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX,                            XXXXXX, KC_F10, KC_F11, KC_F12, XXXXXX, XXXXXX,
                                          ______, ______,                            ______, _________
  ),
};

/************************************************************************************************************************
* Status LEDs                                                                                                           *
* These are the status LEDs of the voyager. Controlled by STATUS_LED_X(bool) (X=1...4).
* LED 1 and 2 are on the left half, 3 and 4 on the right. 1 and 3 are red, 2 and 4 green.
************************************************************************************************************************/
#ifdef STATUS_LED_1
// LED 1 indicates when any layer above the MAC layer is active.
void set_layer_indicator_led(bool active) {
  STATUS_LED_1(active);
}
#endif  // STATUS_LED_1

#ifdef STATUS_LED_3
// LED 3 indicates when Caps word is active.
void caps_word_set_user(bool active) {
  STATUS_LED_3(active);
}
#endif  // STATUS_LED_3

/************************************************************************************************************************
 *    COMBOS                                                                                                            *
 * See https://docs.qmk.fm/#/feature_combo                                                                              *
 * !! When new combos are declared, COMBO_COUNT in config.h must be adjusted to the new number! !!                      *
 * I am using a "nifty trick" below, so this is not necessary. I don't fully understand this trick                      *
 ************************************************************************************************************************/

// define combo names
enum combos {
  COMBO_TOGGLE_MOUSE,

  COMBO_LSHIFT,
  COMBO_LCTRL,
  COMBO_LALT,
  COMBO_LCTRL_SHIFT,
  COMBO_LCTRL_ALT,
  COMBO_LCTRL_ALT_SHIFT,
  COMBO_LALT_SHIFT1,
  COMBO_LALT_SHIFT2,

  COMBO_RSHIFT,
  COMBO_RCTRL,
  COMBO_RALT,
  COMBO_RCTRL_SHIFT,
  COMBO_RCTRL_ALT,
  COMBO_RCTRL_ALT_SHIFT,
  COMBO_RALT_SHIFT1,
  COMBO_RALT_SHIFT2,

  // nifty trick to auto-specify how many combos you have
  COMBO_LENGTH
};

// nifty trick continued
uint16_t COMBO_LEN = COMBO_LENGTH;

// define keys that make up combos
// Combos are declared on the keycodes of the base layer (see #define COMBO_ONLY_FROM_LAYER 0 in config.h), so other layers work with them, even if the keys are set to NO_OP
const uint16_t PROGMEM toggle_mouse[] = {LT(MOVEMENT, KC_BSPC), LT(SYM_NUM, KC_TAB), COMBO_END};

const uint16_t PROGMEM df_combo[] = {KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM sd_combo[] = {KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM as_combo[] = {KC_A, KC_S, COMBO_END};
const uint16_t PROGMEM sdf_combo[] = {KC_S, KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM adf_combo[] = {KC_A, KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM asd_combo[] = {KC_A, KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM asf_combo[] = {KC_A, KC_S, KC_F, COMBO_END};
const uint16_t PROGMEM asdf_combo[] = {KC_A, KC_S, KC_D, KC_F, COMBO_END};

const uint16_t PROGMEM jk_combo[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM kl_combo[] = {KC_K, KC_L, COMBO_END};
const uint16_t PROGMEM lquot_combo[] = {KC_L, CU_QUOT, COMBO_END};
const uint16_t PROGMEM jkl_combo[] = {KC_J, KC_K, KC_L, COMBO_END};

const uint16_t PROGMEM jkquot_combo[] = {KC_J, KC_K, CU_QUOT, COMBO_END};
const uint16_t PROGMEM jlquot_combo[] = {KC_J, KC_L, CU_QUOT, COMBO_END};
const uint16_t PROGMEM klquot_combo[] = {KC_K, KC_L, CU_QUOT, COMBO_END};
const uint16_t PROGMEM jklquot_combo[] = {KC_J, KC_K, KC_L, CU_QUOT, COMBO_END};

void process_combo_event(uint16_t combo_index, bool pressed) {
  #ifdef CONSOLE_ENABLE
  if (pressed) {
    uprint("pressed: ");
  } else {
    uprint("released:");
  }
  #endif

  uint16_t alt = KC_LALT;
  uint16_t ctrl = KC_LCTL;
  if (is_mac) {
    alt = KC_LOPT;
    ctrl = KC_LCMD;
  }

  switch (combo_index) {
    case COMBO_LCTRL: case COMBO_RCTRL:
      #ifdef CONSOLE_ENABLE
        uprintf("COMBO_CTRL: %u\n", combo_index);
      #endif
      if (pressed) {
        register_code(ctrl);
      } else {
        unregister_code(ctrl);
      }
      break;
    case COMBO_LALT: case COMBO_RALT:
      #ifdef CONSOLE_ENABLE
        uprintf("COMBO_ALT: %u\n", combo_index);
      #endif
      if (pressed) {
        register_code(alt);
      } else {
        unregister_code(alt);
      }
      break;
    case COMBO_LCTRL_SHIFT: case COMBO_RCTRL_SHIFT:
      #ifdef CONSOLE_ENABLE
        uprintf("COMBO_CTRL_SHIFT: %u\n", combo_index);
      #endif
      if (pressed) {
        register_code(ctrl);
        register_code(KC_LSFT);

      } else {
        unregister_code(KC_LSFT);
        unregister_code(ctrl);
      }
      break;
    case COMBO_LCTRL_ALT: case COMBO_RCTRL_ALT:
      #ifdef CONSOLE_ENABLE
        uprintf("COMBO_LCTRL_ALT: %u\n", combo_index);
      #endif
      if (pressed) {
        register_code(ctrl);
        register_code(alt);

      } else {
        unregister_code(alt);
        unregister_code(ctrl);
      }
      break;
    case COMBO_LALT_SHIFT1: case COMBO_LALT_SHIFT2: case COMBO_RALT_SHIFT1: case COMBO_RALT_SHIFT2:
      #ifdef CONSOLE_ENABLE
        uprintf("COMBO_ALT_SHIFT: %u\n", combo_index);
      #endif
      if (pressed) {
        register_code(alt);
        register_code(KC_LSFT);

      } else {
        unregister_code(KC_LSFT);
        unregister_code(alt);
      }
      break;
    case COMBO_LCTRL_ALT_SHIFT: case COMBO_RCTRL_ALT_SHIFT:
      #ifdef CONSOLE_ENABLE
        uprintf("COMBO_CTRL_ALT_SHIFT: %u\n", combo_index);
      #endif
      if (pressed) {
        register_code(ctrl);
        register_code(alt);
        register_code(KC_LSFT);

      } else {
        unregister_code(KC_LSFT);
        unregister_code(alt);
        unregister_code(ctrl);
      }
      break;
    default:
      break;
  }
}

//map combo names to their keys and the keys to their trigger.
//all but three combos (those with COMBO, not COMBO_ACTION) are handled in process_combo_event()
combo_t key_combos[] = {
  [COMBO_TOGGLE_MOUSE] = COMBO(toggle_mouse, TG(MOUSE)), // not handled in process_combo_event()
  // Left hand single mod
  [COMBO_LSHIFT] = COMBO(df_combo, KC_LSFT), // not handled in process_combo_event()
  [COMBO_LCTRL] = COMBO_ACTION(sd_combo),
  [COMBO_LALT] = COMBO_ACTION(as_combo),

  // Left hand two mods
  [COMBO_LCTRL_SHIFT] = COMBO_ACTION(sdf_combo),
  [COMBO_LCTRL_ALT] = COMBO_ACTION(asd_combo),
  // two combos for alt-shift. Every combo for two mods consists of three keys.
  // when the two mods are next to each other (shift+ctrl, alt+ctrl) all keys of both mods are pressed, since they overlap.
  // the single non-overlapping mod combination alt+shift needs two keys of one mod and one of the other to use the same logic of three keys.
  // since there is no natural "leading mod" that obviously gets two keys and one "lesser mod" that just uses one, both combos ("two keys for shift, one for alt" and "one key for shift, two for alt")
  // are bound to alt+shift
  [COMBO_LALT_SHIFT1] = COMBO_ACTION(adf_combo),
  [COMBO_LALT_SHIFT2] = COMBO_ACTION(asf_combo),

  // Left hand three mods
  [COMBO_LCTRL_ALT_SHIFT] = COMBO_ACTION(asdf_combo),

  // Right hand single mod
  [COMBO_RSHIFT] = COMBO(jk_combo, KC_LSFT), // not handled in process_combo_event()
  [COMBO_RCTRL] = COMBO_ACTION(kl_combo),
  [COMBO_RALT] = COMBO_ACTION(lquot_combo),

  // Right hand two mods
  [COMBO_RCTRL_SHIFT] = COMBO_ACTION(jkl_combo),
  [COMBO_RCTRL_ALT] = COMBO_ACTION(klquot_combo),

  // two combos for alt+shift, see above why
  [COMBO_RALT_SHIFT1] = COMBO_ACTION(jkquot_combo),
  [COMBO_RALT_SHIFT2] = COMBO_ACTION(jlquot_combo),

  // Right hand three mods
  [COMBO_RCTRL_ALT_SHIFT] = COMBO_ACTION(jklquot_combo),
};

// Set tapping term per key (https://docs.qmk.fm/#/tap_hold?id=tapping-term)
// A key counts as HOLD if held longer than TAPPING_TERM, as TAP/DOUBLE TAP if shorter
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    // case HYPR_T(KC_SPACE):
    //   return TAPPING_TERM * 2.5;
    default:
      return TAPPING_TERM;
  }
}

uint32_t detect_os_callback(uint32_t trigger_time, void *cb_arg) {
  #ifdef CONSOLE_ENABLE
    uprint("CALLED detect_os_callback()\n");
  #endif
  switch (detected_host_os()) {
    case OS_MACOS:
      #ifdef CONSOLE_ENABLE
        uprint("DETECTED MAC-OS\n");
      #endif
      set_os_mac();
      break;
    case OS_IOS:
      #ifdef CONSOLE_ENABLE
        uprint("DETECTED iOS\n");
      #endif
      set_os_mac();
      break;
    case OS_WINDOWS:
      #ifdef CONSOLE_ENABLE
        uprint("DETECTED WINDOWS\n");
      #endif
      set_os_win();
      break;
    case OS_LINUX:
      #ifdef CONSOLE_ENABLE
        uprint("DETECTED LINUX\n");
      #endif
      set_os_win();
      break;
    case OS_UNSURE:
      #ifdef CONSOLE_ENABLE
        uprint("unsure about os\n");
      #endif
      set_os_mac();
      break;
  }
  return 0;
}

void detect_os(void) {
    switch (detected_host_os()) {
      case OS_MACOS:
        #ifdef CONSOLE_ENABLE
          uprint("DETECTED MAC-OS\n");
        #endif
        set_os_mac();
        break;
      case OS_IOS:
        #ifdef CONSOLE_ENABLE
          uprint("DETECTED iOS\n");
        #endif
        set_os_mac();
        break;
      case OS_WINDOWS:
        #ifdef CONSOLE_ENABLE
          uprint("DETECTED WINDOWS\n");
        #endif
        set_os_win();
        break;
      case OS_LINUX:
        #ifdef CONSOLE_ENABLE
          uprint("DETECTED LINUX\n");
        #endif
        set_os_win();
        break;
      case OS_UNSURE:
        #ifdef CONSOLE_ENABLE
          uprint("unsure about os\n");
        #endif
        set_os_mac();
        break;
  }
}

void keyboard_post_init_user(void) {
  rgb_matrix_enable();
  rgb_matrix_mode(RGB_MATRIX_CYCLE_LEFT_RIGHT);

  defer_exec(5000, change_led_effect_heatmap_callback, NULL);
  defer_exec(500, detect_os_callback, NULL);
}

layer_state_t layer_state_set_user(layer_state_t state) {
  uint8_t new_layer = get_highest_layer(state);
  set_layer_indicator_led(new_layer > MAC); // control status LED, on for layers higher than the MAC layer

  if (current_layer != new_layer) {
    if (new_layer != FUNCTION) {
      in_one_shot_layer_f_keys = false;
    }
    switch (new_layer) {
      case SYM_NUM:
        #ifdef CONSOLE_ENABLE
          print("layer: SYM_NUM\n");
        #endif
        rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
        rgb_matrix_sethsv(HSV_GREEN);
        break;
      case MOVEMENT:
        #ifdef CONSOLE_ENABLE
          print("layer: MOVEMENT\n");
        #endif
        rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
        rgb_matrix_sethsv(HSV_BLUE);
        break;
      case MOUSE:
        #ifdef CONSOLE_ENABLE
          print("layer: MOUSE\n");
        #endif
        rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
        rgb_matrix_sethsv(HSV_RED);
        break;
      case BASE: case MAC:
        #ifdef CONSOLE_ENABLE
          print("layer: BASE\n");
        #endif
        rgb_matrix_mode(RGB_MATRIX_TYPING_HEATMAP);
        break;
      case GAMING:
        #ifdef CONSOLE_ENABLE
          print("layer: GAMING\n");
        #endif
        rgb_matrix_mode(RGB_MATRIX_CUSTOM_overwatch);
        break;
      case FUNCTION:
        #ifdef CONSOLE_ENABLE
          print("layer: FUNCTION\n");
        #endif
        rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
        rgb_matrix_sethsv(HSV_GOLD);
        break;
      case UMLAUT:
        #ifdef CONSOLE_ENABLE
          print("layer: UMLAUT\n");
        #endif
        rgb_matrix_mode(RGB_MATRIX_CYCLE_LEFT_RIGHT);
        break;
      default:
        #ifdef CONSOLE_ENABLE
          print("layer: DEFAULT\n");
        #endif
        rgb_matrix_mode(RGB_MATRIX_CYCLE_LEFT_RIGHT);
        break;
    }
    current_layer = new_layer;
  }

  return state;
}

// Handles custom keycodes related to specific functionality
static bool handle_custom_keycodes(uint16_t keycode, keyrecord_t *record) {
  uint8_t mod_state = get_mods();

  switch (keycode) {
    case OS_TOGGLE:
      if (record->event.pressed) {
          toggle_os_layer();
          return false;  // Skip default processing
      }

    case CU_HOME:
      return win_or_mac(KC_HOME, G(KC_LEFT), is_mac, !REMOVE_MODS, record); // do not remove mods
    case CU_END:
      return win_or_mac(KC_END, G(KC_RIGHT), is_mac, !REMOVE_MODS, record); // do not remove mods
    case CU_EURO:
      return win_or_mac(RALT(KC_5), S(A(KC_2)), is_mac, REMOVE_MODS, record);
    case CU_DEG:
      return win_or_mac(RALT(LSFT(KC_SEMICOLON)), S(A(KC_8)), is_mac, REMOVE_MODS, record);
    case CU_SECT:
      return win_or_mac(RALT(LSFT(KC_S)), KC_GRAVE, is_mac, REMOVE_MODS, record);

    case CU_TILD:
      if (record->event.pressed) {
        del_mods(mod_state);
        if (is_mac) {
          tap_code16(S(KC_NONUS_BACKSLASH)); // This is a macOs special- I want the tilde ~ here, normally on SHIFT(KC_GRAVE). But macOs uses it for ±
          tap_code16(KC_SPACE);
          return false;
        } else {
          tap_code16(KC_TILD);
          tap_code16(KC_SPACE);
          return true;
        }
        set_mods(mod_state);
      }
      return true;

    case CU_AE:
      if (record->event.pressed) {
        if (is_mac) {
          del_mods(mod_state);
          if (mod_state & MOD_MASK_SHIFT) {
            SEND_STRING("\"A"); // Outputs Ä
          } else {
            SEND_STRING("\"a"); // Outputs ä
          }
          set_mods(mod_state);
          return false;
        } else {
          tap_code16(US_ADIA);
          return true;
        }
      }
      return true;

    case CU_OE:
      if (record->event.pressed) {
        if (is_mac) {
          del_mods(mod_state);
          if (mod_state & MOD_MASK_SHIFT) {
            SEND_STRING("\"O"); // Outputs Ö
          } else {
            SEND_STRING("\"o"); // Outputs ö
          }
          set_mods(mod_state);
          return false;
        } else {
          tap_code16(US_ODIA);
          return true;
        }
      }
      return true;

    case CU_UE:
      if (record->event.pressed) {
        if (is_mac) {
          del_mods(mod_state);
          if (mod_state & MOD_MASK_SHIFT) {
            SEND_STRING("\"U"); // Outputs Ü
          } else {
            SEND_STRING("\"u"); // Outputs ü
          }
          set_mods(mod_state);
          return false;
        } else {
          tap_code16(US_UDIA);
          return true;
        }
      }
      return true;

    case CU_QUOT:
      if (record->event.pressed) {
        if (mod_state & MOD_MASK_SHIFT) {
          del_mods(mod_state);
          tap_code16(KC_QUOT);
          tap_code16(KC_SPACE);
          set_mods(mod_state);
        } else {
          tap_code16(KC_DOUBLE_QUOTE);
          tap_code16(KC_SPACE);
        }
      }
      return true;

    case CU_PGUP:
      return win_or_mac(KC_PGUP, G(KC_UP), is_mac, !REMOVE_MODS, record); // do not remove mods

    case CU_PGDOWN:
      return win_or_mac(KC_PGDN, G(KC_DOWN), is_mac, !REMOVE_MODS, record); // do not remove mods

    case CU_MACSPC:
      if (is_mac && record->event.pressed && (mod_state & MOD_MASK_CTRL)) {
        tap_code16(KC_SPACE); // Additional Space for mac. Only used for COMMAND-Space, since those two are on the same key for mac
      }
      return true;

    case CU_SLASH: // Slash and backslash
      if (record->event.pressed) {
        if (mod_state & MOD_MASK_SHIFT) {
          del_mods(mod_state);
          tap_code16(KC_BACKSLASH);
          set_mods(mod_state);
        } else {
            tap_code16(KC_SLASH);
        }
      }
      return true;

    case CU_COMMA: // , and ;
      if (record->event.pressed) {
        if (mod_state & MOD_MASK_SHIFT) {
          del_mods(mod_state);
          tap_code16(KC_SEMICOLON);
          set_mods(mod_state);
        } else {
            tap_code16(KC_COMMA);
        }
      }
      return true;

    case CU_DOT: // . and :
      if (record->event.pressed) {
        if (mod_state & MOD_MASK_SHIFT) {
          del_mods(mod_state);
          tap_code16(KC_COLON);
          set_mods(mod_state);
        } else {
            tap_code16(KC_DOT);
        }
      }
      return true;

    case CU_HASH:
      if (record->event.pressed) {
        if (mod_state & MOD_MASK_SHIFT) {
          del_mods(mod_state);
          if (is_mac) {
            tap_code16(KC_NONUS_BACKSLASH); // This is a macOs special- I want the backtick ` here, normally KC_GRAVE. But macOs uses KC_GRAVE for §
          } else {
            tap_code16(KC_GRAVE); // backtick `
          }

          tap_code16(KC_SPACE);
          set_mods(mod_state);
        } else {
          tap_code16(KC_HASH);
        }
      }
      return true;

    case CU_PLUS:
      if (record->event.pressed) {
        if (mod_state & MOD_MASK_SHIFT) {
          tap_code16(KC_ASTERISK);
        } else {
          tap_code16(KC_PLUS);
        }
      }
      return true;

    case CU_CIRC:
    if (record->event.pressed) {
      tap_code16(KC_CIRCUMFLEX);
      tap_code16(KC_SPACE);
    }
    return true;
  }
  return true;
}

// Handles one-shot layer behavior, including combinations of modifiers
static bool handle_one_shot_layer_for_f_keys(uint16_t keycode, keyrecord_t *record, bool *in_one_shot_layer_f_keys) {
  if (record->event.pressed) {
    if (*in_one_shot_layer_f_keys) {
      switch (keycode) {
        case KC_F1 ... KC_F12: {
          // Handle F-key presses in the FUNCTION layer
          layer_state_t current_layer_state = layer_state;
          if (layer_state_is(FUNCTION)) {
            layer_state_set(current_layer_state);  // Process key in the FUNCTION layer
            tap_code16(keycode);  // Send the F-key press
          }
          *in_one_shot_layer_f_keys = false;
          clear_oneshot_layer_state(ONESHOT_OTHER_KEY_PRESSED);
          return false; // Skip further processing
        }
        // Handle all mod-keys: Mod keys can be pressed without exiting the layer
        case KC_LCTL:
        case KC_LSFT:
        case KC_LALT:
        case KC_LGUI:
        case LCTL(KC_LSFT):
        case LCTL(KC_LALT):
        case LALT(KC_LSFT):
        case LCTL(LALT(KC_LSFT)): {
          return true;  // Do not exit layer
        }
        default:
          // Exit the layer only for non-modifier, non-F-key presses
          #ifdef CONSOLE_ENABLE
            uprintf("Leaving FUNCTION: Keycode: 0x%04X\n", keycode);
          #endif
          *in_one_shot_layer_f_keys = false;
          clear_oneshot_layer_state(ONESHOT_OTHER_KEY_PRESSED);
          return true;
      }
    } else if (keycode == OSL(FUNCTION)) {
      uprintf("Entering FUNCTION: Keycode: 0x%04X\n", keycode);
      *in_one_shot_layer_f_keys = true;  // Activate the one-shot layer
        return true;
    }
  }
  return true;
}


// Handles custom umlaut layer activation and deactivation
static void handle_umlaut_layer(uint16_t keycode, keyrecord_t *record, bool *is_umlaut_layer_active) {
  if (!*is_umlaut_layer_active && keycode == TG_UML && record->event.pressed) {
    *is_umlaut_layer_active = true;
    layer_on(UMLAUT);
  } else if (*is_umlaut_layer_active && keycode != TG_UML && !record->event.pressed) {
    *is_umlaut_layer_active = false;
    layer_off(UMLAUT);
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  #ifdef CONSOLE_ENABLE
    if(is_umlaut_layer_active) {
      uprint("(Layer UMLAUT) ");
    }
    if (in_one_shot_layer_f_keys) {
      uprint("(Layer FKEYS) ");
    }

    if (record->event.pressed) {
      uprint("DOWN ");
    } else {
      uprint("UP ");
    }

    switch (keycode) {
      case KC_F1 ... KC_F12:
        uprint("F key: ");
        break;
      case KC_LCTL:
          uprint("CTRL: ");
          break;
      case KC_LALT:
        uprint("ALT: ");
        break;
      case KC_LSFT:
        uprint("SHIFT: ");
        break;
      case LCTL(KC_LSFT):{
        uprint("CTRL+SHIFT: ");
        break;}
      case LCTL(KC_LALT):{
        uprint("CTRL+ALT: ");
        break;}
      case LALT(KC_LSFT):{
        uprint("ALT+SHIFT: ");
        break;}
      case LCTL(LALT(KC_LSFT)):{
        uprint("CTRL+ALT+SHIFT: ");
        break;}
      case OSL(FUNCTION):{
        uprint("Toggle FUNCTION Layer: ");
        break;}
      default:
        uprint("Other key: ");
    }
    uprintf("keycode: 0x%04X/%u, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u, active Mods: 0x%02X: \n", keycode, keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count, get_mods());
  #endif

  // Handle one-shot layer behavior
  if (!handle_one_shot_layer_for_f_keys(keycode, record, &in_one_shot_layer_f_keys)) {
    return false;
  }

  // Handle custom umlaut layer behavior
  handle_umlaut_layer(keycode, record, &is_umlaut_layer_active);

  // Handle custom keycodes
  return handle_custom_keycodes(keycode, record);
}

// gets called all the time with every matrix scan
void matrix_scan_user(void) {
  STATUS_LED_2(!is_mac); // switch os-layer status led on/off. Mac=off, Win=on
}
