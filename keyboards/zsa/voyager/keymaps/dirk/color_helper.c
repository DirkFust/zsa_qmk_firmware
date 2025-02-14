uint32_t change_color_red_callback(uint32_t trigger_time, void *cb_arg) {
    rgb_matrix_sethsv_noeeprom(HSV_RED);
    return 0;
}

uint32_t change_color_green_callback(uint32_t trigger_time, void *cb_arg) {
    rgb_matrix_sethsv_noeeprom(HSV_GREEN);
    return 0;
}

uint32_t change_color_blue_callback(uint32_t trigger_time, void *cb_arg) {
    rgb_matrix_sethsv_noeeprom(HSV_BLUE);
    return 0;
}

uint32_t change_led_effect_heatmap_callback(uint32_t trigger_time, void *cb_arg) {
    rgb_matrix_mode(RGB_MATRIX_TYPING_HEATMAP);
    return 0;
}

uint32_t set_color(HSV hsv) {
    rgb_matrix_sethsv_noeeprom(hsv.h, hsv.s, hsv.v);
    return 0;
}