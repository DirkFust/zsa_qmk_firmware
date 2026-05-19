// clang-format off

#include QMK_KEYBOARD_H
#include "print.h"
#include "version.h"
#include "tap_hold_helper.h"
#include "color_helper.c"
#include "keymap_us_international.h"
#include "dynamic_mt.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  You can use _______ in place for KC_TRANS (transparent)  *
 *  Or you can use XXXXXXX for KC_NO (NOOP)                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define _______ KC_TRANSPARENT
#define XXXXXXX KC_NO

#define TAB_LEFT LCTL(LSFT(KC_TAB))
#define TAB_RIGHT LCTL(KC_TAB)

// Define mod-tap for homerow-mods. There is a set of definitions for WIN and one for MAC
// Both are missing the rightmost key, it is defined with HR_QUOT because it needs to do more than MT() provides
// WIN homerow-mods
#define WIN_HR_A MT(MOD_LGUI, KC_A)
#define WIN_HR_S MT(MOD_LALT, KC_S)
#define WIN_HR_D MT(MOD_LCTL, KC_D)
#define WIN_HR_F MT(MOD_LSFT, KC_F)
#define WIN_HR_J MT(MOD_RSFT, KC_J)
#define WIN_HR_K MT(MOD_RCTL, KC_K)
#define WIN_HR_L MT(MOD_RALT, KC_L)

// MAC homerow-mods
#define MAC_HR_A MT(MOD_LCTL, KC_A)
#define MAC_HR_S MT(MOD_LALT, KC_S)
#define MAC_HR_D MT(MOD_LGUI, KC_D)
#define MAC_HR_F MT(MOD_LSFT, KC_F)
#define MAC_HR_J MT(MOD_RSFT, KC_J)
#define MAC_HR_K MT(MOD_RGUI, KC_K)
#define MAC_HR_L MT(MOD_RALT, KC_L)


// define for nicer name
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

  CU_HASH,      // # and ` for us keyboard layout
  CU_PLUS,      // + and * for us keyboard layout
  CU_SLASH,     // / and backslash for us keyboard layout
  CU_COMMA,     // , and ; for us keyboard layout
  CU_DOT,       // . and : for us keyboard layout

  CU_AE,        // ä
  CU_OE,        // ö
  CU_UE,        // ü
  CU_EURO,      // €
  CU_DEG,       // °
  CU_SECT,      // §

  DRAG_SCROLL,
  TOGGLE_SCROLL,
  NAVIGATOR_INC_CPI,
  NAVIGATOR_DEC_CPI,
  NAVIGATOR_TURBO,
  NAVIGATOR_AIM,

  // THE FOLLOWING KEYS ARE USED TO ADDRESS DIFFERENCES BETWEEN WIN/MAC
  HR_QUOT,      // us KC_QUOT has ' and ". I want them the other way round... " unshifted, ' shifted. And this key is part of the homerow-mods
  CU_LCMD,      // standin for the mod KC_LGUI, but I switch the behavior for win/mac
  CU_LCTL,      // standin for the mod KC_LCTL, but I switch the behavior for win/mac
  CU_PSCR,      // mac has no printscreen, so handle this
  // mod-tap for SYM_NUM-layer. The clean way would be to have two layers for win/mac, but this was fun to develop.
  // this is dynamic_mt, that kind of mimics MT(), mod-tap, but with flexible mods, depending on a condition.
  // THIS DOES NOT MIX WELL WITH MT-KEYS, so on a layer with only dynamic_mt this is ok, but it tends to break with real MT()
  HR_EQL,       // dynamic_mt homerow mod on SYM_NUM-layer
  HR_QUES,      // dynamic_mt homerow mod on SYM_NUM-layer
  HR_EXLM,      // dynamic_mt homerow mod on SYM_NUM-layer
  HR_LPRN,      // dynamic_mt homerow mod on SYM_NUM-layer
  HR_4,         // dynamic_mt homerow mod on SYM_NUM-layer
  HR_5,         // dynamic_mt homerow mod on SYM_NUM-layer
  HR_6,         // dynamic_mt homerow mod on SYM_NUM-layer
  HR_PAST,      // dynamic_mt homerow mod on SYM_NUM-layer
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

// See: https://docs.qmk.fm/tap_hold#chordal-hold
// tells chordal hold which keys are considered on the left/right hand.
// I want the thumb keys to not be left/right so they don't "block" same-hand-holds (for too long, for example typing "(" and other braces requires the switch to SYM_NUM-layer and the key, both on the same hand)
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM =
  LAYOUT_voyager(
        'L', 'L', 'L', 'L', 'L', 'L',            'R', 'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L', 'L',            'R', 'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L', 'L',            'R', 'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L', 'L',            'R', 'R', 'R', 'R', 'R', 'R',
                            '*', '*',            '*', '*'
    );

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [BASE] = LAYOUT_voyager(
        KC_ESCAPE, KC_Q    , KC_W    , KC_E    , KC_R    , KC_T   ,                                                     KC_Z, KC_U    , KC_I    , KC_O    , KC_P   , KC_DEL ,
        TG_UML   , WIN_HR_A, WIN_HR_S, WIN_HR_D, WIN_HR_F, KC_G   ,                                                     KC_H, WIN_HR_J, WIN_HR_K, WIN_HR_L, HR_QUOT, CU_HASH,   // HR for HOME_ROW_MOD
        CW_TOGG  , KC_Y    , KC_X    , KC_C    , KC_V    , KC_B   ,                                                     KC_N, KC_M    , CU_COMMA, CU_DOT  , KC_MINS, CU_PLUS,
        XXXXXXX, XXXXXXX, XXXXXXX , CU_SLASH, XXXXXXX, XXXXXXX,                                                     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_F5  ,
                       MT(MOD_LCTL, KC_ENTER), LT(SYM_NUM, KC_TAB),                                                     LT(MOVEMENT, KC_BSPC)  , KC_SPACE
  ),
  [MAC] = LAYOUT_voyager( // GUI acts as COMMAND in macOs, CTRL as control
        _______, _______, _______, _______, _______, _______,                                                     _______, _______, _______, _______    , _______, _______,
        _______, MAC_HR_A, MAC_HR_S, MAC_HR_D, MAC_HR_F    , _______,                                                     _______, MAC_HR_J, MAC_HR_K, MAC_HR_L    , _______, _______,  // HR for HOME_ROW_MOD
        _______, _______, _______, _______, _______, _______,                                                     _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, S(G(KC_F18)), _______,                                                     _______, _______, _______, S(G(KC_F11)), S(G(KC_F12)), G(KC_R),
                                    MT(MOD_LGUI, KC_ENTER) , _______,                                                     _______, _______
  ),
  [UMLAUT] = LAYOUT_voyager(
        _______, KC_AT, _______, CU_EURO, _______, _______,                                                               _______, CU_UE, _______, CU_OE, _______, CU_PSCR,
        _______, CU_AE, US_SS, _______, _______, _______,                                                               _______, _______, _______, KC_AT, _______, _______,
        _______, _______, _______, _______, _______, _______,                                                               _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,                                                               _______, _______, _______, _______, _______, _______,
                                        _______, _______,                                                               _______, _______
  ),
  [SYM_NUM] = LAYOUT_voyager(
        KC_ESCAPE, CU_DEG, CU_SECT, KC_PERC, KC_DLR , CU_CIRC,                                                          XXXXXXX, KC_7   , KC_8   , KC_9   , KC_EQL , KC_DEL,
        CU_TILD  , HR_EQL, HR_EXLM, HR_QUES, HR_LPRN, KC_RPRN,                                                          KC_PPLS, HR_4   , HR_5   , HR_6   , HR_PAST, XXXXXXX,
        XXXXXXX  , XXXXXXX, KC_AMPR, KC_PIPE, KC_LCBR, KC_RCBR,                                                          KC_PMNS, KC_1   , KC_2   , KC_3   , KC_PSLS, XXXXXXX,
        XXXXXXX  , XXXXXXX, KC_LABK, KC_RABK, KC_LBRC, KC_RBRC,                                                          XXXXXXX, KC_DOT , KC_COLN, KC_COMM, XXXXXXX, XXXXXXX,
                                              _______, _______,                                                          _______, KC_0
  ),
  [MOVEMENT] = LAYOUT_voyager(
        _______, CU_PGUP  , CU_HOME , KC_UP  , CU_END      , XXXXXXX,                                                   OS_TOGGLE, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, TO(GAMING),
        _______, CU_PGDOWN, KC_LEFT , KC_DOWN, KC_RIGHT    , XXXXXXX,                                                   XXXXXXX  , KC_LSFT, CU_LCMD, KC_LALT, CU_LCTL, XXXXXXX ,
        _______, XXXXXXX, KC_COMMA, XXXXXXX, KC_SEMICOLON, XXXXXXX,                                                   XXXXXXX  , XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX ,
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX   , XXXXXXX,                                                   XXXXXXX  , XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______ ,
                                                 _______ , _______,                                                   _______ , _______
  ),
  [MOUSE] = LAYOUT_voyager(
        NAVIGATOR_DEC_CPI, NAVIGATOR_INC_CPI, XXXXXXX, XXXXXXX, XXXXXXX , QK_LLCK     ,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, CU_LCTL          , KC_LALT, CU_LCMD   , KC_LSFT   , KC_MS_BTN2  ,                            KC_MS_BTN3, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_MS_BTN2, XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                                                   KC_MS_BTN1, DRAG_SCROLL ,                            XXXXXXX, XXXXXXX
  ),
  [FUNCTION] = LAYOUT_voyager(
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, QK_LLCK,                                                          XXXXXXX, KC_F1 , KC_F2 , KC_F3 , XXXXXXX, XXXXXXX,
        _______, CU_LCTL, KC_LALT, CU_LCMD, KC_LSFT, XXXXXXX,                                                          XXXXXXX, KC_F4 , KC_F5 , KC_F6 , XXXXXXX, XXXXXXX,
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                                          XXXXXXX, KC_F7 , KC_F8 , KC_F9 , XXXXXXX, XXXXXXX,
        TO(BASE), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                                          XXXXXXX, KC_F10, KC_F11, KC_F12, XXXXXXX, XXXXXXX,
                                             _______, _______,                                                          _______, _______
  ),
};

/******************************
 NAVIGATOR
******************************/
extern bool set_scrolling;
extern bool navigator_turbo;
extern bool navigator_aim;
void pointing_device_init_user(void) {
  set_auto_mouse_enable(true);
}

bool is_mouse_record_kb(uint16_t keycode, keyrecord_t* record) {
  switch (keycode) {
    case NAVIGATOR_INC_CPI ... NAVIGATOR_AIM:
    case DRAG_SCROLL:
    case TOGGLE_SCROLL:
    case KC_MS_BTN3:
    case KC_MS_BTN2:
    case KC_MS_BTN1:
      return true;
  }
  return is_mouse_record_user(keycode, record);
}

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
 *
 * I am using a "nifty trick" below, so this is not necessary.                                                          *
 ************************************************************************************************************************/

// define combo names
enum combos {
  // COMBO_TOGGLE_MOUSE,
  COMBO_TOGGLE_F_LAYER,

  // nifty trick to auto-specify how many combos you have
  COMBO_LENGTH
};

// nifty trick continued. Every item in an enum gets a consecutive number, so the last
// item has the number <number of items>. This so COMBO_LEN=COMBO_LENGTH
uint16_t COMBO_LEN = COMBO_LENGTH;

// define keys that make up combos
// Combos are declared on the keycodes of the base layer (see #define COMBO_ONLY_FROM_LAYER 0 in config.h), so other layers work with them, even if the keys are set to NO_OP
const uint16_t PROGMEM toggle_f_layer[] = {LT(MOVEMENT, KC_BSPC), LT(SYM_NUM, KC_TAB), COMBO_END};
// const uint16_t PROGMEM toggle_mouse[] = {MT(MOD_LCTL, KC_ENTER), KC_M, COMBO_END};

//map combo names to their keys and the keys to their trigger.
combo_t key_combos[] = {
  // [COMBO_TOGGLE_MOUSE] = COMBO(toggle_mouse, TG(MOUSE)),
  [COMBO_TOGGLE_F_LAYER] = COMBO(toggle_f_layer, OSL(FUNCTION)),
};

// Set tapping term per key (https://docs.qmk.fm/#/tap_hold?id=tapping-term)
// A key counts as HOLD if held longer than TAPPING_TERM, as TAP/DOUBLE TAP if shorter
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case LT(SYM_NUM, KC_TAB):
      return 150;
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

/************************************************************************************************************************
 *    DYNAMIC-MT                                                                                                        *
 * Mod-Tap is used to have a modifier on hold and a normal key on tap. This is the normal way for homerow-mods          *
 * I want to use the keyboard for win and mac, and some modifiers differ between the different os's, but the            *
 * mod-tap system can't change the modifier depending on some condition (like "which os am I connected to")             *
 * So I build a system to handle this.
 ************************************************************************************************************************/

// Custom tap handler for HR_QUOT. This is a deadkey under US international and is used to write ä, for example, as "a.
// I want to use it as a non-dead key, so I have to tap SPACE afterwards and juggle with the SHIFT mod
void hr_quot_tap_handler(void) {
    if (get_mods() & MOD_MASK_SHIFT) {
        uint8_t saved_mods = get_mods();
        del_mods(saved_mods);
        tap_code16(KC_QUOT);
        tap_code16(KC_SPACE);
        set_mods(saved_mods);
    } else {
        tap_code16(KC_DOUBLE_QUOTE);
        tap_code16(KC_SPACE);
    }
}

// Configure your dynamic mod-tap keys - explicitly list which keys are MT keys
static dynamic_mt_state_t mt_states[] = {
    // Argument sequence: Keycode, mac-modifer, win-modifier, tap-action/keycode
    DEFINE_DYNAMIC_MT_CUSTOM(HR_QUOT, KC_RCTL, KC_RGUI, hr_quot_tap_handler),     // we want to do more than just type a single key
    DEFINE_DYNAMIC_MT_SIMPLE(HR_EQL, KC_LCTL, KC_LGUI, KC_EQL),                   // all other just have a simple keycode to tap
    DEFINE_DYNAMIC_MT_SIMPLE(HR_QUES, KC_LGUI, KC_LCTL, KC_QUES),
    DEFINE_DYNAMIC_MT_SIMPLE(HR_5, KC_RGUI, KC_RCTL, KC_5),
    DEFINE_DYNAMIC_MT_SIMPLE(HR_PAST, KC_RCTL, KC_RGUI, KC_PAST),
    DEFINE_DYNAMIC_MT_SIMPLE(HR_EXLM, KC_LALT, KC_LALT, KC_EXLM),
    DEFINE_DYNAMIC_MT_SIMPLE(HR_LPRN, KC_LSFT, KC_LSFT, KC_LPRN),
    DEFINE_DYNAMIC_MT_SIMPLE(HR_4, KC_RSFT, KC_RSFT, KC_4),
    DEFINE_DYNAMIC_MT_SIMPLE(HR_6, KC_RALT, KC_RALT, KC_6),
};

void keyboard_post_init_user(void) {
  rgb_matrix_enable();
  rgb_matrix_mode(RGB_MATRIX_CYCLE_LEFT_RIGHT);
  init_dynamic_mt_states(mt_states, ARRAY_SIZE(mt_states));

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
      }
      return false;  // Skip default processing

    case QK_MODS ... QK_MODS_MAX:
      // Mouse keys with modifiers work inconsistently across operating systems, this makes sure that modifiers are always
      // applied to the mouse key that was pressed.
      if (IS_MOUSE_KEYCODE(QK_MODS_GET_BASIC_KEYCODE(keycode))) {
        if (record->event.pressed) {
          add_mods(QK_MODS_GET_MODS(keycode));
          send_keyboard_report();
          wait_ms(2);
          register_code(QK_MODS_GET_BASIC_KEYCODE(keycode));
          return false;
        } else {
          wait_ms(2);
          del_mods(QK_MODS_GET_MODS(keycode));
        }
      }
      break;
    case DRAG_SCROLL:
      if (record->event.pressed) {
        set_scrolling = true;
      } else {
        set_scrolling = false;
      }
      return false;
    case TOGGLE_SCROLL:
      if (record->event.pressed) {
        set_scrolling = !set_scrolling;
      }
      return false;
    case NAVIGATOR_TURBO:
      if (record->event.pressed) {
        navigator_turbo = true;
      } else {
        navigator_turbo = false;
      }
      return false;
    case NAVIGATOR_AIM:
      if (record->event.pressed) {
        navigator_aim = true;
      } else {
        navigator_aim = false;
      }
      return false;
    case NAVIGATOR_INC_CPI:
      if (record->event.pressed) {
        pointing_device_set_cpi(1);
      }
      return false;
    case NAVIGATOR_DEC_CPI:
      if (record->event.pressed) {
        pointing_device_set_cpi(0);
     }
    return false;

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
          set_mods(mod_state);
          return false;
        } else {
          tap_code16(KC_TILD);
          tap_code16(KC_SPACE);
          set_mods(mod_state);
          return true;
        }
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

    case CU_PSCR:
      return win_or_mac(KC_PSCR, KC_F13, is_mac, REMOVE_MODS, record);

    case CU_PGUP:
      return win_or_mac(KC_PGUP, G(KC_UP), is_mac, !REMOVE_MODS, record); // do not remove mods

    case CU_PGDOWN:
      return win_or_mac(KC_PGDN, G(KC_DOWN), is_mac, !REMOVE_MODS, record); // do not remove mods

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
          del_mods(mod_state);
          tap_code16(KC_ASTERISK);
          set_mods(mod_state);
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

    case CU_LCTL:
      if (record->event.pressed) {
        register_code16(is_mac ? KC_LCTL : KC_LGUI);
      } else {
        unregister_code16(is_mac ? KC_LCTL : KC_LGUI);
      }
      return false;

    case CU_LCMD:
      if (record->event.pressed) {
        register_code16(is_mac ? KC_LGUI : KC_LCTL);
      } else {
        unregister_code16(is_mac ? KC_LGUI : KC_LCTL);
      }
      return false;

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

  // Handle dynamic mod-tap keys
  if (!process_dynamic_mt(keycode, record)) {
    return false; // Key was handled by dynamic MT system
  }

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

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
  if (set_scrolling) {
    mouse_report.v = -mouse_report.v;
  }
  return mouse_report;
}
