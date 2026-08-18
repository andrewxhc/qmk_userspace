// Copyright 2026 andrewxhc
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include QMK_KEYBOARD_H

enum andrewxhc_layers {
    DSEND,
    QWERTY,
    SYM,
    NAV,
    NUM,
    SYS,
    MOU,
    BTN,
    APP,
    FN,
};

enum andrewxhc_keycodes {
    U_ESC_TAB = QK_USER,
    U_OS_START,
    U_OS_END,
    U_OS_LEFT,
    U_OS_RIGHT,
    U_OS_UP,
    U_OS_DOWN,
    U_OS_WIN,
    U_OS_MAC,
    U_OS_LIN,
    U_CUT,
    U_COPY,
    U_PASTE,
    U_UNDO,
    U_REDO,
    U_SELECT_ALL,
    U_FIND,
    U_NEW_TAB,
    U_NEW_WINDOW,
    U_PRIVATE_WINDOW,
    U_ADDRESS_BAR,
    U_DESKTOP_PREV,
    U_DESKTOP_MANAGER,
    U_DESKTOP_NEXT,
    U_TILE_LEFT,
    U_TILE_FILL,
    U_TILE_RIGHT,
    U_TILE_RESTORE,
    U_MONITOR_NEXT,
    U_MINIMIZE,
    U_SCREENSHOT_FULL,
    U_SCREENSHOT_WINDOW,
    U_SCREENSHOT_REGION,
    U_SMART_MOUSE,
    U_SMART_BUTTON,
    U_CANCEL,
};

// Cyclotab trigger keycodes. The module holds Alt between repeated presses
// and releases it when the APP layer-tap is released.
#define U_SWAPPER A(KC_TAB)
#define U_WINDOW_SWAPPER A(KC_GRV)

// Basic-key placeholders let QMK's native tap-hold engine resolve the hold
// side. Their taps are intercepted in process_record_user().
#define U_APP_LAUNCHER LT(APP, KC_F20)
#define U_SMART_NUM LT(NUM, KC_F21)
#define U_SYM_SPACE LT(SYM, KC_F22)
#define U_NAV_BSPC LT(NAV, KC_F23)
#define U_MAGIC_SHIFT LSFT_T(KC_F24)
#define U_NAV_ENTER LT(NAV, KC_ENT)

#define U_HML_GUI(kc) LGUI_T(kc)
#define U_HML_ALT(kc) LALT_T(kc)
#define U_HML_SFT(kc) LSFT_T(kc)
#define U_HML_CTL(kc) LCTL_T(kc)
#define U_HMR_CTL(kc) LCTL_T(kc)
#define U_HMR_SFT(kc) RSFT_T(kc)
#define U_HMR_ALT(kc) LALT_T(kc)
#define U_HMR_GUI(kc) LGUI_T(kc)

#define U_MEH(kc) LCTL(LALT(LSFT(kc)))

// Tap-hold results emitted by the overlapping leader combos.
#define U_COMBO_TAB MT(MOD_LSFT | MOD_LALT, KC_TAB)
#define U_COMBO_LEADER MT(MOD_LCTL | MOD_LSFT, KC_F19)
#define U_COMBO_SHIFT_LEADER MT(MOD_LCTL | MOD_LSFT | MOD_LALT, KC_F18)

void andrewxhc_combo_note_keypress(void);
void andrewxhc_combo_note_event(keyrecord_t *record);
void andrewxhc_combo_task(void);
void andrewxhc_consume_oneshot_num(void);
void andrewxhc_num_word_break(void);
void andrewxhc_repeat_cancel(void);
void andrewxhc_repeat_note_passthrough(uint16_t keycode, keyrecord_t *record);
void andrewxhc_leader_start(bool shifted);
bool andrewxhc_leader_is_active(void);
bool andrewxhc_leader_process(uint16_t keycode, keyrecord_t *record);

// Place the source Corne-ish Zen 3x6+3 layout on the lower three Sofle rows.
// The number row, encoder switches, and four outer thumb switches are disabled.
#define LAYOUT_ZEN(                                                                                      \
    K_LT5, K_LT4, K_LT3, K_LT2, K_LT1, K_LT0, K_RT0, K_RT1, K_RT2, K_RT3, K_RT4, K_RT5,              \
    K_LM5, K_LM4, K_LM3, K_LM2, K_LM1, K_LM0, K_RM0, K_RM1, K_RM2, K_RM3, K_RM4, K_RM5,              \
    K_LB5, K_LB4, K_LB3, K_LB2, K_LB1, K_LB0, K_RB0, K_RB1, K_RB2, K_RB3, K_RB4, K_RB5,              \
                         K_LH2, K_LH1, K_LH0, K_RH0, K_RH1, K_RH2                                     \
) LAYOUT(                                                                                                \
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,             KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,   \
    K_LT5, K_LT4, K_LT3, K_LT2, K_LT1, K_LT0,             K_RT0, K_RT1, K_RT2, K_RT3, K_RT4, K_RT5,   \
    K_LM5, K_LM4, K_LM3, K_LM2, K_LM1, K_LM0,             K_RM0, K_RM1, K_RM2, K_RM3, K_RM4, K_RM5,   \
    K_LB5, K_LB4, K_LB3, K_LB2, K_LB1, K_LB0, KC_NO, KC_NO, K_RB0, K_RB1, K_RB2, K_RB3, K_RB4, K_RB5, \
           KC_NO, KC_NO, K_LH2, K_LH1, K_LH0,             K_RH0, K_RH1, K_RH2, KC_NO, KC_NO            \
)
