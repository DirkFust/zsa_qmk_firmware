// Documentation: https://docs.qmk.fm/#/feature_rgb_matrix?id=custom-rgb-matrix-effects

// For inspiration and examples, check out the built-in effects under quantum/rgb_matrix/animations/.

// Step 1.
// Declare custom effects using the RGB_MATRIX_EFFECT macro
// (note the lack of semicolon after the macro!)
RGB_MATRIX_EFFECT(overwatch)

// Step 2.
// Define effects inside the `RGB_MATRIX_CUSTOM_EFFECT_IMPLS` ifdef block
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// e.g: A simple effect, self-contained within a single method
static bool overwatch(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    #ifdef CONSOLE_ENABLE
      uprintf("overwatch(): min %u, max %u\n", led_min, led_max);
    #endif

    HSV red     = {0, 255, 255};
    HSV green   = {85, 255, 255};
    HSV blue    = {170, 255, 255};
    HSV magenta = {213, 255, 255};
    HSV yellow  = {41,255,255};
    HSV white   = {0, 0, 64};
    // HSV off     = {0, 0, 0};

    // RGB rgb = rgb_matrix_hsv_to_rgb(rgb_matrix_config.hsv);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        HSV color;

        switch (i) {
            case 31:
                color = red;
                break;
            case 9:
            case 14 ... 16:
                color = green;
                break;
            case 10:
            case 13:
                color = blue;
                break;
            case 22:
                color = yellow;
                break;
            case 8:
                color = magenta;
                break;
            default:
                color = white;
                break;
        }

        RGB rgb = rgb_matrix_hsv_to_rgb(color);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif