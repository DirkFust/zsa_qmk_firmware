// clang-format off
// enum tap_dance_codes {
//     TD_TAB,
// };

typedef struct {
    bool    is_press_action;
    uint8_t step;
} tap;

enum {
    SINGLE_TAP = 1,
    SINGLE_HOLD,
    DOUBLE_TAP,
    DOUBLE_HOLD,
    DOUBLE_SINGLE_TAP,
    MORE_TAPS
};

static tap dance_state[1];

uint8_t dance_step(qk_tap_dance_state_t *state);

uint8_t dance_step(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed)
            return SINGLE_TAP;
        else
            return SINGLE_HOLD;
    } else if (state->count == 2) {
        if (state->interrupted)
            return DOUBLE_SINGLE_TAP;
        else if (state->pressed)
            return DOUBLE_HOLD;
        else
            return DOUBLE_TAP;
    }
    return MORE_TAPS;
}

// void on_dance_decimal_point(qk_tap_dance_state_t *state, void *user_data);
// void dance_finished_decimal_point(qk_tap_dance_state_t *state, void *user_data);
// void dance_decimal_point_reset(qk_tap_dance_state_t *state, void *user_data);

// void on_dance_decimal_point(qk_tap_dance_state_t *state, void *user_data) {
//     if (state->count == 3) {
//         tap_code16(KC_KP_DOT);
//         tap_code16(KC_KP_DOT);
//         tap_code16(KC_KP_DOT);
//     }
//     if (state->count > 3) {
//         tap_code16(KC_KP_DOT);
//     }
// }

// void dance_finished_decimal_point(qk_tap_dance_state_t *state, void *user_data) {
//     dance_state[0].step = dance_step(state);
//     switch (dance_state[0].step) {
//         case SINGLE_TAP:
//             register_code16(KC_KP_DOT);
//             break;
//         case DOUBLE_TAP:
//             register_code16(KC_KP_COMMA);
//             break;
//         case DOUBLE_SINGLE_TAP:
//             tap_code16(KC_KP_DOT);
//             register_code16(KC_KP_DOT);
//     }
// }

// void dance_decimal_point_reset(qk_tap_dance_state_t *state, void *user_data) {
//     wait_ms(10);
//     switch (dance_state[0].step) {
//         case SINGLE_TAP:
//             unregister_code16(KC_KP_DOT);
//             break;
//         case DOUBLE_TAP:
//             unregister_code16(KC_KP_COMMA);
//             break;
//         case DOUBLE_SINGLE_TAP:
//             unregister_code16(KC_KP_DOT);
//             break;
//     }
//     dance_state[0].step = 0;
// }

// qk_tap_dance_action_t tap_dance_actions[] = {
//     [TD_DECP] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_decimal_point, dance_finished_decimal_point, dance_decimal_point_reset),
// };