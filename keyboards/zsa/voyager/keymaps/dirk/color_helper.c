// Copyright 2026 Dirk (@DirkFust)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "color_helper.h"

uint32_t change_led_effect_heatmap_callback(uint32_t trigger_time, void *cb_arg) {
    rgb_matrix_mode(RGB_MATRIX_TYPING_HEATMAP);
    return 0;
}
