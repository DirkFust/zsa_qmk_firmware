# Copyright 2026 Dirk (@DirkFust)
# SPDX-License-Identifier: GPL-2.0-or-later

# Set any rules.mk overrides for your specific keymap here.
# See rules at https://docs.qmk.fm/#/config_options?id=the-rulesmk-file
CONSOLE_ENABLE = no
COMMAND_ENABLE = no
ORYX_ENABLE = yes
RGB_MATRIX_CUSTOM_KB = yes
SPACE_CADET_ENABLE = no
CAPS_WORD_ENABLE = yes
LAYER_LOCK_ENABLE = yes
TAP_DANCE_ENABLE = yes
# Trackpad is a DIGITIZER (PTP), loaded via the zsa/navigator_trackpad module in
# keymap.json. POINTING_DEVICE stays off; the trackball-coupled MOUSE-layer
# handlers in keymap.c are #ifdef POINTING_DEVICE_ENABLE-guarded out for now.
POINTING_DEVICE_ENABLE = no
COMBO_ENABLE = yes
DEFERRED_EXEC_ENABLE = yes
RGB_MATRIX_CUSTOM_USER = yes
OS_DETECTION_ENABLE = yes


SRC += dynamic_mt.c
SRC += tap_hold_helper.c
SRC += color_helper.c
