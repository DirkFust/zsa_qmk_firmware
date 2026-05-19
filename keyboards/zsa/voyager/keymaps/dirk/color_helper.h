#pragma once
#include QMK_KEYBOARD_H

// defer_exec callback that switches the RGB matrix into typing-heatmap mode.
uint32_t change_led_effect_heatmap_callback(uint32_t trigger_time, void *cb_arg);
