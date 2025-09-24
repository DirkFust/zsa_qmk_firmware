/*
 *  Set any config.h overrides for your specific keymap here.
 *  See config.h options at https://docs.qmk.fm/#/config_options?id=the-configh-file
 */
#define USB_SUSPEND_WAKEUP_DELAY 0
#define CHORDAL_HOLD
#define PERMISSIVE_HOLD

/*
 * Edit the layout under https://configure.zsa.io/voyager/layouts, so that it looks like what you want, then compile and download.
 * The name of the downloaded file is something like "zsa_voyager_dirk_al9ov_BBnJA.bin". Alternativly, it can be seen here: https://configure.zsa.io/my_layout/view/al9ov
 * The last two parts go into the firmware version here, seperated by a "/"
 * This makes Keymapp use the keyboard layout
 */
#define SERIAL_NUMBER "al9ov/QzBV5V"
#define LAYER_STATE_8BIT
#define ONESHOT_TAP_TOGGLE 3 /* Tapping this number of times holds the key until tapped once again. */
#define ONESHOT_TIMEOUT 5000 /* Time (in ms) before the one shot key is released */

// enables the status LEDs. Use STATUS_LED_1(true) to STATUS_LED_4(false). 1 (red) and 2 (green) are left, 3 (red) and 4 (green) are right
#define VOYAGER_USER_LEDS

// Navigator Trackball
#define WHEEL_EXTENDED_SUPPORT
#define POINTING_DEVICE_HIRES_SCROLL_ENABLE
#define POINTING_DEVICE_HIRES_SCROLL_EXPONENT 1
#define POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
#define MOUSE_EXTENDED_REPORT
#define NAVIGATOR_SCROLL_DIVIDER 50

#define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#define AUTO_MOUSE_DEFAULT_LAYER 3
#define AUTO_MOUSE_THRESHOLD 10
#define AUTO_MOUSE_SCROLL_THRESHOLD AUTO_MOUSE_THRESHOLD / NAVIGATOR_SCROLL_DIVIDER
#define RGB_MATRIX_STARTUP_SPD 60

/************************************************************************************************************************
 *    COMBOS                                                                                                            *
 * See https://docs.qmk.fm/#/feature_combo                                                                              *
 ************************************************************************************************************************/
// #define COMBO_COUNT 8
#define COMBO_ONLY_FROM_LAYER 0

// how quickly all combo keys must be pressed in succession to trigger (ms)
#define COMBO_TERM 60

// how long (ms) at least one of the combo keys must be held to trigger
#define COMBO_HOLD_TERM 150

// if a combo triggers a modifier, only trigger when the combo is held
#define COMBO_MUST_HOLD_MODS

#define TAPPING_TERM_PER_KEY
#define TAPPING_TERM 200 // in ms

#define RGB_MATRIX_STARTUP_SPD 60

#undef ENABLE_RGB_MATRIX_BAND_SAT
#undef ENABLE_RGB_MATRIX_BAND_VAL
#undef ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
#undef ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
#undef ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#undef ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#undef ENABLE_RGB_MATRIX_CYCLE_ALL
#undef ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#undef ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#undef ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
#undef ENABLE_RGB_MATRIX_DUAL_BEACON
#undef ENABLE_RGB_MATRIX_RAINDROPS
#undef ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
#undef ENABLE_RGB_MATRIX_HUE_BREATHING
#undef ENABLE_RGB_MATRIX_HUE_PENDULUM
#undef ENABLE_RGB_MATRIX_HUE_WAVE
#undef ENABLE_RGB_MATRIX_PIXEL_FRACTAL
#undef ENABLE_RGB_MATRIX_PIXEL_FLOW
#undef ENABLE_RGB_MATRIX_PIXEL_RAIN
#undef ENABLE_RGB_MATRIX_DIGITAL_RAIN
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#undef ENABLE_RGB_MATRIX_SPLASH
#undef ENABLE_RGB_MATRIX_MULTISPLASH
#undef ENABLE_RGB_MATRIX_SOLID_SPLASH
#undef ENABLE_RGB_MATRIX_SOLID_MULTISPLASH
