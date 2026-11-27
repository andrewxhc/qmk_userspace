// Copyright 2026 andrewxhc
// SPDX-License-Identifier: GPL-2.0-only

#include "andrewxhc.h"

#define ___ KC_TRNS

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [DSEND] = LAYOUT_ZEN(
        // LT5          LT4             LT3             LT2             LT1             LT0              RT0            RT1             RT2             RT3             RT4             RT5
        U_ESC_TAB,      KC_X,           KC_C,           KC_L,           KC_F,           KC_V,            KC_QUOT,        KC_SCLN,         KC_U,           KC_P,           KC_Q,           MO(FIG),
        U_OS_START,     U_HML_GUI(KC_R), U_HML_ALT(KC_S), U_HML_SFT(KC_N), U_HML_CTL(KC_T), KC_G,          KC_DOT,         U_HMR_CTL(KC_A), U_HMR_SFT(KC_E), U_HMR_ALT(KC_H), U_HMR_GUI(KC_I), U_OS_END,
        KC_LCTL,        KC_Z,           KC_W,           KC_M,           KC_D,           KC_B,            KC_COMM,        KC_O,            KC_Y,           KC_K,           KC_J,           KC_RGUI,
                                                        U_APP_LAUNCHER, U_MAGIC_SHIFT,   U_NAV_BSPC,      U_NAV_ENTER,    U_SYM_SPACE,     U_SMART_NUM
    ),

    [QWERTY] = LAYOUT_ZEN(
        U_ESC_TAB,      KC_Q,           KC_W,           KC_E,           KC_R,           KC_T,            KC_Y,           KC_U,            KC_I,           KC_O,           KC_P,           MO(FIG),
        U_OS_START,     U_HML_GUI(KC_A), U_HML_ALT(KC_S), U_HML_SFT(KC_D), U_HML_CTL(KC_F), KC_G,          KC_H,           U_HMR_CTL(KC_J), U_HMR_SFT(KC_K), U_HMR_ALT(KC_L), U_HMR_GUI(KC_SCLN), U_OS_END,
        KC_LCTL,        KC_Z,           KC_X,           KC_C,           KC_V,           KC_B,            KC_N,           KC_M,            KC_COMM,        KC_DOT,         KC_QUOT,        KC_RGUI,
                                                        U_APP_LAUNCHER, U_MAGIC_SHIFT,   U_NAV_BSPC,      U_NAV_ENTER,    U_SYM_SPACE,     U_SMART_NUM
    ),

    [SYM] = LAYOUT_ZEN(
        ___,            KC_AMPR,        KC_AT,          KC_HASH,        KC_DLR,         KC_CIRC,         KC_DQUO,        KC_LPRN,         KC_RPRN,        KC_LBRC,        KC_RBRC,        ___,
        KC_PERC,        KC_PLUS,        KC_MINS,        KC_ASTR,        KC_SLSH,        KC_EQL,          KC_EXLM,        ___,             ___,            ___,            ___,            ___,
        ___,            KC_TILD,        KC_PIPE,        KC_UNDS,        KC_COLN,        KC_BSLS,         KC_QUES,        KC_LCBR,         KC_RCBR,        KC_LT,          KC_GT,          ___,
                                                        ___,            ___,             ___,             ___,            ___,            ___
    ),

    [NAV] = LAYOUT_ZEN(
        ___,            U_SCREENSHOT_FULL, U_SCREENSHOT_WINDOW, U_SCREENSHOT_REGION, U_TAB_PREV, U_TAB_NEXT,   U_ZOOM_IN,      U_ADDRESS_BAR,   MS_BTN4,        MS_BTN5,        U_RELOAD,       KC_PGUP,
        ___,            U_REDO,          ___,             U_SELECT_ALL,    U_FIND,          U_CLOSE,         U_ZOOM_RESET,   U_OS_LEFT,       U_OS_UP,         U_OS_DOWN,       U_OS_RIGHT,     ___,
        ___,            U_UNDO,          U_CUT,           U_COPY,          U_PASTE,         KC_INS,          U_ZOOM_OUT,     U_NEW_TAB,       U_NEW_WINDOW,    U_PRIVATE_WINDOW, U_REOPEN_TAB,  KC_PGDN,
                                                        ___,             ___,             ___,             ___,            ___,            ___
    ),

    [NUM] = LAYOUT_ZEN(
        ___,            ___,             KC_7,            KC_8,            KC_9,            KC_ASTR,         ___,            ___,             ___,            ___,            ___,            ___,
        ___,            KC_PLUS,         KC_4,            KC_5,            KC_6,            KC_EQL,          ___,            ___,             ___,            ___,            ___,            ___,
        ___,            KC_MINS,         KC_1,            KC_2,            KC_3,            KC_SLSH,         ___,            ___,             ___,            ___,            ___,            ___,
                                                        LT(APP, KC_DOT), LSFT_T(KC_0),   ___,             ___,            ___,            ___
    ),

    [FIG] = LAYOUT_ZEN(
        U_CANCEL,       ___,             ___,             ___,             ___,             ___,             KC_MPRV,        KC_MPLY,         KC_MNXT,        MO(MNT),        KC_F22,         ___,
        ___,            U_LANG_EN,       U_LANG_PINYIN,   U_LANG_NEXT,     ___,             ___,             KC_VOLD,        KC_MUTE,         KC_VOLU,        ___,            KC_F23,         KC_F24,
        ___,            U_OS_WIN,        U_OS_MAC,        U_OS_LIN,        U_SMART_BUTTON,  U_SMART_MOUSE,   ___,            ___,             ___,            ___,            TO(DSEND),      TO(QWERTY),
                                                        ___,             ___,             ___,             ___,            ___,            ___
    ),

    [MOU] = LAYOUT_ZEN(
        ___,            ___,             ___,             ___,             ___,             ___,             ___,            KC_PGUP,         MS_UP,           KC_PGDN,        ___,            U_SMART_MOUSE,
        ___,            ___,             ___,             ___,             ___,             ___,             MS_WHLL,        MS_LEFT,         MS_DOWN,         MS_RGHT,         MS_WHLR,        ___,
        ___,            ___,             ___,             ___,             ___,             ___,             ___,            MS_BTN1,         MS_BTN3,         MS_BTN2,         ___,            ___,
                                                        ___,             ___,             ___,             MS_WHLU,        MS_WHLD,        ___
    ),

    [BTN] = LAYOUT_ZEN(
        ___,            U_UNDO,          U_CUT,           U_COPY,          U_PASTE,         U_REDO,          U_REDO,         U_PASTE,         U_COPY,         U_CUT,          U_UNDO,         U_SMART_BUTTON,
        ___,            KC_LGUI,         KC_LALT,         KC_LSFT,         KC_LCTL,         ___,             ___,            KC_LCTL,         KC_LSFT,        KC_LALT,        KC_LGUI,        ___,
        ___,            U_UNDO,          U_CUT,           U_COPY,          U_PASTE,         U_REDO,          U_REDO,         U_PASTE,         U_COPY,         U_CUT,          U_UNDO,         ___,
                                                        MS_BTN3,         MS_BTN1,         MS_BTN2,         MS_BTN2,        MS_BTN1,        MS_BTN3
    ),

    [APP] = LAYOUT_ZEN(
        U_SWAPPER,      S(U_SWAPPER),     U_DESKTOP_PREV,  U_DESKTOP_MANAGER, U_DESKTOP_NEXT, ___,            U_MEH(KC_A),    U_MEH(KC_B),     U_MEH(KC_C),    U_MEH(KC_D),    U_MEH(KC_E),    ___,
        U_WINDOW_SWAPPER, KC_LSFT,       U_TILE_LEFT,     U_TILE_FILL,     U_TILE_RIGHT,    U_OS_QUIT,       U_MEH(KC_F),    U_MEH(KC_G),     U_MEH(KC_H),    U_MEH(KC_I),    U_MEH(KC_J),    ___,
        ___,            ___,             U_MONITOR_NEXT,  U_MINIMIZE,      U_TILE_RESTORE,  ___,             U_MEH(KC_K),    U_MEH(KC_L),     U_MEH(KC_M),    U_MEH(KC_N),    U_MEH(KC_O),    ___,
                                                        ___,             ___,             ___,             ___,            ___,            ___
    ),

    [FN] = LAYOUT_ZEN(
        ___,            KC_F12,          KC_F7,           KC_F8,           KC_F9,           KC_PSCR,         ___,            ___,             ___,            ___,            ___,            ___,
        ___,            KC_F11,          KC_F4,           KC_F5,           KC_F6,           KC_SCRL,         ___,            ___,             ___,            ___,            ___,            ___,
        ___,            KC_F10,          KC_F1,           KC_F2,           KC_F3,           KC_PAUS,         ___,            ___,             ___,            ___,            ___,            ___,
                                                        ___,             ___,             ___,             ___,            ___,            ___
    ),

    // Maintenance: reached by holding MO(MNT) on FIG. Every other position is
    // a dead KC_NO, matching the source's XXX (&none) entries.
    [MNT] = LAYOUT_ZEN(
        KC_NO,          KC_NO,           KC_NO,           KC_NO,           KC_NO,           KC_NO,           KC_NO,          KC_NO,           KC_NO,          KC_NO,          KC_NO,          KC_NO,
        KC_NO,          KC_NO,           KC_NO,           KC_NO,           KC_NO,           KC_NO,           KC_NO,          KC_NO,           KC_NO,          KC_NO,          KC_NO,          KC_NO,
        KC_NO,          KC_NO,           KC_NO,           KC_NO,           QK_BOOT,         QK_REBOOT,       QK_REBOOT,      QK_BOOT,         KC_NO,          KC_NO,          KC_NO,          KC_NO,
                                                        KC_NO,           KC_NO,           KC_NO,           KC_NO,          KC_NO,          KC_NO
    ),
};
